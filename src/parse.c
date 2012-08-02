#include <glib.h>
#include <gtk/gtk.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <string.h>

#include "feedlist.h"
#include "parse.h"
#include "util.h"

void parsefeed(FeedList *_list) {
    FeedList *list = _list;
    GtkWidget *submenu = NULL;
    int id = 0;
    
    if(list != NULL) {
        submenu = list->submenu;
        id = list->id;
    }
    
    xmlDocPtr file = NULL;
    xmlNodePtr node;

    char filename[BUFSIZ];
    sprintf(filename,"/tmp/feedreader/%d",id);
    file = xmlParseFile(filename);
    
    if (file == NULL ) {
        fprintf(stderr,"Document not parsed successfully. \n");
        return;
    }
    
    node = xmlDocGetRootElement(file);
    
    if (node == NULL) {
       fprintf(stderr,"Empty document. \n");
        if(file != NULL) xmlFreeDoc(file);
        return;
    } else if ((xmlStrcmp(node->name, (const xmlChar *)"rss"))) {
        fprintf(stderr,"Root node is not 'rss'. \n");
        if(file != NULL) xmlFreeDoc(file);
        if(node != NULL) xmlFreeNode(node);
        return;
    }
    
    static GtkWidget *item;
    
    if(node != NULL) node = node->xmlChildrenNode;
    
    while (node != NULL) {
        if ((!xmlStrcmp(node->name, (const xmlChar *)"channel"))) {
            xmlNodePtr child_item = node->xmlChildrenNode;
            while (child_item != NULL) {
                if ((!xmlStrcmp(child_item->name, (const xmlChar *)"item"))) {
                    xmlChar *title;
                    xmlChar *link;
                    xmlNodePtr child_details = child_item->xmlChildrenNode;
                    while (child_details != NULL) {
                        if ((!xmlStrcmp(child_details->name, (const xmlChar *)"title"))) {
                            title = xmlNodeListGetString(file, child_details->xmlChildrenNode, 1);
                            if(strlen((char *)title) > MAXLABELSIZE) {
                                gchar *newtitle = g_strndup((const gchar *)title,MAXLABELSIZE);
                                item = gtk_menu_item_new_with_label(g_strconcat(newtitle,"...",NULL));
                                g_free(newtitle);
                            } else {
                                item = gtk_menu_item_new_with_label((const gchar *)title);
                            }
                            if(submenu != NULL) gtk_menu_append(submenu, item);
                            if(title != NULL) xmlFree(title);
                        }
                        if ((!xmlStrcmp(child_details->name, (const xmlChar *)"link"))) {
                            link = xmlNodeListGetString(file, child_details->xmlChildrenNode, 1);
                            gtk_widget_set_tooltip_text(item,(const gchar *)link);
                            g_signal_connect(G_OBJECT(item),"activate",G_CALLBACK(open_link),NULL);
                            if(link != NULL) xmlFree(link);
                        }

                        child_details = child_details->next;
                    }
                }
                child_item = child_item->next;
            }
        }
        node = node->next;
    }
    
    if(file != NULL) xmlFreeDoc(file);
    if(node != NULL) xmlFreeNode(node);
}

