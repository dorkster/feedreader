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

#include <stdlib.h>

#include "feedlist.h"
#include "gui.h"
#include "parse.h"
#include "util.h"

GtkWidget *main_menu = NULL;
GtkWidget *alt_menu = NULL;
GtkStatusIcon *status_icon = NULL;
struct PrefWindow prefs = {};

void create_tray_icon() {
    // Set the status icon and make it visible
    GtkIconTheme *icontheme = gtk_icon_theme_get_default();
    status_icon = gtk_status_icon_new_from_pixbuf(gtk_icon_theme_load_icon(icontheme,"feedreader",-1,0,NULL));
    if (!gtk_status_icon_get_pixbuf(status_icon)) gtk_status_icon_set_from_file(status_icon, "/usr/share/pixmaps/feedreader.svg");
    if (!gtk_status_icon_get_pixbuf(status_icon)) gtk_status_icon_set_from_file(status_icon, "./feedreader.svg");
    if (!gtk_status_icon_get_pixbuf(status_icon)) gtk_status_icon_set_from_stock(status_icon, GTK_STOCK_EXECUTE);
    gtk_status_icon_set_visible(status_icon, TRUE);

    gtk_status_icon_set_tooltip(status_icon, "feedreader");

    // Connect tray icon signals
    g_signal_connect (G_OBJECT (status_icon), "activate", G_CALLBACK (primary_menu), NULL);
    g_signal_connect (G_OBJECT (status_icon), "popup-menu", G_CALLBACK (alternate_menu), NULL);

    // Add a timer for updating feeds
    g_timeout_add_seconds(UPDATE_INTERVAL, (GSourceFunc) create_primary_menu, NULL);
}

gboolean create_primary_menu() {

    if(!main_menu) {
        main_menu = gtk_menu_new();
        if (!main_menu) return FALSE;
    } else {
        gtk_container_forall( GTK_CONTAINER( main_menu ), (GtkCallback) gtk_widget_destroy, NULL );
    }

    loadconfig();

    GtkWidget *item;
    GtkWidget *submenu;
    GtkWidget *submenu_item;
 
    int i;
    for (i=0; i<feed_count; ++i) {
        item = gtk_menu_item_new_with_label(feed_list[i]->name);
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), NULL);

        gtk_widget_set_tooltip_text(item,(const gchar *)feed_list[i]->uri);
        gtk_widget_set_has_tooltip(item,FALSE);

        submenu = gtk_menu_new();

        if (parsefeed(i) == TRUE) {
            int j = (feed_list[i]->article_count <= MAX_ARTICLES ? feed_list[i]->article_count : MAX_ARTICLES);
            while(j>0) {
                j--;
                submenu_item = gtk_menu_item_new_with_label(feed_list[i]->articles[j]->name);
                gtk_widget_set_tooltip_text(submenu_item,feed_list[i]->articles[j]->uri);
                g_signal_connect(G_OBJECT(submenu_item),"activate",G_CALLBACK(open_link),NULL);
                gtk_menu_prepend(submenu,submenu_item);
            }
        }

        // we might have loaded the feed title from the RSS file, so update it
        gtk_menu_item_set_label(GTK_MENU_ITEM(item), feed_list[i]->name);

        gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), submenu);
        gtk_menu_append(main_menu, item);
    }

    GtkWidget *sep = gtk_separator_menu_item_new();
    item = gtk_image_menu_item_new_with_label("Reload Feeds");
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item),gtk_image_new_from_stock(GTK_STOCK_REFRESH, GTK_ICON_SIZE_MENU));
    g_signal_connect (G_OBJECT(item), "activate", G_CALLBACK(create_primary_menu), NULL);
    gtk_menu_append(main_menu, sep);
    gtk_menu_append(main_menu, item);

    return TRUE;
}

void destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit ();
}

void primary_menu (GtkStatusIcon *status_icon, gpointer user_data) {
    gtk_widget_show_all(main_menu);
    gtk_menu_popup(GTK_MENU(main_menu), NULL, NULL, gtk_status_icon_position_menu, status_icon, 0, gtk_get_current_event_time());
}

