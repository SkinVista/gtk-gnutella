/*
 * $Id$
 *
 * Copyright (c) 2004, Raphael Manfredi
 * Copyright (c) 2003, Markus Goetz
 *
 * This file is based a lot on the whitelist stuff by vidar.
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

/**
 * @file
 *
 * Support for the hostiles.txt of BearShare
 */

#include "common.h"

RCSID("$Id$");

#include "hostiles.h"
#include "settings.h"
#include "nodes.h"

#include "lib/file.h"
#include "lib/misc.h"
#include "lib/glib-missing.h"
#include "lib/iprange.h"
#include "lib/walloc.h"
#include "lib/watcher.h"

#include "if/gnet_property.h"
#include "if/gnet_property_priv.h"
#include "if/bridge/c2ui.h"

#include "lib/override.h"		/* Must be the last header included */

#define THERE	GUINT_TO_POINTER(0x2)

typedef enum {
	HOSTILE_GLOBAL = 0,
	HOSTILE_PRIVATE = 1,

	NUM_HOSTILES
} hostiles_t;

static const gchar hostiles_file[] = "hostiles.txt";
static const gchar hostiles_what[] = "hostile IP addresses";

static gpointer hostile_db[NUM_HOSTILES];		/* The hostile database */

/**
 * Frees all entries in the given hostiles
 */
static void
hostiles_close_one(hostiles_t which)
{
	g_assert((gint) which >= 0 && which < NUM_HOSTILES);
	
	if (hostile_db[which]) {
		iprange_free_each(hostile_db[which], NULL);
		hostile_db[which] = NULL;
	}
}

/**
 * Load hostile data from the supplied FILE.
 * Returns the amount of entries loaded.
 */
static gint
hostiles_load(FILE *f, hostiles_t which)
{
	gchar line[1024];
	gchar *p;
	guint32 ip, netmask;
	int linenum = 0;
	gint count = 0;
	gint bits;
	iprange_err_t error;

	g_assert((gint) which >= 0 && which < NUM_HOSTILES);
	g_assert(NULL == hostile_db[which]);

	hostile_db[which] = iprange_make(NULL, NULL);

	while (fgets(line, sizeof(line), f)) {
		linenum++;
		if (*line == '\0' || *line == '#')
			continue;

		/*
		 * Remove all trailing spaces in string.
		 * Otherwise, lines which contain only spaces would cause a warning.
		 */

		p = strchr(line, '\0');
		while (--p >= line) {
			guchar c = (guchar) *p;
			if (!is_ascii_space(c))
				break;
			*p = '\0';
		}
		if ('\0' == *line)
			continue;

		if (!gchar_to_ip_and_mask(line, &ip, &netmask)) {
			g_warning("%s, line %d: invalid IP or netmask \"%s\"",
				hostiles_file, linenum, line);
			continue;
		}

		bits = 32;
		while (0 == (netmask & 0x1)) {
			netmask >>= 1;
			bits--;
		}

		error = iprange_add_cidr(hostile_db[which], ip, bits, THERE);

		switch (error) {
		case IPR_ERR_OK:
			break;
		case IPR_ERR_RANGE_OVERLAP:
			error = iprange_add_cidr_force(hostile_db[which],
						ip, bits, THERE, NULL);
			if (dbg > 0 && error == IPR_ERR_OK) {
				g_warning("%s: line %d: "
					"entry \"%s\" (%s/%d) superseded earlier smaller range",
					hostiles_file, linenum, line, ip_to_gchar(ip), bits);
				break;
			}
			/* FALL THROUGH */
		default:
			if (dbg > 0 || error != IPR_ERR_RANGE_SUBNET) {
				g_warning("%s, line %d: rejected entry \"%s\" (%s/%d): %s",
					hostiles_file, linenum, line, ip_to_gchar(ip), bits,
					iprange_strerror(error));
			}
			continue;
		}

		count++;
	}

	if (dbg) {
		iprange_stats_t stats;

		iprange_get_stats(hostile_db[which], &stats);

		g_message("loaded %d hostile IP addresses/netmasks", count);
		g_message("hostile stats: count=%d level2=%d heads=%d enlisted=%d",
			stats.count, stats.level2, stats.heads, stats.enlisted);
	}

	return count;
}

/**
 * Watcher callback, invoked when the file from which we read the hostile
 * addresses changed.
 */
