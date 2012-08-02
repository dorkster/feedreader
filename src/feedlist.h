#ifndef FEEDLIST_H
#define FEEDLIST_H

#include <gtk/gtk.h>

typedef struct FeedList {
    int id;
    char *name;
    char *uri;
    GtkWidget *submenu;
    struct FeedList *next;
}FeedList;

int feedcount;
FeedList *feeds;

void add_FeedList(char *name, char *uri);
void loadconfig();

#endif
