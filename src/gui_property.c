/*
 * Copyright (c) 2001-2002, Richard Eckart
 *
 * THIS FILE IS AUTOGENERATED! DO NOT EDIT!
 *
 *----------------------------------------------------------------------
 * This file is part of gtk-gnutella.
 *
 *  gtk-gnutella is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  gtk-gnutella is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with gtk-gnutella; if not, write to the Free Software
 *  Foundation, Inc.:
 *      59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *----------------------------------------------------------------------
 */

#include "prop.h"
#include "gui_property_priv.h"
#include "gui_property.h"


/*
 * Includes specified by "uses"-statement in .ag file
 */



gboolean monitor_enabled     = FALSE;
gboolean monitor_enabled_def = FALSE;
guint32  monitor_max_items     = 25;
guint32  monitor_max_items_def = 25;
gboolean queue_regex_case     = TRUE;
gboolean queue_regex_case_def = TRUE;
gboolean search_pick_all     = TRUE;
gboolean search_pick_all_def = TRUE;
guint32  nodes_col_widths[5]     = { 130, 50, 120, 20, 80 };
guint32  nodes_col_widths_def[5] = { 130, 50, 120, 20, 80 };
guint32  dl_active_col_widths[5]     = { 240, 80, 80, 80, 80 };
guint32  dl_active_col_widths_def[5] = { 240, 80, 80, 80, 80 };
guint32  dl_queued_col_widths[5]     = { 240, 80, 80, 80, 80 };
guint32  dl_queued_col_widths_def[5] = { 240, 80, 80, 80, 80 };
guint32  search_results_col_visible[6]     = { 1,1,1,1,0,1};
guint32  search_results_col_visible_def[6] = { 1,1,1,1,0,1};
guint32  search_list_col_widths[3]     = { 80, 20, 20 };
guint32  search_list_col_widths_def[3] = { 80, 20, 20 };
guint32  search_results_col_widths[6]     = { 210, 80, 50, 70, 70, 140 };
guint32  search_results_col_widths_def[6] = { 210, 80, 50, 70, 70, 140 };
guint32  search_stats_col_widths[3]     = { 200, 80, 80 };
guint32  search_stats_col_widths_def[3] = { 200, 80, 80 };
guint32  ul_stats_col_widths[5]     = { 200, 80, 80, 80, 80 };
guint32  ul_stats_col_widths_def[5] = { 200, 80, 80, 80, 80 };
guint32  uploads_col_widths[6]     = { 200, 120, 36, 80, 80, 80 };
guint32  uploads_col_widths_def[6] = { 200, 120, 36, 80, 80, 80 };
guint32  filter_rules_col_widths[4]     = { 10, 240, 80, 40 };
guint32  filter_rules_col_widths_def[4] = { 10, 240, 80, 40 };
guint32  filter_filters_col_widths[3]     = { 80, 20, 20 };
guint32  filter_filters_col_widths_def[3] = { 80, 20, 20 };
guint32  window_coords[4]     = { 0, 0, 0, 0 };
guint32  window_coords_def[4] = { 0, 0, 0, 0 };
guint32  filter_dlg_coords[4]     = { 0, 0, 0, 0 };
guint32  filter_dlg_coords_def[4] = { 0, 0, 0, 0 };
guint32  downloads_divider_pos     = 0x0000;
guint32  downloads_divider_pos_def = 0x0000;
guint32  main_divider_pos     = 0x0000;
guint32  main_divider_pos_def = 0x0000;
guint32  side_divider_pos     = 0x0000;
guint32  side_divider_pos_def = 0x0000;
guint32  search_max_results     = 5000;
guint32  search_max_results_def = 5000;
guint32  gui_debug     = 0;
guint32  gui_debug_def = 0;
guint32  filter_main_divider_pos     = 0x0000;
guint32  filter_main_divider_pos_def = 0x0000;
gboolean search_results_show_tabs     = FALSE;
gboolean search_results_show_tabs_def = FALSE;
gboolean toolbar_visible     = FALSE;
gboolean toolbar_visible_def = FALSE;
gboolean statusbar_visible     = TRUE;
gboolean statusbar_visible_def = TRUE;
gboolean progressbar_uploads_visible     = TRUE;
gboolean progressbar_uploads_visible_def = TRUE;
gboolean progressbar_downloads_visible     = TRUE;
gboolean progressbar_downloads_visible_def = TRUE;
gboolean progressbar_connections_visible     = TRUE;
gboolean progressbar_connections_visible_def = TRUE;
gboolean progressbar_bws_in_visible     = TRUE;
gboolean progressbar_bws_in_visible_def = TRUE;
gboolean progressbar_bws_out_visible     = TRUE;
gboolean progressbar_bws_out_visible_def = TRUE;
gboolean progressbar_bws_gin_visible     = TRUE;
gboolean progressbar_bws_gin_visible_def = TRUE;
gboolean progressbar_bws_gout_visible     = TRUE;
gboolean progressbar_bws_gout_visible_def = TRUE;
gboolean progressbar_bws_in_avg     = TRUE;
gboolean progressbar_bws_in_avg_def = TRUE;
gboolean progressbar_bws_out_avg     = TRUE;
gboolean progressbar_bws_out_avg_def = TRUE;
gboolean progressbar_bws_gin_avg     = TRUE;
gboolean progressbar_bws_gin_avg_def = TRUE;
gboolean progressbar_bws_gout_avg     = TRUE;
gboolean progressbar_bws_gout_avg_def = TRUE;
gboolean search_autoselect_ident     = FALSE;
gboolean search_autoselect_ident_def = FALSE;
gboolean jump_to_downloads     = TRUE;
gboolean jump_to_downloads_def = TRUE;

