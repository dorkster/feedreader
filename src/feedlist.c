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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "feedlist.h"
#include "util.h"

void add_feed(FeedList **list, char *name, char *uri) {
    FeedList *temp;
    
    temp = (FeedList *)malloc(sizeof(FeedList));
    temp->name = g_strdup(name);
    temp->uri = g_strdup(uri);
    temp->articles = NULL;
    temp->id = feedcount;
    feedcount++;
    
    if(*list == NULL) {
        *list = temp;
        (**list).next = NULL;
    } else {
        temp->next = *list;
        *list = temp;
    }
}

void add_article(ArticleList **list, char *name, char *uri) {
    ArticleList *temp;
    
    temp = (ArticleList *)malloc(sizeof(ArticleList));
    temp->name = g_strdup(name);
    temp->uri = g_strdup(uri);
    temp->id = articlecount;
    articlecount++;
    
    if(*list == NULL) {
        *list = temp;
        (**list).next = NULL;
    } else {
        temp->next = *list;
        *list = temp;
    }
}

void loadconfig() {
    // Clear the feeds list frist
    while (feeds != NULL) {
        free(feeds->name);
        free(feeds->uri);
        while (feeds->articles != NULL) {
            free(feeds->articles->name);
            free(feeds->articles->uri);
            feeds->articles = feeds->articles->next;
        }
        feeds = feeds->next;
    }

    feedcount = 0;

    FILE *config;
    char *name;
    char *url;
    char buffer[BUFSIZ];
    char *temp;
    
    config = fopen(g_strconcat(USER_DIR,"/rssfeeds",NULL),"r+");
    
    if(config != NULL) {
        while(fgets(buffer,BUFSIZ,config) != NULL) {
            temp = buffer;
            if (temp[0] == '#') continue;
            name=strtok(temp,"\\");
            url=strtok(NULL,"\n");
            
            if(url !=  NULL) {
                add_feed(&feeds,name,url);
            }
        }

        fclose(config);
    }
    
}
