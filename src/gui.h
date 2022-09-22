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

#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include <glib.h>

struct PrefWindow {
    GtkWidget* window;
    GtkWidget* button_ok;
    GtkWidget* button_cancel;
    GtkWidget* entry_browser;
    GtkWidget* spin_update;
    GtkWidget* spin_articles;
    GtkWidget* tree_feeds;
    GtkWidget* button_feed_remove;
    GtkWidget* button_feed_add;
    GtkWidget* button_feed_moveup;
    GtkWidget* button_feed_movedown;
};

extern GtkWidget *main_menu;
extern GtkWidget *alt_menu;
extern GtkStatusIcon *status_icon;
extern struct PrefWindow prefs;

void create_tray_icon();
gboolean create_primary_menu();
void destroy (GtkWidget *widget, gpointer data);
void primary_menu (GtkStatusIcon *status_icon, gpointer user_data);
void alternate_menu (GtkStatusIcon *status_icon, guint button, guint activate_time, gpointer user_data);
void create_pref_window();
void destroy_pref_window(GtkWidget *widget, gpointer data);
void show_pref_window(GtkWidget *widget, gpointer data);
void pref_feed_edited(GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data);
void pref_apply(GtkWidget* widget, gpointer data);
gboolean pref_feed_write_config(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data);
void pref_feed_remove(GtkWidget* widget, gpointer data);
void pref_feed_add(GtkWidget* widget, gpointer data);
void pref_feed_moveup(GtkWidget* widget, gpointer data);
void pref_feed_movedown(GtkWidget* widget, gpointer data);

#endif
