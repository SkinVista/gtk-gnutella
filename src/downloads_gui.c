/*
 * $Id$
 *
 * Copyright (c) 2001-2002, Raphael Manfredi, Richard Eckart
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

#include "downloads_gui.h"

#include "downloads.h" // FIXME: remove this dependency
#include "statusbar_gui.h"
#include "downloads_cb.h"

#define IO_STALLED		60		/* If nothing exchanged after that many secs */

static gchar tmpstr[4096];

void gui_update_download_clear(void)
{
	GSList *l;
	gboolean clear = FALSE;

	for (l = sl_unqueued; !clear && l; l = l->next) {
		switch (((struct download *) l->data)->status) {
		case GTA_DL_COMPLETED:
		case GTA_DL_ERROR:
		case GTA_DL_ABORTED:
		case GTA_DL_VERIFIED:
			clear = TRUE;
			break;
		default:
			break;
		}
	}

	gtk_widget_set_sensitive(
        lookup_widget(main_window, "button_downloads_clear_completed"), 
        clear);
}

void gui_update_download(struct download *d, gboolean force)
{
	gchar *a = NULL;
	gint row;
	time_t now = time((time_t *) NULL);
    GdkColor *color;
    GtkCList *clist_downloads;
	struct dl_file_info *fi;
	gint rw;
	extern gint sha1_eq(gconstpointer a, gconstpointer b);

    if (d->last_gui_update == now && !force)
		return;

    clist_downloads = GTK_CLIST
        (lookup_widget(main_window, "clist_downloads"));

    color = &(gtk_widget_get_style(GTK_WIDGET(clist_downloads))
        ->fg[GTK_STATE_INSENSITIVE]);

	d->last_gui_update = now;
	fi = d->file_info;

	switch (d->status) {
	case GTA_DL_QUEUED:
		a = (gchar *) ((d->remove_msg) ? d->remove_msg : "");
		break;

	case GTA_DL_CONNECTING:
		a = "Connecting...";
		break;

	case GTA_DL_PUSH_SENT:
		a = "Push sent";
		break;

	case GTA_DL_REQ_SENT:
		a = "Request sent";
		break;

	case GTA_DL_HEADERS:
		a = "Receiving headers";
		break;

	case GTA_DL_ABORTED:
		a = "Aborted";
		break;

	case GTA_DL_FALLBACK:
		a = "Falling back to push";
		break;

	case GTA_DL_COMPLETED:
		if (d->last_update != d->start_date) {
			guint32 spent = d->last_update - d->start_date;

			gfloat rate = ((d->range_end - d->skip + d->overlap_size) /
				1024.0) / spent;
			g_snprintf(tmpstr, sizeof(tmpstr), "%s (%.1f k/s) %s",
				FILE_INFO_COMPLETE(fi) ? "Completed" : "Chunk done",
				rate, short_time(spent));
		} else {
			g_snprintf(tmpstr, sizeof(tmpstr), "%s (< 1s)",
				FILE_INFO_COMPLETE(fi) ? "Completed" : "Chunk done");
		}
		a = tmpstr;
		break;

	case GTA_DL_VERIFY_WAIT:
		g_assert(FILE_INFO_COMPLETE(fi));
		g_snprintf(tmpstr, sizeof(tmpstr), "Waiting for SHA1 checking...");
		a = tmpstr;
		break;

	case GTA_DL_VERIFYING:
		g_assert(FILE_INFO_COMPLETE(fi));
		g_snprintf(tmpstr, sizeof(tmpstr),
			"Computing SHA1 (%.02f%%)", fi->cha1_hashed * 100.0 / fi->size);
		a = tmpstr;
		break;

	case GTA_DL_VERIFIED:
		g_assert(FILE_INFO_COMPLETE(fi));
		g_assert(fi->cha1_hashed <= fi->size);
		{
			gboolean sha1_ok = fi->cha1 &&
				(fi->sha1 == NULL || sha1_eq(fi->sha1, fi->cha1));

			rw = g_snprintf(tmpstr, sizeof(tmpstr), "SHA1 check %s",
				fi->cha1 == NULL ?	"ERROR" :
				sha1_ok ?			"OK" :
									"FAILED");
			if (fi->cha1)
				rw += g_snprintf(&tmpstr[rw], sizeof(tmpstr)-rw,
					" (%.1f k/s) %s",
					(gfloat) (fi->cha1_hashed >> 10) / fi->cha1_elapsed,
					short_time(fi->cha1_elapsed));
		}
		a = tmpstr;
		break;

	case GTA_DL_RECEIVING:
		if (d->pos - d->skip > 0) {
			gfloat p = 0, pt = 0;
			gint bps;
			guint32 avg_bps;

			if (d->size)
                p = (d->pos - d->skip) * 100.0 / d->size;
            if (download_filesize(d))
                pt = download_filedone(d) * 100.0 / download_filesize(d);

			bps = bio_bps(d->bio);
			avg_bps = bio_avg_bps(d->bio);

			if (avg_bps <= 10 && d->last_update != d->start_date)
				avg_bps = (d->pos - d->skip) / (d->last_update - d->start_date);

			if (avg_bps) {
				gint slen;
				guint32 s;
				gfloat bs;

                if (d->size > (d->pos - d->skip))
                    s = (d->size - (d->pos - d->skip)) / avg_bps;
                else
                    s=0;

				bs = bps / 1024.0;

				slen = g_snprintf(tmpstr, sizeof(tmpstr),
					"%.02f%% / %.02f%% ", p, pt);

				if (now - d->last_update > IO_STALLED)
					slen += g_snprintf(&tmpstr[slen], sizeof(tmpstr)-slen,
						"(stalled) ");
				else
					slen += g_snprintf(&tmpstr[slen], sizeof(tmpstr)-slen,
						"(%.1f k/s) ", bs);

				g_snprintf(&tmpstr[slen], sizeof(tmpstr)-slen,
					"TR: %s", s ? short_time(s) : "-");
			} else
				g_snprintf(tmpstr, sizeof(tmpstr), "%.02f%%%s", p,
					(now - d->last_update > IO_STALLED) ? " (stalled)" : "");

			a = tmpstr;
		} else
			a = "Connected";
		break;

	case GTA_DL_ERROR:
		a = (gchar *) ((d->remove_msg) ? d->remove_msg : "Unknown Error");
		break;

	case GTA_DL_TIMEOUT_WAIT:
		{
			gint when = d->timeout_delay - (now - d->last_update);
			g_snprintf(tmpstr, sizeof(tmpstr), "Retry in %ds", MAX(0, when));
		}
		a = tmpstr;
		break;
	default:
		g_snprintf(tmpstr, sizeof(tmpstr), "UNKNOWN STATUS %u",
				   d->status);
		a = tmpstr;
	}

	if (d->status != GTA_DL_TIMEOUT_WAIT)
		d->last_gui_update = now;

	if (d->status != GTA_DL_QUEUED) {
		row = gtk_clist_find_row_from_data(clist_downloads, (gpointer) d);
		gtk_clist_set_text(clist_downloads, row, c_dl_status, a);
        if (DOWNLOAD_IS_IN_PUSH_MODE(d))
             gtk_clist_set_foreground(clist_downloads, row, color);
	}
    if (d->status == GTA_DL_QUEUED) {
        GtkCList *clist_downloads_queue = GTK_CLIST
            (lookup_widget(main_window, "clist_downloads_queue"));

		row = gtk_clist_find_row_from_data
            (clist_downloads_queue, (gpointer) d);
		gtk_clist_set_text(clist_downloads_queue, row, c_queue_status, a);
        if (d->always_push)
             gtk_clist_set_foreground(clist_downloads_queue, row, color);
	}
}