static prop_set_t *gui_property = NULL;

prop_set_t *gui_prop_init(void) {
    gui_property = g_new(prop_set_t, 1);
    gui_property->name   = "gui_property";
    gui_property->desc   = "";
    gui_property->size   = GUI_PROPERTY_NUM;
    gui_property->offset = 1000;
    gui_property->mtime  = 0;
    gui_property->props  = g_new(prop_def_t, GUI_PROPERTY_NUM);
    gui_property->get_stub = gui_prop_get_stub;


    /*
     * PROP_MONITOR_ENABLED:
     *
     * General data:
     */
    gui_property->props[0].name = "monitor_enabled";
    gui_property->props[0].desc = "Search monitor enabled";
    gui_property->props[0].prop_changed_listeners = NULL;
    gui_property->props[0].save = TRUE;
    gui_property->props[0].vector_size = 1;

    /* Type specific data: */
    gui_property->props[0].type               = PROP_TYPE_BOOLEAN;
    gui_property->props[0].data.boolean.def   = &monitor_enabled_def;
    gui_property->props[0].data.boolean.value = &monitor_enabled;


    /*
     * PROP_MONITOR_MAX_ITEMS:
     *
     * General data:
     */
    gui_property->props[1].name = "monitor_max_items";
    gui_property->props[1].desc = "Maximum number of queries visible in search monitor";
    gui_property->props[1].prop_changed_listeners = NULL;
    gui_property->props[1].save = TRUE;
    gui_property->props[1].vector_size = 1;

    /* Type specific data: */
    gui_property->props[1].type               = PROP_TYPE_GUINT32;
    gui_property->props[1].data.guint32.def   = &monitor_max_items_def;
    gui_property->props[1].data.guint32.value = &monitor_max_items;
    gui_property->props[1].data.guint32.max   = 100;
    gui_property->props[1].data.guint32.min   = 0;


    /*
     * PROP_QUEUE_REGEX_CASE:
     *
     * General data:
     */
    gui_property->props[2].name = "queue_regex_case";
    gui_property->props[2].desc = "Match queue selection by regexp case sensitive";
    gui_property->props[2].prop_changed_listeners = NULL;
    gui_property->props[2].save = TRUE;
    gui_property->props[2].vector_size = 1;

    /* Type specific data: */
    gui_property->props[2].type               = PROP_TYPE_BOOLEAN;
    gui_property->props[2].data.boolean.def   = &queue_regex_case_def;
    gui_property->props[2].data.boolean.value = &queue_regex_case;


    /*
     * PROP_SEARCH_PICK_ALL:
     *
     * General data:
     */
    gui_property->props[3].name = "search_pick_all";
    gui_property->props[3].desc = "Autoselect similar files in searches";
    gui_property->props[3].prop_changed_listeners = NULL;
    gui_property->props[3].save = TRUE;
    gui_property->props[3].vector_size = 1;

    /* Type specific data: */
    gui_property->props[3].type               = PROP_TYPE_BOOLEAN;
    gui_property->props[3].data.boolean.def   = &search_pick_all_def;
    gui_property->props[3].data.boolean.value = &search_pick_all;


    /*
     * PROP_NODES_COL_WIDTHS:
     *
     * General data:
     */
    gui_property->props[4].name = "widths_nodes";
    gui_property->props[4].desc = "Widths of the columns in the nodes table";
    gui_property->props[4].prop_changed_listeners = NULL;
    gui_property->props[4].save = TRUE;
    gui_property->props[4].vector_size = 5;

    /* Type specific data: */
    gui_property->props[4].type               = PROP_TYPE_GUINT32;
    gui_property->props[4].data.guint32.def   = nodes_col_widths_def;
    gui_property->props[4].data.guint32.value = nodes_col_widths;


    /*
     * PROP_DL_ACTIVE_COL_WIDTHS:
     *
     * General data:
     */
    gui_property->props[5].name = "widths_dl_active";
    gui_property->props[5].desc = "Widths of the columns in the active downloads table";
    gui_property->props[5].prop_changed_listeners = NULL;
    gui_property->props[5].save = TRUE;
    gui_property->props[5].vector_size = 5;

    /* Type specific data: */
    gui_property->props[5].type               = PROP_TYPE_GUINT32;
    gui_property->props[5].data.guint32.def   = dl_active_col_widths_def;
    gui_property->props[5].data.guint32.value = dl_active_col_widths;


    /*
     * PROP_DL_QUEUED_COL_WIDTHS:
     *
     * General data:
     */
    gui_property->props[6].name = "widths_dl_queued";
    gui_property->props[6].desc = "Widths of the columns in the queued downloads table";
    gui_property->props[6].prop_changed_listeners = NULL;
    gui_property->props[6].save = TRUE;
    gui_property->props[6].vector_size = 5;

    /* Type specific data: */
    gui_property->props[6].type               = PROP_TYPE_GUINT32;
    gui_property->props[6].data.guint32.def   = dl_queued_col_widths_def;
    gui_property->props[6].data.guint32.value = dl_queued_col_widths;


    /*
     * PROP_SEARCH_RESULTS_COL_VISIBLE:
     *
     * General data:
     */
    gui_property->props[7].name = "search_results_col_visible";
    gui_property->props[7].desc = "Which columns are visible in the search results tables";
    gui_property->props[7].prop_changed_listeners = NULL;
    gui_property->props[7].save = TRUE;
    gui_property->props[7].vector_size = 6;

    /* Type specific data: */
    gui_property->props[7].type               = PROP_TYPE_GUINT32;
    gui_property->props[7].data.guint32.def   = search_results_col_visible_def;
    gui_property->props[7].data.guint32.value = search_results_col_visible;


    /*
     * PROP_SEARCH_LIST_COL_WIDTHS:
     *
     * General data:
     */
    gui_property->props[8].name = "widths_search_list";
    gui_property->props[8].desc = "Widths of the columns in the search list on the sidebar";
    gui_property->props[8].prop_changed_listeners = NULL;
    gui_property->props[8].save = TRUE;
    gui_property->props[8].vector_size = 3;

    /* Type specific data: */
    gui_property->props[8].type               = PROP_TYPE_GUINT32;
    gui_property->props[8].data.guint32.def   = search_list_col_widths_def;
    gui_property->props[8].data.guint32.value = search_list_col_widths;


    /*
     * PROP_SEARCH_RESULTS_COL_WIDTHS:
     *
     * General data:
     */
    gui_property->props[9].name = "widths_search_results";
    gui_property->props[9].desc = "Widths of the columns in the search results tables";
    gui_property->props[9].prop_changed_listeners = NULL;
    gui_property->props[9].save = TRUE;
    gui_property->props[9].vector_size = 6;

    /* Type specific data: */
    gui_property->props[9].type               = PROP_TYPE_GUINT32;
    gui_property->props[9].data.guint32.def   = search_results_col_widths_def;
    gui_property->props[9].data.guint32.value = search_results_col_widths;


    /*
     * PROP_SEARCH_STATS_COL_WIDTHS:
     *
     * General data:
     */
    gui_property->props[10].name = "widths_search_stats";
    gui_property->props[10].desc = "Widths of the columns in the search stats table";
    gui_property->props[10].prop_changed_listeners = NULL;
    gui_property->props[10].save = TRUE;
    gui_property->props[10].vector_size = 3;

    /* Type specific data: */
    gui_property->props[10].type               = PROP_TYPE_GUINT32;
    gui_property->props[10].data.guint32.def   = search_stats_col_widths_def;
    gui_property->props[10].data.guint32.value = search_stats_col_widths;


    /*
     * PROP_UL_STATS_COL_WIDTHS:
     *
     * General data:
     */
    gui_property->props[11].name = "widths_ul_stats";
    gui_property->props[11].desc = "Widths of the columns in the upload stats table";
    gui_property->props[11].prop_changed_listeners = NULL;
    gui_property->props[11].save = TRUE;
    gui_property->props[11].vector_size = 5;

    /* Type specific data: */
    gui_property->props[11].type               = PROP_TYPE_GUINT32;
    gui_property->props[11].data.guint32.def   = ul_stats_col_widths_def;
    gui_property->props[11].data.guint32.value = ul_stats_col_widths;


    /*
     * PROP_UPLOADS_COL_WIDTHS:
     *
     * General data:
     */
    gui_property->props[12].name = "widths_uploads";
    gui_property->props[12].desc = "Widths of the columns in the uploads table";
    gui_property->props[12].prop_changed_listeners = NULL;
    gui_property->props[12].save = TRUE;
    gui_property->props[12].vector_size = 6;

    /* Type specific data: */
    gui_property->props[12].type               = PROP_TYPE_GUINT32;
    gui_property->props[12].data.guint32.def   = uploads_col_widths_def;
    gui_property->props[12].data.guint32.value = uploads_col_widths;


    /*
     * PROP_FILTER_RULES_COL_WIDTHS:
     *
     * General data:
     */
    gui_property->props[13].name = "widths_filter_table";
    gui_property->props[13].desc = "Widths of the columns in the rules table in the filter dialog";
    gui_property->props[13].prop_changed_listeners = NULL;
    gui_property->props[13].save = TRUE;
    gui_property->props[13].vector_size = 4;

    /* Type specific data: */
    gui_property->props[13].type               = PROP_TYPE_GUINT32;
    gui_property->props[13].data.guint32.def   = filter_rules_col_widths_def;
    gui_property->props[13].data.guint32.value = filter_rules_col_widths;


    /*
     * PROP_FILTER_FILTERS_COL_WIDTHS:
     *
     * General data:
     */
    gui_property->props[14].name = "widths_filter_filters";
    gui_property->props[14].desc = "Widths of the columns in the filter table in the filter dialog";
    gui_property->props[14].prop_changed_listeners = NULL;
    gui_property->props[14].save = TRUE;
    gui_property->props[14].vector_size = 3;

    /* Type specific data: */
    gui_property->props[14].type               = PROP_TYPE_GUINT32;
    gui_property->props[14].data.guint32.def   = filter_filters_col_widths_def;
    gui_property->props[14].data.guint32.value = filter_filters_col_widths;


    /*
     * PROP_WINDOW_COORDS:
     *
     * General data:
     */
    gui_property->props[15].name = "window_coords";
    gui_property->props[15].desc = "Position and size of the main window";
    gui_property->props[15].prop_changed_listeners = NULL;
    gui_property->props[15].save = TRUE;
    gui_property->props[15].vector_size = 4;

    /* Type specific data: */
    gui_property->props[15].type               = PROP_TYPE_GUINT32;
    gui_property->props[15].data.guint32.def   = window_coords_def;
    gui_property->props[15].data.guint32.value = window_coords;


    /*
     * PROP_FILTER_DLG_COORDS:
     *
     * General data:
     */
    gui_property->props[16].name = "filter_dlg_coords";
    gui_property->props[16].desc = "Position and size of the filter dialog";
    gui_property->props[16].prop_changed_listeners = NULL;
    gui_property->props[16].save = TRUE;
    gui_property->props[16].vector_size = 4;

    /* Type specific data: */
    gui_property->props[16].type               = PROP_TYPE_GUINT32;
    gui_property->props[16].data.guint32.def   = filter_dlg_coords_def;
    gui_property->props[16].data.guint32.value = filter_dlg_coords;


    /*
     * PROP_DOWNLOADS_DIVIDER_POS:
     *
     * General data:
     */
    gui_property->props[17].name = "downloads_divider_pos";
    gui_property->props[17].desc = "Position of the divider in the downloads panel";
    gui_property->props[17].prop_changed_listeners = NULL;
    gui_property->props[17].save = TRUE;
    gui_property->props[17].vector_size = 1;

    /* Type specific data: */
    gui_property->props[17].type               = PROP_TYPE_GUINT32;
    gui_property->props[17].data.guint32.def   = &downloads_divider_pos_def;
    gui_property->props[17].data.guint32.value = &downloads_divider_pos;
    gui_property->props[17].data.guint32.max   = 0xFFFF;
    gui_property->props[17].data.guint32.min   = 0x0000;


    /*
     * PROP_MAIN_DIVIDER_POS:
     *
     * General data:
     */
    gui_property->props[18].name = "main_divider_pos";
    gui_property->props[18].desc = "Size of the sidebar";
    gui_property->props[18].prop_changed_listeners = NULL;
    gui_property->props[18].save = TRUE;
    gui_property->props[18].vector_size = 1;

    /* Type specific data: */
    gui_property->props[18].type               = PROP_TYPE_GUINT32;
    gui_property->props[18].data.guint32.def   = &main_divider_pos_def;
    gui_property->props[18].data.guint32.value = &main_divider_pos;
    gui_property->props[18].data.guint32.max   = 0xFFFF;
    gui_property->props[18].data.guint32.min   = 0x0000;


    /*
     * PROP_SIDE_DIVIDER_POS:
     *
     * General data:
     */
    gui_property->props[19].name = "side_divider_pos";
    gui_property->props[19].desc = "Size of the menu in the sidebar";
    gui_property->props[19].prop_changed_listeners = NULL;
    gui_property->props[19].save = TRUE;
    gui_property->props[19].vector_size = 1;

    /* Type specific data: */
    gui_property->props[19].type               = PROP_TYPE_GUINT32;
    gui_property->props[19].data.guint32.def   = &side_divider_pos_def;
    gui_property->props[19].data.guint32.value = &side_divider_pos;
    gui_property->props[19].data.guint32.max   = 0xFFFF;
    gui_property->props[19].data.guint32.min   = 0x0000;


    /*
     * PROP_SEARCH_MAX_RESULTS:
     *
     * General data:
     */
    gui_property->props[20].name = "search_max_results";
    gui_property->props[20].desc = "Maximum number of results to show in any search";
    gui_property->props[20].prop_changed_listeners = NULL;
    gui_property->props[20].save = TRUE;
    gui_property->props[20].vector_size = 1;

    /* Type specific data: */
    gui_property->props[20].type               = PROP_TYPE_GUINT32;
    gui_property->props[20].data.guint32.def   = &search_max_results_def;
    gui_property->props[20].data.guint32.value = &search_max_results;
    gui_property->props[20].data.guint32.max   = 100000;
    gui_property->props[20].data.guint32.min   = 10;


    /*
     * PROP_GUI_DEBUG:
     *
     * General data:
     */
    gui_property->props[21].name = "gui_debug";
    gui_property->props[21].desc = "Debug level for the gui";
    gui_property->props[21].prop_changed_listeners = NULL;
    gui_property->props[21].save = TRUE;
    gui_property->props[21].vector_size = 1;

    /* Type specific data: */
    gui_property->props[21].type               = PROP_TYPE_GUINT32;
    gui_property->props[21].data.guint32.def   = &gui_debug_def;
    gui_property->props[21].data.guint32.value = &gui_debug;
    gui_property->props[21].data.guint32.max   = 20;
    gui_property->props[21].data.guint32.min   = 0;


    /*
     * PROP_FILTER_MAIN_DIVIDER_POS:
     *
     * General data:
     */
    gui_property->props[22].name = "filter_main_divider_pos";
    gui_property->props[22].desc = "Size of the filter tree in the filter dialog";
    gui_property->props[22].prop_changed_listeners = NULL;
    gui_property->props[22].save = TRUE;
    gui_property->props[22].vector_size = 1;

    /* Type specific data: */
    gui_property->props[22].type               = PROP_TYPE_GUINT32;
    gui_property->props[22].data.guint32.def   = &filter_main_divider_pos_def;
    gui_property->props[22].data.guint32.value = &filter_main_divider_pos;
    gui_property->props[22].data.guint32.max   = 0xFFFF;
    gui_property->props[22].data.guint32.min   = 0x0000;


    /*
     * PROP_SEARCH_RESULTS_SHOW_TABS:
     *
     * General data:
     */
    gui_property->props[23].name = "search_results_show_tabs";
    gui_property->props[23].desc = "Show tabs or search list";
    gui_property->props[23].prop_changed_listeners = NULL;
    gui_property->props[23].save = TRUE;
    gui_property->props[23].vector_size = 1;

    /* Type specific data: */
    gui_property->props[23].type               = PROP_TYPE_BOOLEAN;
    gui_property->props[23].data.boolean.def   = &search_results_show_tabs_def;
    gui_property->props[23].data.boolean.value = &search_results_show_tabs;


    /*
     * PROP_TOOLBAR_VISIBLE:
     *
     * General data:
     */
    gui_property->props[24].name = "toolbar_visible";
    gui_property->props[24].desc = "Display toolbar";
    gui_property->props[24].prop_changed_listeners = NULL;
    gui_property->props[24].save = TRUE;
    gui_property->props[24].vector_size = 1;

    /* Type specific data: */
    gui_property->props[24].type               = PROP_TYPE_BOOLEAN;
    gui_property->props[24].data.boolean.def   = &toolbar_visible_def;
    gui_property->props[24].data.boolean.value = &toolbar_visible;


    /*
     * PROP_STATUSBAR_VISIBLE:
     *
     * General data:
     */
    gui_property->props[25].name = "statusbar_visible";
    gui_property->props[25].desc = "Display statusbar";
    gui_property->props[25].prop_changed_listeners = NULL;
    gui_property->props[25].save = TRUE;
    gui_property->props[25].vector_size = 1;

    /* Type specific data: */
    gui_property->props[25].type               = PROP_TYPE_BOOLEAN;
    gui_property->props[25].data.boolean.def   = &statusbar_visible_def;
    gui_property->props[25].data.boolean.value = &statusbar_visible;


    /*
     * PROP_PROGRESSBAR_UPLOADS_VISIBLE:
     *
     * General data:
     */
    gui_property->props[26].name = "progressbar_uploads_visible";
    gui_property->props[26].desc = "Display upload statistics in sidebar";
    gui_property->props[26].prop_changed_listeners = NULL;
    gui_property->props[26].save = TRUE;
    gui_property->props[26].vector_size = 1;

    /* Type specific data: */
    gui_property->props[26].type               = PROP_TYPE_BOOLEAN;
    gui_property->props[26].data.boolean.def   = &progressbar_uploads_visible_def;
    gui_property->props[26].data.boolean.value = &progressbar_uploads_visible;


    /*
     * PROP_PROGRESSBAR_DOWNLOADS_VISIBLE:
     *
     * General data:
     */
    gui_property->props[27].name = "progressbar_downloads_visible";
    gui_property->props[27].desc = "Display download statistics in sidebar";
    gui_property->props[27].prop_changed_listeners = NULL;
    gui_property->props[27].save = TRUE;
    gui_property->props[27].vector_size = 1;

    /* Type specific data: */
    gui_property->props[27].type               = PROP_TYPE_BOOLEAN;
    gui_property->props[27].data.boolean.def   = &progressbar_downloads_visible_def;
    gui_property->props[27].data.boolean.value = &progressbar_downloads_visible;


    /*
     * PROP_PROGRESSBAR_CONNECTIONS_VISIBLE:
     *
     * General data:
     */
    gui_property->props[28].name = "progressbar_connections_visible";
    gui_property->props[28].desc = "Display connection statistics in sidebar";
    gui_property->props[28].prop_changed_listeners = NULL;
    gui_property->props[28].save = TRUE;
    gui_property->props[28].vector_size = 1;

    /* Type specific data: */
    gui_property->props[28].type               = PROP_TYPE_BOOLEAN;
    gui_property->props[28].data.boolean.def   = &progressbar_connections_visible_def;
    gui_property->props[28].data.boolean.value = &progressbar_connections_visible;


    /*
     * PROP_PROGRESSBAR_BWS_IN_VISIBLE:
     *
     * General data:
     */
    gui_property->props[29].name = "progressbar_bws_in_visible";
    gui_property->props[29].desc = "Display incoming HTTP traffic bandwidth usage";
    gui_property->props[29].prop_changed_listeners = NULL;
    gui_property->props[29].save = TRUE;
    gui_property->props[29].vector_size = 1;

    /* Type specific data: */
    gui_property->props[29].type               = PROP_TYPE_BOOLEAN;
    gui_property->props[29].data.boolean.def   = &progressbar_bws_in_visible_def;
    gui_property->props[29].data.boolean.value = &progressbar_bws_in_visible;


    /*
     * PROP_PROGRESSBAR_BWS_OUT_VISIBLE:
     *
     * General data:
     */
    gui_property->props[30].name = "progressbar_bws_out_visible";
    gui_property->props[30].desc = "Display outgoing HTTP traffic bandwidth usage";
    gui_property->props[30].prop_changed_listeners = NULL;
    gui_property->props[30].save = TRUE;
    gui_property->props[30].vector_size = 1;

    /* Type specific data: */
    gui_property->props[30].type               = PROP_TYPE_BOOLEAN;
    gui_property->props[30].data.boolean.def   = &progressbar_bws_out_visible_def;
    gui_property->props[30].data.boolean.value = &progressbar_bws_out_visible;


    /*
     * PROP_PROGRESSBAR_BWS_GIN_VISIBLE:
     *
     * General data:
     */
    gui_property->props[31].name = "progressbar_bws_gin_visible";
    gui_property->props[31].desc = "Display incoming gNet traffic bandwidth usage";
    gui_property->props[31].prop_changed_listeners = NULL;
    gui_property->props[31].save = TRUE;
    gui_property->props[31].vector_size = 1;

    /* Type specific data: */
    gui_property->props[31].type               = PROP_TYPE_BOOLEAN;
    gui_property->props[31].data.boolean.def   = &progressbar_bws_gin_visible_def;
    gui_property->props[31].data.boolean.value = &progressbar_bws_gin_visible;


    /*
     * PROP_PROGRESSBAR_BWS_GOUT_VISIBLE:
     *
     * General data:
     */
    gui_property->props[32].name = "progressbar_bws_gout_visible";
    gui_property->props[32].desc = "Display outgoing gNet traffic bandwidth usage";
    gui_property->props[32].prop_changed_listeners = NULL;
    gui_property->props[32].save = TRUE;
    gui_property->props[32].vector_size = 1;

    /* Type specific data: */
    gui_property->props[32].type               = PROP_TYPE_BOOLEAN;
    gui_property->props[32].data.boolean.def   = &progressbar_bws_gout_visible_def;
    gui_property->props[32].data.boolean.value = &progressbar_bws_gout_visible;


    /*
     * PROP_PROGRESSBAR_BWS_IN_AVG:
     *
     * General data:
     */
    gui_property->props[33].name = "progressbar_bws_in_avg";
    gui_property->props[33].desc = "Display incoming HTTP traffic bandwidth average";
    gui_property->props[33].prop_changed_listeners = NULL;
    gui_property->props[33].save = TRUE;
    gui_property->props[33].vector_size = 1;

    /* Type specific data: */
    gui_property->props[33].type               = PROP_TYPE_BOOLEAN;
    gui_property->props[33].data.boolean.def   = &progressbar_bws_in_avg_def;
    gui_property->props[33].data.boolean.value = &progressbar_bws_in_avg;


    /*
     * PROP_PROGRESSBAR_BWS_OUT_AVG:
     *
     * General data:
     */
    gui_property->props[34].name = "progressbar_bws_out_avg";
    gui_property->props[34].desc = "Display outgoing HTTP traffic bandwidth average";
    gui_property->props[34].prop_changed_listeners = NULL;
    gui_property->props[34].save = TRUE;
    gui_property->props[34].vector_size = 1;

    /* Type specific data: */
    gui_property->props[34].type               = PROP_TYPE_BOOLEAN;
    gui_property->props[34].data.boolean.def   = &progressbar_bws_out_avg_def;
    gui_property->props[34].data.boolean.value = &progressbar_bws_out_avg;


    /*
     * PROP_PROGRESSBAR_BWS_GIN_AVG:
     *
     * General data:
     */
    gui_property->props[35].name = "progressbar_bws_gin_avg";
    gui_property->props[35].desc = "Display incoming gNet traffic bandwidth average";
    gui_property->props[35].prop_changed_listeners = NULL;
    gui_property->props[35].save = TRUE;
    gui_property->props[35].vector_size = 1;

    /* Type specific data: */
    gui_property->props[35].type               = PROP_TYPE_BOOLEAN;
    gui_property->props[35].data.boolean.def   = &progressbar_bws_gin_avg_def;
    gui_property->props[35].data.boolean.value = &progressbar_bws_gin_avg;


    /*
     * PROP_PROGRESSBAR_BWS_GOUT_AVG:
     *
     * General data:
     */
    gui_property->props[36].name = "progressbar_bws_gout_avg";
    gui_property->props[36].desc = "Display outgoing gNet traffic bandwidth average";
    gui_property->props[36].prop_changed_listeners = NULL;
    gui_property->props[36].save = TRUE;
    gui_property->props[36].vector_size = 1;

    /* Type specific data: */
    gui_property->props[36].type               = PROP_TYPE_BOOLEAN;
    gui_property->props[36].data.boolean.def   = &progressbar_bws_gout_avg_def;
    gui_property->props[36].data.boolean.value = &progressbar_bws_gout_avg;


    /*
     * PROP_SEARCH_AUTOSELECT_IDENT:
     *
     * General data:
     */
    gui_property->props[37].name = "search_autoselect_ident";
    gui_property->props[37].desc = "When enabled autoselection only takes place if filesize matches exactly, otherwise it must be equal or greater";
    gui_property->props[37].prop_changed_listeners = NULL;
    gui_property->props[37].save = TRUE;
    gui_property->props[37].vector_size = 1;

    /* Type specific data: */
    gui_property->props[37].type               = PROP_TYPE_BOOLEAN;
    gui_property->props[37].data.boolean.def   = &search_autoselect_ident_def;
    gui_property->props[37].data.boolean.value = &search_autoselect_ident;


    /*
     * PROP_JUMP_TO_DOWNLOADS:
     *
     * General data:
     */
    gui_property->props[38].name = "jump_to_downloads";
    gui_property->props[38].desc = "Jump to downloads screen when downloading a file";
    gui_property->props[38].prop_changed_listeners = NULL;
    gui_property->props[38].save = TRUE;
    gui_property->props[38].vector_size = 1;

    /* Type specific data: */
    gui_property->props[38].type               = PROP_TYPE_BOOLEAN;
    gui_property->props[38].data.boolean.def   = &jump_to_downloads_def;
    gui_property->props[38].data.boolean.value = &jump_to_downloads;
    return gui_property;
}

