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

#include <gtk/gtk.h>
#include <stdlib.h>

#include "util.h"

void open_link(gpointer data) {
    gchar *link = gtk_widget_get_tooltip_text(data);
    if(link != NULL) {
        gchar *command = g_strconcat(BROWSER," \"",link,"\" &",NULL);
        g_print("%s\n",command);
        system(command);
        g_free(command);
        g_free(link);
    }
}

void create_dirs() {
    // rssfeeds will go here
    USER_DIR = g_strconcat(getenv("XDG_CONFIG_HOME"),"/feedreader/",NULL);
    system(g_strconcat("mkdir -p ",USER_DIR,NULL));

    // A temp env variable isn't always used across distros, so this will be hard-coded for now
    TEMP_DIR = g_strconcat("/tmp/feedreader-",getenv("USER"),"/",NULL);
    system(g_strconcat("mkdir -p ",TEMP_DIR,NULL));
}

void clean_up() {
    system(g_strconcat("rm -rf ",TEMP_DIR,NULL));
}