void gui_update_download_server(struct download *d)
{
	gint row;
    GtkCList *clist_downloads = GTK_CLIST
            (lookup_widget(main_window, "clist_downloads"));

	g_assert(d);
	g_assert(d->status != GTA_DL_QUEUED);
	g_assert(d->server);
	g_assert(download_vendor(d));

	row = gtk_clist_find_row_from_data(clist_downloads,	(gpointer) d);
	gtk_clist_set_text(clist_downloads, row, c_dl_server, download_vendor(d));
}

void gui_update_download_range(struct download *d)
{
	guint32 len;
	gchar *and_more = "";
	gint rw;
	gint row;
    GtkCList *clist_downloads = GTK_CLIST
            (lookup_widget(main_window, "clist_downloads"));

	g_assert(d);
	g_assert(d->status != GTA_DL_QUEUED);

	if (d->file_info->use_swarming) {
		len = d->size;
		if (d->range_end > d->skip + d->size)
			and_more = "+";
	} else
		len = d->range_end - d->skip;

	len += d->overlap_size;

	rw = g_snprintf(tmpstr, sizeof(tmpstr), "%s%s",
		compact_size(len), and_more);

	if (d->skip)
		g_snprintf(&tmpstr[rw], sizeof(tmpstr)-rw, " @ %s",
			compact_size(d->skip));

	row = gtk_clist_find_row_from_data(clist_downloads,	(gpointer) d);
	gtk_clist_set_text(clist_downloads, row, c_dl_range, tmpstr);
}

void gui_update_c_downloads(gint c, gint ec)
{
    GtkProgressBar *pg = GTK_PROGRESS_BAR
        (lookup_widget(main_window, "progressbar_downloads"));
    gfloat frac;

	g_snprintf(tmpstr, sizeof(tmpstr), "%u/%u download%s", c, ec,
			   (c == 1 && ec == 1) ? "" : "s");
    
    frac = MIN(c, ec) != 0 ? (float)MIN(c, ec) / ec : 0;

    gtk_progress_bar_set_text(pg, tmpstr);
    gtk_progress_bar_set_fraction(pg, frac);
}

