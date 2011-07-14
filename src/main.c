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
#include "icon.h"

static GtkWidget *main_menu = NULL;
static GtkWidget *alt_menu = NULL;
static GtkWidget *art_menu = NULL;
static GtkStatusIcon *status_icon = NULL;
static xmlDoc *feedfile;
static xmlNode *feednode;

typedef struct UriList
{
    char *name;
    char *uri;
    struct UriList *next;
}UriList;

UriList *uris = NULL;

static void
add_UriList(char *name, char *uri)
{
    UriList *temp;
    
    temp = (UriList *)malloc(sizeof(UriList));
    temp->name = name;
    temp->uri = uri;
    
    if(uris == NULL)
    {
        uris = temp;
        uris->next = NULL;
    }
    else
    {
        temp->next = uris;
        uris = temp;
    }
}

static void
loadconfig()
{
    FILE *config;
    char *homedir = getenv("HOME");
    char *name;
    char *url;
    char buffer[BUFSIZ];
    char *temp;
    
    if ((config = fopen(g_strconcat(homedir,"/.rssfeeds",NULL),"r")) == NULL)
    {
        g_print("Error: can't open config");
        return;
    }
    
    while(fgets(buffer,BUFSIZ,config) != NULL)
    {        
        temp = buffer;
        name=strtok(temp,"\\");
        url=strtok(NULL,"\n");
        
        //This is awful, but for now it's the only way I'm passing these two strings
        add_UriList(g_strconcat(name,NULL),g_strconcat(url,NULL));
    }
    
    fclose(config);
}

static int
parsefeed(const gchar *f)
{
    xmlDocPtr file;
    xmlNodePtr node;
    
    file = xmlParseFile(f);

    if (file == NULL )
    {
        fprintf(stderr,"Document not parsed successfully. \n");
        return 1;
    }
    
    node = xmlDocGetRootElement(file);

    if (node == NULL)
    {
        fprintf(stderr,"empty document\n");
        xmlFreeDoc(file);
        return 1;
    }

    if (xmlStrcmp(node->name, (const xmlChar *) "rss"))
    {
        fprintf(stderr,"document of the wrong type, root node != rss");
        xmlFreeDoc(file);
        return 1;
    }
    
    feedfile = file;
    feednode = node;
    return 0;
}

static void 
destroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit ();
}

static void
open_link(gpointer data)
{
    gchar *link = gtk_widget_get_tooltip_text(data);
    gchar *command = g_strconcat("xdg-open \"",link,"\"",NULL);
    g_print("%s\n",command);
    system(command);
    g_free(command);
}

static GtkWidget *
article_menu(const gchar * url)
{
    if(parsefeed(url) == 0)
    {
        if (!art_menu)
        {
            static GtkWidget *item;
            art_menu = gtk_menu_new();
            feednode = feednode->xmlChildrenNode;
            
            while (feednode != NULL)
            {
                if ((!xmlStrcmp(feednode->name, (const xmlChar *)"channel")))
                {
                    xmlNodePtr child_item = feednode->xmlChildrenNode;
                    while (child_item != NULL)
                    {
                        //item = NULL;
                        if ((!xmlStrcmp(child_item->name, (const xmlChar *)"item")))
                        {
                            xmlChar *title;
                            xmlChar *link;
                            xmlNodePtr child_details = child_item->xmlChildrenNode;
                            while (child_details != NULL)
                            {
                                if ((!xmlStrcmp(child_details->name, (const xmlChar *)"title")))
                                {
                                    title = xmlNodeListGetString(feedfile, child_details->xmlChildrenNode, 1);
                                    item = gtk_menu_item_new_with_label((const gchar *)title);
                                    gtk_menu_append(art_menu, item);
                                    xmlFree(title);
                                }
                                if ((!xmlStrcmp(child_details->name, (const xmlChar *)"link")))
                                {
                                    link = xmlNodeListGetString(feedfile, child_details->xmlChildrenNode, 1);
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
                feednode = feednode->next;
            }
        }
    }
    else
    {
        art_menu = gtk_menu_new();
    }
    
    gtk_widget_show_all(art_menu);
    return art_menu;
}

static void
primary_menu_select(GtkMenuItem * item)
{
    GtkWidget * sub = gtk_menu_item_get_submenu(item);
    if (sub != NULL)
    {
        sub = article_menu(gtk_widget_get_tooltip_text((GtkWidget *)item));
        gtk_menu_item_set_submenu(item, sub);
    }
}

static void
primary_menu_deselect(GtkMenuItem * item)
{
    xmlFreeDoc(feedfile);
    xmlFreeNode(feednode);
    gtk_menu_item_set_submenu(item, gtk_menu_new());
    art_menu = NULL;
}

static void
primary_menu (GtkStatusIcon *status_icon, gpointer user_data)
{
    if (!main_menu)
    {
        GtkWidget *item;
        main_menu = gtk_menu_new();
        
        UriList *urilist = uris;
        
        while(urilist != NULL)
        {
            item = gtk_menu_item_new_with_label(urilist->name);
            gtk_widget_set_tooltip_text(item,(const gchar *)urilist->uri);
            gtk_widget_set_has_tooltip(item,FALSE);
            GtkWidget * dummy = gtk_menu_new();
            gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), dummy);
            gtk_menu_append(main_menu, item);
            g_signal_connect (G_OBJECT(item), "activate", G_CALLBACK(primary_menu_select), item);
            g_signal_connect (G_OBJECT(item), "deselect", G_CALLBACK(primary_menu_deselect), item);
            urilist = urilist->next;
        }
    }

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
    
    /* Connect signals */
    g_signal_connect (G_OBJECT (status_icon), "activate", G_CALLBACK (primary_menu), NULL);
    g_signal_connect (G_OBJECT (status_icon), "popup-menu", G_CALLBACK (alternate_menu), NULL);
    
    // Load rss feeds from $HOME/.rssfeeds
    loadconfig();
    
    gtk_main();

    return 0;
}