/*
 * gui_prop_shutdown:
 *
 * Free memory allocated by the property set.
 */
void gui_prop_shutdown(void) {
    gint n;

    for (n = 0; n < GUI_PROPERTY_NUM; n ++) {
        if (gui_property->props[n].type == PROP_TYPE_STRING) {
            g_free(*gui_property->props[n].data.string.value);
            *gui_property->props[n].data.string.value = NULL;
        }
    }

    g_free(gui_property->props);
    g_free(gui_property);
}

prop_def_t *gui_prop_get_def(property_t p)
{
    return prop_get_def(gui_property, p);
}

/*
 * gui_prop_add_prop_changed_listener:
 *
 * Add a change listener to a given property. If init is TRUE then
 * the listener is immediately called.
 */
void gui_prop_add_prop_changed_listener
    (property_t prop, prop_changed_listener_t l, gboolean init)
{
    prop_add_prop_changed_listener(gui_property, prop, l, init);
}

void gui_prop_remove_prop_changed_listener
    (property_t prop, prop_changed_listener_t l)
{
    prop_remove_prop_changed_listener(gui_property, prop, l);
}

void gui_prop_set_boolean
    (property_t prop, const gboolean *src, gsize offset, gsize length)
{
    prop_set_boolean(gui_property, prop, src, offset, length);
}