void alternate_menu(GtkStatusIcon *status_icon, guint button, guint activate_time, gpointer user_data) {
    if (!alt_menu) {
        alt_menu = gtk_menu_new();

        GtkWidget *item_pref = gtk_image_menu_item_new_with_label("Preferences");
        gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item_pref),gtk_image_new_from_stock(GTK_STOCK_PREFERENCES, GTK_ICON_SIZE_MENU));
        g_signal_connect(G_OBJECT(item_pref), "activate", G_CALLBACK(show_pref_window), NULL);

        GtkWidget *item_quit = gtk_image_menu_item_new_with_label("Quit");
        gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item_quit),gtk_image_new_from_stock(GTK_STOCK_QUIT, GTK_ICON_SIZE_MENU));
        g_signal_connect(G_OBJECT(item_quit), "activate", G_CALLBACK(destroy), NULL);

        gtk_menu_append(alt_menu, item_pref);
        gtk_menu_append(alt_menu, item_quit);
    }

    gtk_widget_show_all(alt_menu);
    gtk_menu_popup(GTK_MENU(alt_menu), NULL, NULL, NULL, status_icon, button, activate_time);
}

void create_pref_window() {
    prefs.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(prefs.window), 640, 480);
    gtk_window_set_title(GTK_WINDOW(prefs.window), "feedreader Preferences");
    g_signal_connect(G_OBJECT(prefs.window), "delete_event", G_CALLBACK(destroy_pref_window), NULL);

    GtkWidget* vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(prefs.window), vbox);

    // browser
    prefs.entry_browser = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(prefs.entry_browser), BROWSER);

    GtkWidget* label_browser = gtk_label_new("Browser");

    GtkWidget* hbox_browser = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_browser, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_browser), label_browser, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(hbox_browser), prefs.entry_browser, FALSE, FALSE, 0);

    // update interval
    prefs.spin_update = gtk_spin_button_new_with_range(10, UINT_MAX, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(prefs.spin_update), UPDATE_INTERVAL);

    GtkWidget* label_update = gtk_label_new("Update Interval (seconds)");

    GtkWidget* hbox_update = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_update, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_update), label_update, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(hbox_update), prefs.spin_update, FALSE, FALSE, 0);

    // max articles
    prefs.spin_articles = gtk_spin_button_new_with_range(1, UINT_MAX, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(prefs.spin_articles), MAX_ARTICLES);

    GtkWidget* label_articles = gtk_label_new("Max Articles per Feed");

    GtkWidget* hbox_articles = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_articles, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_articles), label_articles, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(hbox_articles), prefs.spin_articles, FALSE, FALSE, 0);

    // feeds
    GtkWidget *scroll_feeds = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_feeds), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    prefs.tree_feeds = gtk_tree_view_new();
    gtk_container_add(GTK_CONTAINER(scroll_feeds), prefs.tree_feeds);

    GtkCellRenderer *cell_renderer;

    cell_renderer = gtk_cell_renderer_text_new();
    g_object_set(cell_renderer, "editable", TRUE, NULL);
    g_object_set_data(G_OBJECT(cell_renderer), "my_column_num", GUINT_TO_POINTER(0));
    g_signal_connect(cell_renderer, "edited", G_CALLBACK(pref_feed_edited), NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(prefs.tree_feeds), -1, "Name", cell_renderer, "text", 0, NULL);

    cell_renderer = gtk_cell_renderer_text_new();
    g_object_set(cell_renderer, "editable", TRUE, NULL);
    g_object_set_data(G_OBJECT(cell_renderer), "my_column_num", GUINT_TO_POINTER(1));
    g_signal_connect(cell_renderer, "edited", G_CALLBACK(pref_feed_edited), NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(prefs.tree_feeds), -1, "URI", cell_renderer, "text", 1, NULL);

    GtkListStore *list_feeds = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    GtkTreeIter treeiter_feeds;

    int i;
    for (i = 0; i < feed_count; ++i) {
        gtk_list_store_append(list_feeds, &treeiter_feeds);
        gtk_list_store_set(list_feeds, &treeiter_feeds, 0, feed_list[i]->name, 1, feed_list[i]->uri, -1);
    }

    gtk_tree_view_set_model(GTK_TREE_VIEW(prefs.tree_feeds), GTK_TREE_MODEL(list_feeds));
    g_object_unref(list_feeds);

    prefs.button_feed_remove = gtk_button_new_from_stock(GTK_STOCK_DELETE);
    g_signal_connect(G_OBJECT(prefs.button_feed_remove), "clicked", G_CALLBACK(pref_feed_remove), NULL);

    prefs.button_feed_add = gtk_button_new_from_stock(GTK_STOCK_ADD);
    g_signal_connect(G_OBJECT(prefs.button_feed_add), "clicked", G_CALLBACK(pref_feed_add), NULL);

    GtkWidget* hbox_feeds = gtk_hbox_new(FALSE, 0);
    GtkWidget* vbox_feed_buttons = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_feeds, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_feeds), scroll_feeds, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_feeds), vbox_feed_buttons, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_feed_buttons), prefs.button_feed_remove, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_feed_buttons), prefs.button_feed_add, TRUE, FALSE, 0);

    // dialog buttons
    prefs.button_cancel = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
    g_signal_connect(G_OBJECT(prefs.button_cancel), "clicked", G_CALLBACK(destroy_pref_window), NULL);

    prefs.button_ok = gtk_button_new_from_stock(GTK_STOCK_OK);
    g_signal_connect(G_OBJECT(prefs.button_ok), "clicked", G_CALLBACK(pref_apply), NULL);

    GtkWidget* hbox_buttons = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_end(GTK_BOX(vbox), hbox_buttons, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(hbox_buttons), prefs.button_ok, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(hbox_buttons), prefs.button_cancel, FALSE, FALSE, 0);
}

