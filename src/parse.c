/*
    feedreader
    Copyright (C) 2011-2012  Justin Jacobs

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <glib.h>
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
    xmlNodePtr node = NULL;

    char filename[BUFSIZ];
    sprintf(filename,"%s/%d",TEMP_DIR,id);
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
                            add_article(&list->articles,(char*)title,(char*)link);
                            title = NULL;
                            link = NULL;
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

