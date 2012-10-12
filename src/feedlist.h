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

#ifndef FEEDLIST_H
#define FEEDLIST_H

typedef struct ArticleList {
    int id;
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
int articlecount;
FeedList *feeds;

void add_feed(FeedList **list, char *name, char *uri);
void add_article(ArticleList **list, char *name, char *uri);
void loadconfig();

#endif
