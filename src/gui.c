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

void create_tray_icon() {
    // Set the status icon and make it visible
    GtkIconTheme *icontheme = gtk_icon_theme_get_default();
    status_icon = gtk_status_icon_new_from_pixbuf(gtk_icon_theme_load_icon(icontheme,"feedreader",16,0,NULL));
    if (!gtk_status_icon_get_pixbuf(status_icon)) status_icon = gtk_status_icon_new_from_file("/usr/share/pixmaps/feedreader.png");
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
        GtkWidget *item;
        alt_menu = gtk_menu_new();

        item = gtk_menu_item_new_with_label("Quit");
        gtk_menu_append(alt_menu, item);
        g_signal_connect (G_OBJECT(item), "activate", G_CALLBACK(destroy), NULL);
    }

    gtk_widget_show_all(alt_menu);
    gtk_menu_popup(GTK_MENU(alt_menu), NULL, NULL, NULL, status_icon, button, activate_time);
}

