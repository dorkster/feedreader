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
    int id = 0;
    
    if(list != NULL) {
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
    
    if(node != NULL) node = node->xmlChildrenNode;
    
    xmlChar *title = NULL;
    xmlChar *link = NULL;
    xmlNodePtr child_item;
    xmlNodePtr child_details;

    while (node != NULL) {
        if ((!xmlStrcmp(node->name, (const xmlChar *)"channel"))) {
            child_item = node->xmlChildrenNode;
            while (child_item != NULL) {
                if ((!xmlStrcmp(child_item->name, (const xmlChar *)"item"))) {
                    child_details = child_item->xmlChildrenNode;
                    while (child_details != NULL) {
                        // article title
                        if ((!xmlStrcmp(child_details->name, (const xmlChar *)"title"))) {
                            title = xmlNodeListGetString(file, child_details->xmlChildrenNode, 1);
                            if(strlen((char *)title) > MAXLABELSIZE) {
                                gchar *newtitle = g_strndup((const gchar *)title,MAXLABELSIZE);
                                title = (xmlChar*)g_strdup(g_strconcat(newtitle,"...",NULL));
                                g_free(newtitle);
                            }
                        }
                        // article link
                        if ((!xmlStrcmp(child_details->name, (const xmlChar *)"link"))) {
                            link = xmlNodeListGetString(file, child_details->xmlChildrenNode, 1);
                        }

                        if (title != NULL && link != NULL) {
                            add_article(list->articles,(char*)title,(char*)link);
                            xmlFree(title);
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
    
    if(file != NULL) xmlFreeDoc(file);
    if(node != NULL) xmlFreeNode(node);
    if (child_details != NULL) xmlFreeNode(child_details);
    if (child_item != NULL) xmlFreeNode(child_item);
    if (title != NULL) xmlFree(title);
    if (link != NULL) xmlFree(link);
}

