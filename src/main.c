/*
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <curl/curl.h>
#include "icon.h"

#define UPDATE_INTERVAL 120 // in seconds
#define BROWSER "xdg-open"
#define MAXLABELSIZE 70
#define NETTIMEOUT 15 // in seconds

static GtkWidget *main_menu = NULL;
static GtkWidget *alt_menu = NULL;
static GtkStatusIcon *status_icon = NULL;

static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data);
static void add_FeedList(char *name, char *uri);
static void open_link(gpointer data);
static void parsefeed(const gchar *f, GtkWidget *submenu);
static gboolean loadconfig();
static void destroy(GtkWidget *widget, gpointer data);
static void primary_menu (GtkStatusIcon *status_icon, gpointer user_data);
static void alternate_menu(GtkStatusIcon *status_icon, guint button, guint activate_time, gpointer user_data);

struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t
WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)data;

    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        printf("not enough memory (realloc returned NULL)\n");
        exit(EXIT_FAILURE);
    }

    memcpy(&(mem->memory[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

typedef struct FeedList
{
    char *name;
    char *uri;
    GtkWidget *submenu;
    struct FeedList *next;
}FeedList;

FeedList *feeds = NULL;

static void
add_FeedList(char *name, char *uri)
{
    FeedList *temp;
    
    temp = (FeedList *)malloc(sizeof(FeedList));
    temp->name = name;
    temp->uri = uri;
    temp->submenu = gtk_menu_new();
    
    if(feeds == NULL)
    {
        feeds = temp;
        feeds->next = NULL;
    }
    else
    {
        temp->next = feeds;
        feeds = temp;
    }
}

static void
open_link(gpointer data)
{
    gchar *link = gtk_widget_get_tooltip_text(data);
    gchar *command = g_strconcat(BROWSER," \"",link,"\" &",NULL);
    g_print("%s\n",command);
    system(command);
    g_free(command);
}

static void
parsefeed(const gchar *f, GtkWidget *submenu)
{
    xmlDocPtr file = NULL;
    xmlNodePtr node;
    
    CURL *curl_handle;

    struct MemoryStruct chunk;

    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_global_init(CURL_GLOBAL_ALL);

    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, f);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, NETTIMEOUT);

    curl_easy_perform(curl_handle);
    curl_easy_cleanup(curl_handle);

    if(chunk.memory)
    {
        file = xmlParseMemory(chunk.memory,chunk.size);
        free(chunk.memory);
    }
    
    curl_global_cleanup();

    if (file == NULL )
    {
        fprintf(stderr,"Document not parsed successfully. \n");
        return;
    }
    
    node = xmlDocGetRootElement(file);

    if (node == NULL)
    {
        fprintf(stderr,"empty document\n");
        xmlFreeDoc(file);
        return;
    }

    if (xmlStrcmp(node->name, (const xmlChar *) "rss"))
    {
        fprintf(stderr,"document of the wrong type, root node != rss");
        xmlFreeDoc(file);
        return;
    }
    
    static GtkWidget *item;
    node = node->xmlChildrenNode;
    
    while (node != NULL)
    {
        if ((!xmlStrcmp(node->name, (const xmlChar *)"channel")))
        {
            xmlNodePtr child_item = node->xmlChildrenNode;
            while (child_item != NULL)
            {
                if ((!xmlStrcmp(child_item->name, (const xmlChar *)"item")))
                {
                    xmlChar *title;
                    xmlChar *link;
                    xmlNodePtr child_details = child_item->xmlChildrenNode;
                    while (child_details != NULL)
                    {
                        if ((!xmlStrcmp(child_details->name, (const xmlChar *)"title")))
                        {
                            title = xmlNodeListGetString(file, child_details->xmlChildrenNode, 1);
                            if(strlen((char *)title) > MAXLABELSIZE)
                            {
                                gchar *newtitle = g_strndup((const gchar *)title,MAXLABELSIZE);
                                item = gtk_menu_item_new_with_label(g_strconcat(newtitle,"...",NULL));
                                g_free(newtitle);
                            }
                            else
                            {
                                item = gtk_menu_item_new_with_label((const gchar *)title);
                            }
                            gtk_menu_append(submenu, item);
                            xmlFree(title);
                        }
                        if ((!xmlStrcmp(child_details->name, (const xmlChar *)"link")))
                        {
                            link = xmlNodeListGetString(file, child_details->xmlChildrenNode, 1);
                            gtk_widget_set_tooltip_text(item,(const gchar *)link);
                            g_signal_connect(G_OBJECT(item),"activate",G_CALLBACK(open_link),NULL);
                            xmlFree(link);
                        }

                        child_details = child_details->next;
                    }
                }
                child_item = child_item->next;
            }
        }
        node = node->next;
    }
    
    xmlFreeDoc(file);
    xmlFreeNode(node);

    return;
}

static gboolean
loadconfig()
{
    if(!gtk_widget_get_visible(main_menu))
    {
        if(feeds != NULL)
        {
            while(feeds != NULL)
            {
                gtk_widget_destroy(feeds->submenu);
                feeds = feeds->next;
            }
        }
        gtk_widget_destroy(main_menu);
        main_menu = gtk_menu_new();

        feeds = NULL;
        
        FILE *config;
        char *homedir = getenv("HOME");
        char *name;
        char *url;
        char buffer[BUFSIZ];
        char *temp;
        
        if ((config = fopen(g_strconcat(homedir,"/.rssfeeds",NULL),"r")) == NULL)
        {
            g_print("Error: can't open config");
            return TRUE;
        }
        
        while(fgets(buffer,BUFSIZ,config) != NULL)
        {        
            temp = buffer;
            name=strtok(temp,"\\");
            url=strtok(NULL,"\n");
            
            //This is awful, but for now it's the only way I'm passing these two strings
            if(url !=  NULL)
            {
                add_FeedList(g_strconcat(name,NULL),g_strconcat(url,NULL));
            }
        }
    
        fclose(config);
        
        GtkWidget *item;
        
        FeedList *feedlist = feeds;
        
        while(feedlist != NULL)
        {
            item = gtk_menu_item_new_with_label(feedlist->name);
            if(!strcmp(feedlist->uri,"LABEL"))
            {
                gtk_widget_set_sensitive(item,FALSE);
            }
            else
            {
                gtk_widget_set_tooltip_text(item,(const gchar *)feedlist->uri);
                gtk_widget_set_has_tooltip(item,FALSE);
                parsefeed(feedlist->uri,feedlist->submenu);
                gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), feedlist->submenu);
            }
            gtk_menu_prepend(main_menu, item);
            feedlist = feedlist->next;
        }
        
        GtkWidget *sep = gtk_separator_menu_item_new();
        item = gtk_image_menu_item_new_with_label("Reload Feeds");
        gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item),gtk_image_new_from_stock(GTK_STOCK_REFRESH, GTK_ICON_SIZE_MENU));
        g_signal_connect (G_OBJECT(item), "activate", G_CALLBACK(loadconfig), NULL);
        gtk_menu_append(main_menu, sep);
        gtk_menu_append(main_menu, item);
    }
    return TRUE;
}

static void 
destroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit ();
}

static void
primary_menu (GtkStatusIcon *status_icon, gpointer user_data)
{
    gtk_widget_show_all(main_menu);

    gtk_menu_popup(GTK_MENU(main_menu), NULL, NULL, gtk_status_icon_position_menu, status_icon, 0, gtk_get_current_event_time());

}

static void 
alternate_menu(GtkStatusIcon *status_icon, guint button, guint activate_time, gpointer user_data)
{
    if (!alt_menu)
    {
        GtkWidget *item;
        alt_menu = gtk_menu_new();

        item = gtk_menu_item_new_with_label("Quit");
        gtk_menu_append(alt_menu, item);
        g_signal_connect (G_OBJECT(item), "activate", G_CALLBACK(destroy), NULL);
    }

    gtk_widget_show_all(alt_menu);
    gtk_menu_popup(GTK_MENU(alt_menu), NULL, NULL, NULL, status_icon, button, activate_time);
}

int main( int argc, char* argv[] )
{
    gtk_init( &argc, &argv );
    
    // Set the status icon and make it visible
    status_icon = gtk_status_icon_new_from_pixbuf(gdk_pixbuf_new_from_inline(-1,icon,FALSE,NULL));
    gtk_status_icon_set_visible(status_icon, TRUE);

    gtk_status_icon_set_tooltip(status_icon, "feedreader");
    
    // Connect tray icon signals
    g_signal_connect (G_OBJECT (status_icon), "activate", G_CALLBACK (primary_menu), NULL);
    g_signal_connect (G_OBJECT (status_icon), "popup-menu", G_CALLBACK (alternate_menu), NULL);
    
    // Add a timer for updating feeds
    g_timeout_add_seconds(UPDATE_INTERVAL, (GSourceFunc) loadconfig, NULL);
    
    // Load feeds on startup
    loadconfig();
    
    gtk_main();

    return 0;
}
