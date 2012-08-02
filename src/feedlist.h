#ifndef FEEDLIST_H
#define FEEDLIST_H

typedef struct ArticleList {
    char *name;
    char *uri;
    struct ArticleList *next;
}ArticleList;

typedef struct FeedList {
    int id;
    char *name;
    char *uri;
    ArticleList *articles;
    struct FeedList *next;
}FeedList;

int feedcount;
FeedList *feeds;

void add_feed(FeedList **list, char *name, char *uri);
void add_article(ArticleList **list, char *name, char *uri);
void loadconfig();

#endif
