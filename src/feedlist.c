#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "feedlist.h"
#include "util.h"

void add_feed(FeedList *list, char *name, char *uri) {
    FeedList *temp;
    
    temp = (FeedList *)malloc(sizeof(FeedList));
    temp->name = g_strdup(name);
    temp->uri = g_strdup(uri);
    temp->id = feedcount;
    feedcount++;
    
    if(list == NULL) {
        list = temp;
        list->next = NULL;
    } else {
        temp->next = list;
        list = temp;
    }
}

void add_article(ArticleList *list, char *name, char *uri) {
    ArticleList *temp;
    
    temp = (ArticleList *)malloc(sizeof(ArticleList));
    temp->name = g_strdup(name);
    temp->uri = g_strdup(uri);
    
    if(list == NULL) {
        list = temp;
        list->next = NULL;
    } else {
        temp->next = list;
        list = temp;
    }
}

void loadconfig() {
    if (feeds != NULL) feeds = NULL;

    feedcount = 0;

    FILE *config;
    char *name;
    char *url;
    char buffer[BUFSIZ];
    char *temp;
    
    system(g_strconcat("mkdir -p ",homedir,"/.config/feedreader",NULL));
    config = fopen(g_strconcat(homedir,"/.config/feedreader/rssfeeds",NULL),"r+");
    
    if(config != NULL) {
        while(fgets(buffer,BUFSIZ,config) != NULL) {
            temp = buffer;
            name=strtok(temp,"\\");
            url=strtok(NULL,"\n");
            
            if(url !=  NULL) {
                add_feed(feeds,name,url);
            }
        }

        fclose(config);
    }
    
}