gboolean *gui_prop_get_boolean
    (property_t prop, gboolean *t, gsize offset, gsize length)
{
    return prop_get_boolean(gui_property, prop, t, offset, length);
}

void gui_prop_set_guint32
    (property_t prop, const guint32 *src, gsize offset, gsize length)
{
    prop_set_guint32(gui_property, prop, src, offset, length);
}

guint32 *gui_prop_get_guint32
    (property_t prop, guint32 *t, gsize offset, gsize length)
{
    return prop_get_guint32(gui_property, prop, t, offset, length);
}

void gui_prop_set_string(property_t prop, const gchar *val)
{
    prop_set_string(gui_property, prop, val);
}

gchar *gui_prop_get_string(property_t prop, gchar *t, gsize size)
{
    return prop_get_string(gui_property, prop, t, size);
}

void gui_prop_set_storage(property_t p, const guint8 *v, gsize l)
{
    prop_set_storage(gui_property, p, v, l);
}

guint8 *gui_prop_get_storage(property_t p, guint8 *t, gsize l)
{
    return prop_get_storage(gui_property, p, t, l);
}


/*
 * gui_prop_get_stub:
 *
 * Returns a new stub struct for this property set. Just g_free it
 * when it is no longer needed. All fields are read only!
 */
prop_set_stub_t *gui_prop_get_stub(void) 
{
    prop_set_stub_t *stub;

    stub          = g_new0(prop_set_stub_t, 1);
    stub->size    = GUI_PROPERTY_NUM;
    stub->offset  = GUI_PROPERTY_MIN;
    stub->get_def = gui_prop_get_def;

    stub->prop_changed_listener.add = 
        gui_prop_add_prop_changed_listener;
    stub->prop_changed_listener.remove = 
        gui_prop_remove_prop_changed_listener;

    stub->boolean.get = gui_prop_get_boolean;
    stub->boolean.set = gui_prop_set_boolean;

    stub->guint32.get = gui_prop_get_guint32;
    stub->guint32.set = gui_prop_set_guint32;

    stub->string.get = gui_prop_get_string;
    stub->string.set = gui_prop_set_string;

    stub->storage.get = gui_prop_get_storage;
    stub->storage.set = gui_prop_set_storage;

    return stub;
}