void gui_update_download_abort_resume(void)
{
	struct download *d;
	GList *l;
    GtkCList *clist_downloads;
	gboolean abort  = FALSE;
    gboolean resume = FALSE;
    gboolean remove = FALSE;
    gboolean queue  = FALSE;
    gboolean abort_sha1 = FALSE;

    clist_downloads = GTK_CLIST(lookup_widget(main_window, "clist_downloads"));


	for (l = clist_downloads->selection; l; l = l->next) {
		d = (struct download *)
			gtk_clist_get_row_data(clist_downloads, (gint) l->data);

        if (!d) {
			g_warning
				("gui_update_download_abort_resume(): row %d has NULL data\n",
				 (gint) l->data);
			continue;
		}

		g_assert(d->status != GTA_DL_REMOVED);

		switch (d->status) {
		case GTA_DL_COMPLETED:
		case GTA_DL_VERIFY_WAIT:
		case GTA_DL_VERIFYING:
		case GTA_DL_VERIFIED:
			break;
		default:
			queue = TRUE;
			break;
		}

        if (d->sha1 != NULL)
            abort_sha1 = TRUE;

		switch (d->status) {
		case GTA_DL_QUEUED:
			fprintf(stderr, "gui_update_download_abort_resume(): "
				"found queued download '%s' in active download list !\n",
				d->file_name);
			continue;
		case GTA_DL_CONNECTING:
		case GTA_DL_PUSH_SENT:
		case GTA_DL_FALLBACK:
		case GTA_DL_REQ_SENT:
		case GTA_DL_HEADERS:
		case GTA_DL_RECEIVING:
			abort = TRUE;
			break;
		case GTA_DL_ERROR:
		case GTA_DL_ABORTED:
			resume = TRUE;
            /* only check if file exists if really necessary */
            if (!remove && download_file_exists(d))
                remove = TRUE;
			break;
		case GTA_DL_TIMEOUT_WAIT:
			abort = resume = TRUE;
			break;
		}

		if (abort & resume & remove)
			break;
	}

	gtk_widget_set_sensitive
        (lookup_widget(main_window, "button_downloads_abort"), abort);
	gtk_widget_set_sensitive
        (lookup_widget(popup_downloads, "popup_downloads_abort"), abort);
    gtk_widget_set_sensitive
        (lookup_widget(popup_downloads, "popup_downloads_abort_named"), abort);
    gtk_widget_set_sensitive
        (lookup_widget(popup_downloads, "popup_downloads_abort_host"), abort);
    gtk_widget_set_sensitive(
        lookup_widget(popup_downloads, "popup_downloads_abort_sha1"), 
        abort_sha1);
	gtk_widget_set_sensitive
        (lookup_widget(main_window, "button_downloads_resume"), resume);
	gtk_widget_set_sensitive
        (lookup_widget(popup_downloads, "popup_downloads_resume"), resume);
    gtk_widget_set_sensitive
        (lookup_widget(popup_downloads, "popup_downloads_remove_file"), remove);
    gtk_widget_set_sensitive
        (lookup_widget(popup_downloads, "popup_downloads_queue"), queue);
}

void gui_update_queue_frozen()
{
    static gboolean msg_displayed = FALSE;
    static statusbar_msgid_t id = {0, 0};

    GtkWidget *togglebutton_queue_freeze;

    togglebutton_queue_freeze =
        lookup_widget(main_window, "togglebutton_queue_freeze");

    if (gui_debug >= 3)
	printf("frozen %i, msg %i\n", download_queue_is_frozen(),
	    msg_displayed);

    if (download_queue_is_frozen() > 0) {
		gtk_label_set_text(
            GTK_LABEL(GTK_BIN(togglebutton_queue_freeze)->child),
			"Thaw queue");
        if (!msg_displayed) {
            msg_displayed = TRUE;
          	id = statusbar_gui_message(0, "QUEUE FROZEN");
        }
    } else {
		gtk_label_set_text(
            GTK_LABEL(GTK_BIN(togglebutton_queue_freeze)->child),
			"Freeze queue");
        if (msg_displayed) {
            msg_displayed = FALSE;
            statusbar_gui_remove(id);
        }
	} 

    gtk_signal_handler_block_by_func(
        GTK_OBJECT(togglebutton_queue_freeze),
        GTK_SIGNAL_FUNC(on_togglebutton_queue_freeze_toggled),
        NULL);

    gtk_toggle_button_set_active(
        GTK_TOGGLE_BUTTON(togglebutton_queue_freeze),
        download_queue_is_frozen() > 0);
    
    gtk_signal_handler_unblock_by_func(
        GTK_OBJECT(togglebutton_queue_freeze),
        GTK_SIGNAL_FUNC(on_togglebutton_queue_freeze_toggled),
        NULL);
}
