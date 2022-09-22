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
#include "fileparser.h"
#include "util.h"

int feed_count = 0;
Feed** feed_list = NULL;

void add_feed(char *name, char *uri) {
    feed_list = (Feed**) realloc(feed_list, sizeof(Feed*) * (feed_count+1));
    if (feed_list) {
        feed_list[feed_count] = malloc(sizeof(Feed));
        if (feed_list[feed_count]) {
            feed_list[feed_count]->name = g_strdup(name);
            feed_list[feed_count]->uri = g_strdup(uri);
            feed_list[feed_count]->articles = NULL;
            feed_list[feed_count]->article_count = 0;
            feed_count++;
        }
    }
}

void remove_feed(Feed* feed) {
    if (feed) {
        if (feed->name) free(feed->name);
        if (feed->uri) free(feed->uri);

        while (feed->article_count > 0) {
            feed->article_count--;
            remove_article(feed->articles[feed->article_count]);
        }

        if (feed->articles) free(feed->articles);
        free(feed);
        feed_count--;
    }
}

void add_article(int i, char *name, char *uri) {
    feed_list[i]->articles = (Article**) realloc(feed_list[i]->articles, sizeof(Article*) * (feed_list[i]->article_count+1));
    if (feed_list[i]->articles) {
        feed_list[i]->articles[feed_list[i]->article_count] = malloc(sizeof(Article));
        if (feed_list[i]->articles[feed_list[i]->article_count]) {
            feed_list[i]->articles[feed_list[i]->article_count]->name = g_strdup(name);
            feed_list[i]->articles[feed_list[i]->article_count]->uri = g_strdup(uri);
            feed_list[i]->article_count++;
        }
    }
}

void remove_article(Article* article) {
    if (article) {
        if (article->name) free(article->name);
        if (article->uri) free(article->uri);
        free(article);
    }
}

void clear_feedlist() {
    while (feed_count > 0) {
        remove_feed(feed_list[feed_count-1]);
    }
    if (feed_list) free(feed_list);
    feed_list = NULL;
    feed_count = 0;
}

void loadconfig() {
    // Clear the feeds list frist
    clear_feedlist();

    FileParser* f;
    char* config_path = g_strconcat(USER_DIR,"config",NULL);

    if (config_path) {

        f = fileOpen(config_path);
        if (f) {
            while(fileNext(f)) {
                if (!strcmp("browser",fileGetKey(f))) set_browser(fileGetVal(f));
                else if (!strcmp("update_interval",fileGetKey(f))) UPDATE_INTERVAL = atoi(fileGetVal(f));
                else if (!strcmp("max_articles",fileGetKey(f))) MAX_ARTICLES = atoi(fileGetVal(f));
                else if (!strcmp("feed",fileGetKey(f))) {
                    char *name = fileEatNextVal(f);
                    char *url = fileEatNextVal(f);

                    if (url) {
                        add_feed(name,url);
                    }
                }
            }
            fileClose(f);
        } else {
            fprintf(stderr,"feedreader: Couldn't load config file at \"%s\".\n", config_path);

            FILE *out_file = fopen(config_path, "w+");
            if (out_file) {
                fprintf(out_file, "# settings\n");
                fprintf(out_file, "browser=%s\n", BROWSER);
                fprintf(out_file, "update_interval=%u\n", UPDATE_INTERVAL);
                fprintf(out_file, "max_articles=%u\n", MAX_ARTICLES);

                fprintf(out_file, "\n# feeds\n");
            }
        }

        free(config_path);
    }
}
