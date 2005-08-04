/*
 * $Id$
 *
 * Copyright (c) 2005, Raphael Manfredi
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
 * @ingroup core
 * @file
 *
 * Network driver -- chunked-encoding level.
 *
 * This driver performed a "chunked" encoding of the data it receives
 * before transmitting them.  The specification of that encoding are
 * those of HTTP/1.1, namely:
 *
 *     Chunked-Body   = *chunk
 *                      last-chunk
 *                      trailer
 *                      CRLF
 *
 *     chunk          = chunk-size [ chunk-extension ] CRLF
 *                      chunk-data CRLF
 *     chunk-size     = 1*HEX
 *     last-chunk     = 1*("0") [ chunk-extension ] CRLF
 *
 *     chunk-extension= *( ";" chunk-ext-name [ "=" chunk-ext-val ] )
 *     chunk-ext-name = token
 *     chunk-ext-val  = token | quoted-string
 *     chunk-data     = chunk-size(OCTET)
 *     trailer        = *(entity-header CRLF)
 *
 * @author Raphael Manfredi
 * @date 2005
 */

#include "common.h"

RCSID("$Id$");

#include "tx.h"
#include "tx_chunk.h"

#include "if/gnet_property_priv.h"

#include "lib/cq.h"
#include "lib/glib-missing.h"
#include "lib/walloc.h"
#include "lib/override.h"		/* Must be the last header included */

/*
 * The driver chunks data as it receives it but once the length has been
 * committed, any unwritten data by the lower layer needs to be held by
 * upper layers until we have some room to send them.
 */

#define CHUNK_DIGITS	4		/* At most that many digits in hexa */

/*
 * Private attributes for the link.
 */
struct attr {
	gchar head[CHUNK_DIGITS+5];	/**< Chunk header: hexa size + 2CRLF + NUL */
	ssize_t head_len;			/**< Length of chunk header */
	ssize_t head_remain;		/**< Amount of unwritten header data */
	ssize_t data_remain;		/**< Data required to complete chunk */
	gboolean first;				/**< True for first chunk */
	tx_closed_t closed;			/**< Callback to invoke when layer closed */
	gpointer closed_arg;		/**< Argument for closing routine */
};

/**
 * Flush the chunk header by sending it to the wire.
 *
 * @return TRUE if we were able to flush the whole header.
 */
static gboolean
chunk_flush_header(txdrv_t *tx)
{
	struct attr *attr = tx->opaque;
	ssize_t offset;				/* Offset within head[] for data to TX */
	ssize_t r;

	g_assert(attr->head_len > 0);
	g_assert(attr->head_remain > 0);

	offset = attr->head_len - attr->head_remain;
	g_assert(offset >= 0);

	r = tx_write(tx->lower, &attr->head[offset], attr->head_remain);

	/*
	 * If we were unable to flush everything, enable servicing from
	 * the lower layer: it will call our service routine when it is
	 * able to accept more data from us.
	 */

	if (r != attr->head_remain)
		tx_srv_enable(tx->lower);

	if (r <= 0)
		return FALSE;

	attr->head_remain -= r;
	g_assert(attr->head_remain >= 0);

	return 0 == attr->head_remain;		/* TRUE if we sent everything */
}

/**
 * Begin new chunk of said length, committing to write that much at least
 * until the new chunk header.
 *
 * @return TRUE if we were able to flush the whole header.
 */
