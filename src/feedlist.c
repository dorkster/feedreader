#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "feedlist.h"
#include "util.h"

void add_FeedList(char *name, char *uri) {
    FeedList *temp;
    
    temp = (FeedList *)malloc(sizeof(FeedList));
    temp->name = name;
    temp->uri = uri;
    temp->id = feedcount;
    feedcount++;
    temp->submenu = gtk_menu_new();
    
    if(feeds == NULL) {
        feeds = temp;
        feeds->next = NULL;
    } else {
        temp->next = feeds;
        feeds = temp;
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
                add_FeedList(g_strdup(name),g_strdup(url));
            }
        }

        fclose(config);
    }
    
}