static void
hostiles_changed(const gchar *filename, gpointer udata)
{
	FILE *f;
	gchar buf[80];
	gint count;
	hostiles_t which;

	which = GPOINTER_TO_UINT(udata);
	g_assert((gint) which >= 0 && which < NUM_HOSTILES);

	f = file_fopen(filename, "r");
	if (f == NULL)
		return;

	hostiles_close_one(which);
	count = hostiles_load(f, which);
	fclose(f);

	gm_snprintf(buf, sizeof(buf), "Reloaded %d hostile IP addresses.", count);
	gcu_statusbar_message(buf);
}

static void
hostiles_retrieve_from_file(FILE *f, hostiles_t which,
	const gchar *path, const gchar *filename)
{
	gchar *pathname;

	g_assert(f);
	g_assert(path);
	g_assert(filename);
	g_assert((gint) which >= 0 && which < NUM_HOSTILES);
	
	pathname = make_pathname(path, filename);
	watcher_register(pathname, hostiles_changed, GUINT_TO_POINTER(which));
	G_FREE_NULL(pathname);
	hostiles_load(f, which);
}

/**
 * Loads the hostiles.txt into memory, choosing the first file we find
 * among the several places we look at, typically:
 *
 *    ~/.gtk-gnutella/hostiles.txt
 *    /usr/share/gtk-gnutella/hostiles.txt
 *    /home/src/gtk-gnutella/hostiles.txt
 *
 * The selected file will then be monitored and a reloading will occur
 * shortly after a modification.
 */
static void
hostiles_retrieve(hostiles_t which)
{
	g_assert((gint) which >= 0 && which < NUM_HOSTILES);

	switch (which) {
	case HOSTILE_PRIVATE:
		{
			FILE *f;
			gint idx;
			file_path_t fp_private[1];

			file_path_set(&fp_private[0], settings_config_dir(), hostiles_file);
			f = file_config_open_read_norename_chosen(
				hostiles_what, fp_private, G_N_ELEMENTS(fp_private), &idx);

			if (f) {
				hostiles_retrieve_from_file(f, HOSTILE_PRIVATE,
					fp_private[idx].dir, fp_private[idx].name);
				fclose(f);
			}
		}
		break;

	case HOSTILE_GLOBAL:
		{
			FILE *f;
			gint idx;
			static const file_path_t const fp[] = {
#ifndef OFFICIAL_BUILD
				{ PACKAGE_SOURCE_DIR, hostiles_file },
#endif
				{ PRIVLIB_EXP, hostiles_file },
			};
			

			f = file_config_open_read_norename_chosen(
				hostiles_what, fp, G_N_ELEMENTS(fp), &idx);
			if (f) {
				hostiles_retrieve_from_file(f,
				HOSTILE_GLOBAL, fp[idx].dir, fp[idx].name);
				fclose(f);
			}
		}
		break;

	case NUM_HOSTILES:
		g_assert_not_reached();
	}
}

/**
 * If the property was set to FALSE at startup time, hostile_db[HOSTILE_GLOBAL]
 * is still NULL and we need to load the global hostiles.txt now. Otherwise,
 * there's nothing to do, hostiles_check() will simply ignore
 * hostile_db[HOSTILE_GLOBAL]. The file watcher keeps running though during
 * this session and we keep the database in memory.
 */
static gboolean
use_global_hostiles_txt_changed(property_t unused_prop)
{
	(void) unused_prop;

	if (use_global_hostiles_txt && !hostile_db[HOSTILE_GLOBAL]) {
		hostiles_retrieve(HOSTILE_GLOBAL);
	}
	
    return FALSE;
}

/**
 * Called on startup. Loads the hostiles.txt into memory.
 */
void
hostiles_init(void)
{
	hostiles_retrieve(HOSTILE_PRIVATE);
    gnet_prop_add_prop_changed_listener(PROP_USE_GLOBAL_HOSTILES_TXT,
		use_global_hostiles_txt_changed, TRUE);
}

/**
 * Frees all entries in all the hostiles
 */
void
hostiles_close(void)
{
	gint i;

	for (i = 0; i < NUM_HOSTILES; i++) {
		hostiles_close_one(i);
	}
}

/**
 * Check the given IP against the entries in the hostiles.
 * Returns TRUE if found, and FALSE if not.
 */
gboolean
hostiles_check(guint32 ip)
{
	gint i;

	for (i = 0; i < NUM_HOSTILES; i++) {
		if (i == HOSTILE_GLOBAL && !use_global_hostiles_txt)
			continue;

		if (NULL != hostile_db[i] && THERE == iprange_get(hostile_db[i], ip))
			return TRUE;
	}
	
	return FALSE;
}

/* vi: set ts=4 sw=4 cindent: */