static gboolean
chunk_begin(txdrv_t *tx, size_t len, gboolean final)
{
	struct attr *attr = tx->opaque;
	gint hlen = 0;
	
	g_assert(0 == attr->data_remain);
	g_assert(0 == attr->head_remain);
	g_assert(final || len > 0);

	if (dbg > 9)
		printf("chunk_begin: %s chunk %d byte%s\n",
			final ? "final" :
			attr->first ? "first" :
			"next", len, len == 1 ? "" : "s");

	/*
	 * Build the chunk header, committing on sending `len' bytes of data.
	 * The final chunk header is followed by a trailing CRLF.
	 *
	 * If it's not the first chunk emitted, we must end the previous
	 * data with another CRLF.
	 */

	if (!attr->first)
		hlen = gm_snprintf(attr->head, sizeof(attr->head), "\r\n");
	
	if (final)
		hlen += gm_snprintf(&attr->head[hlen], sizeof(attr->head) - hlen,
			"0\r\n\r\n");
	else
		hlen += gm_snprintf(&attr->head[hlen], sizeof(attr->head) - hlen,
			"%x\r\n", len);

	attr->head_len = attr->head_remain = hlen;
	attr->data_remain = len;
	attr->first = FALSE;

	/*
	 * Flush the chunk header.
	 */

	return chunk_flush_header(tx);
}

/**
 * Compute how much data we can still send within the current chunk, knowing
 * that we're offered `len' bytes from the upper layer..
 */
static ssize_t
chunk_acceptable(txdrv_t *tx, size_t len)
{
	struct attr *attr = tx->opaque;

	g_assert(attr->data_remain >= 0);
	g_assert(attr->head_remain >= 0);

	/*
	 * If we haven't committed any length yet, this begins a new chunk.
	 */

	if (attr->data_remain + attr->head_remain == 0) {
		if (!chunk_begin(tx, len, FALSE))
			return 0;				/* Flow-controlled */
	} else if (attr->head_remain) {
		if (!chunk_flush_header(tx))
			return 0;				/* Flow-controlled */
	}

	return MIN(attr->data_remain, (ssize_t) len);
}

/**
 * Service routine for the chunking stage.
 *
 * Called by lower layer when it is ready to process more data.
 */
static void
chunk_service(gpointer data)
{
	txdrv_t *tx = data;
	struct attr *attr = tx->opaque;

	/*
	 * If we have a pending header to send, do it now.
	 */

	if (attr->head_remain && !chunk_flush_header(tx))
		return;

	/*
	 * We don't buffer data, so we can disable servicing from lower layer.
	 */

	tx_srv_disable(tx->lower);

	/*
	 * If we're closing, we're done.
	 */

	if (tx->flags & TX_CLOSING) {
		(*attr->closed)(tx, attr->closed_arg);
		return;
	}

	/*
	 * And now, service upper layer if it requested it.
	 */

	if (tx->flags & TX_SERVICE) {
		g_assert(tx->srv_routine);
		tx->srv_routine(tx->srv_arg);
	}
}

/***
 *** Polymorphic routines.
 ***/

/**
 * Initialize the driver.
 *
 * Always succeeds, so never returns NULL.
 */
static gpointer
tx_chunk_init(txdrv_t *tx, gpointer unused_args)
{
	struct attr *attr;

	(void) unused_args;
	g_assert(tx);

	attr = walloc(sizeof(*attr));

	attr->head_remain = 0;		/* No committed length yet */
	attr->data_remain = 0;		/* No data yet */
	attr->first = TRUE;

	tx->opaque = attr;

	/*
	 * Register our service routine to the lower layer.
	 */

	tx_srv_register(tx->lower, chunk_service, tx);

	return tx;		/* OK */
}

/**
 * Get rid of the driver's private data.
 */
static void
tx_chunk_destroy(txdrv_t *tx)
{
	struct attr *attr = (struct attr *) tx->opaque;

	wfree(attr, sizeof(*attr));
}

/**
 * Write data buffer.
 *
 * @return amount of data bytes written, or -1 on error.
 */
static ssize_t
tx_chunk_write(txdrv_t *tx, gpointer data, size_t len)
{
	struct attr *attr = (struct attr *) tx->opaque;
	size_t remain = len;
	gchar *ptr = data;
	size_t written = 0;

	g_assert(len > 0);

	while (remain > 0) {
		ssize_t acceptable = chunk_acceptable(tx, remain);
		ssize_t r;

		if (acceptable == 0)	/* Could not flush header probably */
			break;
		
		r = tx_write(tx->lower, ptr, acceptable);

		if (r > 0) {
			ptr += r;
			written += r;
			attr->data_remain -= r;
			remain -= r;
		}

		if (r < 0)
			return -1;			/* Lower layer has invoked error callback */

		if (r != acceptable)
			break;
	}

	/*
	 * If we did not write everything, ask the lower layer to invoke our
	 * service routine when there is some room again...
	 */

	if (written != len)
		tx_srv_enable(tx->lower);

	return written;
}

