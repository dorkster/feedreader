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

typedef struct Article {
    char *name;
    char *uri;
}Article;

typedef struct Feed {
    char *name;
    char *uri;
    Article **articles;
    int article_count;
}Feed;

int feed_count;
Feed **feed_list;

void add_feed(char *name, char *uri);
void remove_feed(Feed* feed);
void add_article(int i, char *name, char *uri);
void remove_article(Article* article);
void clear_feedlist();
void loadconfig();

#endif
