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

#include "gui.h"
#include "util.h"

int main( int argc, char* argv[] ) {
    gtk_init( &argc, &argv );
    
    // Get the user's home directory
    homedir = getenv("HOME");
    
    // Set the status icon and make it visible
    GtkIconTheme *icontheme = gtk_icon_theme_get_default();
    status_icon = gtk_status_icon_new_from_pixbuf(gtk_icon_theme_load_icon(icontheme,"liferea",16,0,NULL));
    gtk_status_icon_set_visible(status_icon, TRUE);

    gtk_status_icon_set_tooltip(status_icon, "feedreader");
    
    // Connect tray icon signals
    g_signal_connect (G_OBJECT (status_icon), "activate", G_CALLBACK (primary_menu), NULL);
    g_signal_connect (G_OBJECT (status_icon), "popup-menu", G_CALLBACK (alternate_menu), NULL);
    
    // Add a timer for updating feeds
    g_timeout_add_seconds(UPDATE_INTERVAL, (GSourceFunc) create_primary_menu, NULL);
    
    // Load feeds on startup
    create_primary_menu();
    
    gtk_main();

    // Remove temporary files on exit
    clean_up();
    
    return 0;
}