void destroy_pref_window(GtkWidget *widget, gpointer data) {
    gtk_widget_destroy(prefs.window);
    prefs.window = NULL;
}

void show_pref_window(GtkWidget *widget, gpointer data) {
    if (!prefs.window) {
        create_pref_window();
    }

    gtk_window_set_position(GTK_WINDOW(prefs.window), GTK_WIN_POS_CENTER);
    gtk_widget_show_all(prefs.window);
}

void pref_feed_edited(GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data) {
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(prefs.tree_feeds));
    GtkTreeIter iter;
    GtkTreePath *path = gtk_tree_path_new_from_string(path_string);
    gtk_tree_model_get_iter(model, &iter, path);

    guint column_number = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(cell), "my_column_num"));

    GValue value = { 0, };
    g_value_init(&value, G_TYPE_STRING);
    g_value_set_string(&value, new_text);
    gtk_list_store_set_value(GTK_LIST_STORE(model), &iter, column_number, &value);
}

void pref_apply(GtkWidget* widget, gpointer data) {
    FILE *f;
    char* config_path = g_strconcat(USER_DIR,"/config",NULL);

    if (config_path) {
        f = fopen(config_path, "w+");
        if (f) {
            fprintf(f, "# settings\n");
            fprintf(f, "browser=%s\n", gtk_entry_get_text(GTK_ENTRY(prefs.entry_browser)));
            fprintf(f, "update_interval=%u\n", gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(prefs.spin_update)));
            fprintf(f, "max_articles=%u\n", gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(prefs.spin_articles)));

            fprintf(f, "\n# feeds\n");
            GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(prefs.tree_feeds));
            gtk_tree_model_foreach(GTK_TREE_MODEL(model), pref_feed_foreach, (gpointer)f);

            fclose(f);
        }
    }

    gtk_widget_destroy(prefs.window);
    prefs.window = NULL;

    create_primary_menu();
}

gboolean pref_feed_foreach(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer user_data) {
    gchar *name;
    gchar *uri;

    gtk_tree_model_get(model, iter, 0, &name, 1, &uri, -1);
    FILE *f = (FILE*)user_data;
    fprintf(f, "feed=%s,%s\n", name, uri);

    g_free(name);
    g_free(uri);

    return FALSE;
}

void pref_feed_remove(GtkWidget* widget, gpointer data) {
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(prefs.tree_feeds));
    GtkTreeIter iter;
    gtk_tree_selection_get_selected(gtk_tree_view_get_selection(GTK_TREE_VIEW(prefs.tree_feeds)), &model, &iter);
    gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
}

void pref_feed_add(GtkWidget* widget, gpointer data) {
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(prefs.tree_feeds));
    GtkTreeIter iter;
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter, 0, "<New Feed>", 1, "http://", -1);
}
