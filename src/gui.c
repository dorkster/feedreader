#include <stdlib.h>

#include "download.h"
#include "feedlist.h"
#include "gui.h"
#include "parse.h"

gboolean create_primary_menu() {

    if(main_menu == NULL || !gtk_widget_get_visible(main_menu)) {
        if(main_menu != NULL) {
            gtk_widget_destroy(main_menu);
        }

        main_menu = gtk_menu_new();

        loadconfig();

        GtkWidget *item;
        
        while(feeds != NULL) {
            item = gtk_menu_item_new_with_label(feeds->name);
            gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), NULL);
 
            gtk_widget_set_tooltip_text(item,(const gchar *)feeds->uri);
            gtk_widget_set_has_tooltip(item,FALSE);

            download(feeds->id,feeds->uri);
            parsefeed(feeds);

            gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), feeds->submenu);

            gtk_menu_prepend(main_menu, item);
            feeds = feeds->next;
        }
                
        GtkWidget *sep = gtk_separator_menu_item_new();
        item = gtk_image_menu_item_new_with_label("Reload Feeds");
        gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item),gtk_image_new_from_stock(GTK_STOCK_REFRESH, GTK_ICON_SIZE_MENU));
        g_signal_connect (G_OBJECT(item), "activate", G_CALLBACK(create_primary_menu), NULL);
        gtk_menu_append(main_menu, sep);
        gtk_menu_append(main_menu, item);
        
    }
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