/**
 * Write I/O vector.
 *
 * @return amount of bytes written, or -1 on error.
 */
static ssize_t
tx_chunk_writev(txdrv_t *tx, struct iovec *iov, gint iovcnt)
{
	ssize_t written = 0;

	/*
	 * XXX Highly inefficient because each item in the vector can be
	 * a standalone chunk... Will do for now -- RAM, 2005-08-02.
	 */

	while (iovcnt--) {
		ssize_t r = tx_chunk_write(tx, iov->iov_base, iov->iov_len);
		if (r > 0)
			written += r;
		if ((size_t) r != iov->iov_len)
			break;
		iov++;
	}

	return written;
}

/**
 * Allow servicing of upper TX queue.
 */
static void
tx_chunk_enable(txdrv_t *unused_tx)
{
	/* Nothing specific */
	(void) unused_tx;
}

/**
 * Disable servicing of upper TX queue.
 */
static void
tx_chunk_disable(txdrv_t *unused_tx)
{
	/* Nothing specific */
	(void) unused_tx;
}

/**
 * @return the amount of data buffered locally.
 */
static size_t
tx_chunk_pending(txdrv_t *tx)
{
	struct attr *attr = (struct attr *) tx->opaque;

	return attr->head_remain;
}

/**
 * Send buffered data, if any.
 */
static void
tx_chunk_flush(txdrv_t *tx)
{
	struct attr *attr = (struct attr *) tx->opaque;

	/*
	 * Try to flush pending header data.
	 */

	if (attr->head_remain)
		chunk_flush_header(tx);
}

/**
 * Disable all further transmission
 */
static void
tx_chunk_shutdown(txdrv_t *tx)
{
	struct attr *attr = (struct attr *) tx->opaque;

	/*
	 * Discard header / data remaining to be sent.
	 */

	attr->head_remain = 0;
	attr->data_remain = 0;
}

/**
 * Close the layer, flushing all the data there is.
 * Once this is done, invoke the supplied callback.
 */
static void
tx_chunk_close(txdrv_t *tx, tx_closed_t cb, gpointer arg)
{
	struct attr *attr = (struct attr *) tx->opaque;

	g_assert(tx->flags & TX_CLOSING);

	/*
	 * If there is any data remaining to be sent, it means the upper layer
	 * was unable to close correctly, or that we were not supplied all the
	 * promised data.  In any case, it's an error
	 */

	g_assert(attr->head_remain == 0 && attr->data_remain == 0);

	/*
	 * Emit the last chunk header, indicating we're done with data.
	 */

	if (chunk_begin(tx, 0, TRUE)) {
		(*cb)(tx, arg);
		return;
	}

	/*
	 * We were unable to send the whole final chunk header.  Let the
	 * service routine handle it.
	 */

	attr->closed = cb;
	attr->closed_arg = arg;

	tx_srv_enable(tx->lower);
}

static const struct txdrv_ops tx_chunk_ops = {
	tx_chunk_init,		/**< init */
	tx_chunk_destroy,	/**< destroy */
	tx_chunk_write,		/**< write */
	tx_chunk_writev,	/**< writev */
	tx_no_sendto,		/**< sendto */
	tx_chunk_enable,	/**< enable */
	tx_chunk_disable,	/**< disable */
	tx_chunk_pending,	/**< pending */
	tx_chunk_flush,		/**< flush */
	tx_chunk_shutdown,	/**< shutdown */
	tx_chunk_close,		/**< close */
	tx_no_source,		/**< bio_source */
};

const struct txdrv_ops *
tx_chunk_get_ops(void)
{
	return &tx_chunk_ops;
}

/* vi: set ts=4 sw=4 cindent: */
