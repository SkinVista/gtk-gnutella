/*
 * Copyright (c) 2001-2003, Richard Eckart
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

#ifndef _if_ui_gtk_statusbar_h_
#define _if_ui_gtk_statusbar_h_

#include "common.h"

typedef enum {
    SB_MESSAGE,
    SB_WARNING
} sb_types_t;

typedef struct {
    guint scid;
    guint msgid;
} statusbar_msgid_t;

/*
 * Public interface, visible from the bridge.
 */

#ifdef GUI_SOURCES

statusbar_msgid_t statusbar_gui_message(guint timeout,
		const gchar *fmt, ...) G_PRINTF(2, 3);

statusbar_msgid_t statusbar_gui_warning(guint timeout,
		const gchar *fmt, ...) G_PRINTF(2, 3);

#endif /* GUI_SOURCES */
#endif /* _if_ui_gtk_statusbar_h_ */

/* vi: set ts=4 sw=4 cindent: */
