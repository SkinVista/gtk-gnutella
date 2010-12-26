/*
 * $Id$
 *
 * Copyright (c) 2010, Raphael Manfredi
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
 * @ingroup lib
 * @file
 *
 * Name / Value pairs and tables.
 *
 * The name is necessary a string.
 * The value is an arbitrary data buffer.
 *
 * @author Raphael Manfredi
 * @date 2010
 */

#include "common.h"

RCSID("$Id$")

#include "nv.h"
#include "atoms.h"
#include "glib-missing.h"
#include "halloc.h"
#include "hashlist.h"
#include "unsigned.h"
#include "walloc.h"
#include "override.h"		/* Must be the last header included */

enum nv_pair_magic { NV_PAIR_MAGIC = 0x60f7c898U };

/**
 * The name/value pair.
 *
 * This is the actual value we store in the hash table.
 */
struct nv_pair {
	enum nv_pair_magic magic;
	const char *name;				/**< The name (atom) */
	void *value;					/**< The value (halloc()'ed); can be NULL */
	nv_pair_val_free_t value_free;	/**< Value free routine (optional) */
	size_t length;					/**< Length of value (0 if value is NULL) */
	int refcnt;						/**< Reference count */
	unsigned allocated:1;			/**< Whether data was allocated */
};

static inline void
nv_pair_check(const nv_pair_t * const nvp)
{
	g_assert(nvp != NULL);
	g_assert(NV_PAIR_MAGIC == nvp->magic);
	g_assert(nvp->name != NULL);
	g_assert(0 == nvp->length || nvp->value != NULL);
	g_assert(nvp->refcnt > 0);
}

enum nv_table_magic { NV_TABLE_MAGIC = 0x2557a3b2U };

/*
 * A name/value pair table.
 */
struct nv_table {
	enum nv_table_magic magic;
	gboolean ordered;			/**< Whether table is ordered */
	union {						/**< Maps "name" -> nv_pair */
		GHashTable *ht;
		hash_list_t *hl;
	} u;
};

static inline void
nv_table_check(const nv_table_t * const nvt)
{
	g_assert(nvt != NULL);
	g_assert(NV_TABLE_MAGIC == nvt->magic);
}

/**
 * Create a new name/value pair.
 *
 * The name string is always copied, but the value is not, unless "copy" is
 * requested.  If the value is not duplicated, it will not be freed either
 * when the name/value pair is discarded, so the data has better be statically
 * allocated or it will leak, since there is no free callback.
 *
 * @param name		the name
 * @param value		the value buffer (can be NULL)
 * @param length	length of value (must be zero if value is NULL)
 * @param copy		whether we should duplicate the value
 *
 * @return the allocated name/value pair.
 */
static nv_pair_t *
nv_pair_make_full(const char *name, const void *value, size_t length,
	gboolean copy)
{
	nv_pair_t *nvp;

	g_assert(name != NULL);
	g_assert(0 == length || value != NULL);
	g_assert(NULL == value || size_is_positive(length));

	nvp = walloc0(sizeof *nvp);
	nvp->magic = NV_PAIR_MAGIC;
	nvp->refcnt = 1;
	nvp->name = atom_str_get(name);
	if (value != NULL) {
		nvp->value = deconstify_gpointer(copy ? hcopy(value, length) : value);
	} else {
		nvp->value = NULL;
	}
	nvp->length = length;
	nvp->allocated = copy && value != NULL;

	return nvp;
}

/**
 * Create a new name/value pair.
 *
 * The name string is always copied, but the value is not: it has to be
 * statically allocated data or be managed by the creator (freeing it before
 * the name/value pair is freed).
 *
 * If the length is 0, the value must be NULL.
 *
 * @param name		the name
 * @param value		the value buffer (can be NULL)
 * @param length	length of value (must be zero if value is NULL)
 *
 * @return the allocated name/value pair.
 */
nv_pair_t *
nv_pair_make_nocopy(const char *name, const void *value, size_t length)
{
	return nv_pair_make_full(name, value, length, FALSE);
}

/**
 * Create a new name/value pair.
 *
 * A copy of both name and value data (if not NULL) is made.
 * If the length is 0, the value must be NULL.
 *
 * @param name		the name
 * @param value		the value buffer (can be NULL)
 * @param length	length of value (must be zero if value is NULL)
 *
 * @return the allocated name/value pair.
 */
nv_pair_t *
nv_pair_make(const char *name, const void *value, size_t length)
{
	return nv_pair_make_full(name, value, length, TRUE);
}

/**
 * Get the pair name.
 */
const char *
nv_pair_name(const nv_pair_t *nvp)
{
	nv_pair_check(nvp);
	return nvp->name;
}

/**
 * Get the pair value and its length.
 *
 * A NULL means a zero-length quantity.
 * If "retlen" is not NULL, the value length is returned there.
 */
void *
nv_pair_value_len(const nv_pair_t *nvp, size_t *retlen)
{
	nv_pair_check(nvp);

	if (retlen != NULL)
		*retlen = nvp->length;
	return nvp->value;
}

/**
 * Get the pair value, without length information.
 *
 * A NULL means a zero-length quantity.
 *
 * This should only be used for string values, whose end is known, or
 * for fixed-sized objects.  Otherwise, use nv_pair_value_len().
 */
void *
nv_pair_value(const nv_pair_t *nvp)
{
	nv_pair_check(nvp);

	return nvp->value;
}

/**
 * Get the pair value, known to be a string, without length information.
 *
 * This should only be used for string values, whose end is known.
 * Otherwise, use nv_pair_value_len().
 */
const char *
nv_pair_value_str(const nv_pair_t *nvp)
{
	static const char empty[] = "";

	nv_pair_check(nvp);

	return NULL == nvp->value ? empty : nvp->value;
}

/**
 * Get a new reference on an existing name/value pair.
 * @return the nv_pair_t object.
 */
nv_pair_t *
nv_pair_refcnt_inc(nv_pair_t *nvp)
{
	nv_pair_check(nvp);

	nvp->refcnt++;
	return nvp;
}

/**
 * Free the value of the name/value pair, making it a zero-length value.
 */
void
nv_pair_free_value(nv_pair_t *nvp)
{
	nv_pair_check(nvp);
	g_assert(nvp->value != NULL);

	if (nvp->allocated) {
		HFREE_NULL(nvp->value);
	} else if (nvp->value_free != NULL) {
		(*nvp->value_free)(nvp->value, nvp->length);
	}

	nvp->value = NULL;
	nvp->length = 0;
	nvp->allocated = FALSE;
}

/**
 * Destroy a name/value pair.
 */
void
nv_pair_free(nv_pair_t *nvp)
{
	nv_pair_check(nvp);

	if (--(nvp->refcnt) != 0)
		return;

	if (nvp->value != NULL) {
		if (nvp->allocated) {
			HFREE_NULL(nvp->value);
		} else if (nvp->value_free != NULL) {
			(*nvp->value_free)(nvp->value, nvp->length);
			nvp->value = NULL;
		}
	}
	atom_str_free_null(&nvp->name);
	nvp->magic = 0;
	wfree(nvp, sizeof *nvp);
}

/**
 * Destroy a name/value pair and nullify its pointer.
 */
void
nv_pair_free_null(nv_pair_t **nvp_ptr)
{
	nv_pair_t *nvp = *nvp_ptr;

	if (nvp != NULL) {
		nv_pair_free(nvp);
		*nvp_ptr = NULL;
	}
}

/**
 * Set free routine for values.
 */
void
nv_pair_set_value_free(nv_pair_t *nvp, nv_pair_val_free_t vf)
{
	nv_pair_check(nvp);
	g_assert(vf != NULL);
	g_assert(nvp->value != NULL);
	g_assert(!nvp->allocated);

	nvp->value_free = vf;
}

/**
 * Hash a name/value pair by name.
 */
unsigned
nv_pair_hash(const void *key)
{
	const nv_pair_t *nv = key;

	return g_str_hash(nv->name);
}

/**
 * Are two name/vale pairs bearing the same name?
 */
gboolean
nv_pair_eq(const void *k1, const void *k2)
{
	const nv_pair_t *n1 = k1;
	const nv_pair_t *n2 = k2;

	return 0 == strcmp(n1->name, n2->name);
}

/**
 * Create a new name/value table.
 *
 * Optionally it can remember the order in which keys are added so that
 * the hash table can later be traversed in that same order.
 */
nv_table_t *
nv_table_make(gboolean ordered)
{
	nv_table_t *nvt;

	nvt = walloc(sizeof *nvt);
	nvt->magic = NV_TABLE_MAGIC;
	nvt->ordered = ordered;
	if (ordered)
		nvt->u.hl = hash_list_new(nv_pair_hash, nv_pair_eq);
	else
		nvt->u.ht = g_hash_table_new(g_str_hash, g_str_equal);

	return nvt;
}

/**
 * Hash table iterator to free up values in the nv_table_t.
 */
static void
nv_table_ht_free_value(void *u_key, void *value, void *u_data)
{
	(void) u_key;
	(void) u_data;
	nv_pair_free(value);
}

/**
 * Hash list iterator to free up values in the nv_table_t.
 */
static void
nv_table_hl_free_value(void *key, void *u_data)
{
	(void) u_data;
	nv_pair_free(key);
}

/**
 * Free a name/value table.
 */
void
nv_table_free(nv_table_t *nvt)
{
	nv_table_check(nvt);

	if (nvt->ordered) {
		hash_list_foreach(nvt->u.hl, nv_table_hl_free_value, NULL);
		hash_list_free(&nvt->u.hl);
	} else {
		g_hash_table_foreach(nvt->u.ht, nv_table_ht_free_value, NULL);
		gm_hash_table_destroy_null(&nvt->u.ht);
	}
	nvt->magic = 0;
	wfree(nvt, sizeof *nvt);
}

/**
 * Free a name/value table and nullify its pointer.
 */
void
nv_table_free_null(nv_table_t **nvt_ptr)
{
	nv_table_t *nvt = *nvt_ptr;

	if (nvt != NULL) {
		nv_table_free(nvt);
		*nvt_ptr = NULL;
	}
}

/**
 * Insert a name value pair into the table.
 *
 * If an entry already existed for that name, it is superseding the old value
 * which is freed.
 */
void
nv_table_insert_pair(const nv_table_t *nvt, nv_pair_t *nvp)
{
	nv_pair_t *old;
	void *pos = NULL;

	nv_table_check(nvt);
	nv_pair_check(nvp);

	/*
	 * If table is ordered and the name/value pair already existed, the new
	 * one is inserted at the same position as the old one.
	 */

	old = nv_table_lookup(nvt, nvp->name);
	if (old != NULL) {
		if (nvt->ordered)
			pos = hash_list_remove_position(nvt->u.hl, old);
		else
			g_hash_table_remove(nvt->u.ht, nvp->name);
		nv_pair_free(old);
	}

	if (nvt->ordered) {
		if (pos != NULL)
			hash_list_insert_position(nvt->u.hl, nvp, pos);
		else
			hash_list_append(nvt->u.hl, nvp);
	} else {
		gm_hash_table_insert_const(nvt->u.ht, nvp->name, nvp);
	}
}

/**
 * Record a name/value pair.
 *
 * The name and value are copied.
 *
 * @param nvt		the name/value table
 * @param name		the name of the pair
 * @param value		the value to insert (may be NULL)
 * @param length	the length of the value (0 if value is NULL)
 */
void
nv_table_insert(const nv_table_t *nvt,
	const char *name, const void *value, size_t length)
{
	nv_pair_t *nvp;

	nv_table_check(nvt);

	nvp = nv_pair_make_full(name, 0 == length ? NULL : value, length, TRUE);
	nv_table_insert_pair(nvt, nvp);
}

/**
 * Record a name/value pair.
 *
 * The name is atomized (copied) but the value is NOT copied.
 *
 * @param nvt		the name/value table
 * @param name		the name of the pair
 * @param value		the value to insert (may be NULL)
 * @param length	the length of the value (0 if value is NULL)
 */
void
nv_table_insert_nocopy(const nv_table_t *nvt,
	const char *name, const void *value, size_t length)
{
	nv_pair_t *nvp;

	nv_table_check(nvt);

	nvp = nv_pair_make_full(name, 0 == length ? NULL : value, length, FALSE);
	nv_table_insert_pair(nvt, nvp);
}

/**
 * Remove name/value pair.
 *
 * @return TRUE if name/value pair existed.
 */
gboolean
nv_table_remove(const nv_table_t *nvt, const char *name)
{
	nv_pair_t *nvp;

	nv_table_check(nvt);
	g_assert(name != NULL);

	nvp = nv_table_lookup(nvt, name);
	if (NULL == nvp)
		return FALSE;

	if (nvt->ordered)
		hash_list_remove(nvt->u.hl, nvp);
	else
		g_hash_table_remove(nvt->u.ht, nvp->name);

	nv_pair_free(nvp);
	return TRUE;
}

/**
 * Get name/value pair from table.
 *
 * @return NULL if name/value pair does not exist, otherwise the nv_pair_t.
 */
nv_pair_t *
nv_table_lookup(const nv_table_t *nvt, const char *name)
{
	nv_table_check(nvt);
	g_assert(name != NULL);

	if (nvt->ordered) {
		nv_pair_t key;
		const void *nvp;

		key.name = name;

		if (hash_list_find(nvt->u.hl, &key, &nvp))
			return deconstify_gpointer(nvp);
		else
			return NULL;
	} else {
		return g_hash_table_lookup(nvt->u.ht, name);
	}
}

/**
 * Get value string associated with a name/value pair in the table.
 *
 * @return NULL if name/value pair does not exist, otherwise the string value.
 */
const char *
nv_table_lookup_str(const nv_table_t *nvt, const char *name)
{
	nv_pair_t *nvp;

	nv_table_check(nvt);
	g_assert(name != NULL);

	nvp = nv_table_lookup(nvt, name);
	if (NULL == nvp)
		return NULL;

	return nv_pair_value_str(nvp);	/* Guaranteed to not be NULL */
}

/**
 * Return amount of entries in the table.
 */
size_t
nv_table_count(const nv_table_t *nvt)
{
	nv_table_check(nvt);

	if (nvt->ordered)
		return hash_list_length(nvt->u.hl);
	else
		return g_hash_table_size(nvt->u.ht);
}

struct nvt_foreach_remove_ctx {
	nv_table_cbr_t func;		/**< User callback */
	void *data;					/**< User additional argument data */
};

static gboolean
nv_table_ht_foreach_rwrap(gpointer ukey, gpointer value, gpointer data)
{
	nv_pair_t *nvp = value;
	struct nvt_foreach_remove_ctx *ctx = data;

	nv_pair_check(nvp);
	(void) ukey;

	return (*ctx->func)(nvp, ctx->data);
}

static gboolean
nv_table_hl_foreach_rwrap(void *key, void *data)
{
	nv_pair_t *nvp = key;
	struct nvt_foreach_remove_ctx *ctx = data;

	nv_pair_check(nvp);

	return (*ctx->func)(nvp, ctx->data);
}


/**
 * Iterate over table, optionally removing entries when callback returns TRUE.
 *
 * @param nvt		the table
 * @param func		callback invoked for each item
 * @param data		user-defined argument
 *
 * @return the number of entries removed.
 */
unsigned
nv_table_foreach_remove(const nv_table_t *nvt, nv_table_cbr_t func, void *data)
{
	struct nvt_foreach_remove_ctx ctx;

	nv_table_check(nvt);

	ctx.func = func;
	ctx.data = data;

	return nvt->ordered ?
		hash_list_foreach_remove(nvt->u.hl, nv_table_hl_foreach_rwrap, &ctx) :
		g_hash_table_foreach_remove(nvt->u.ht, nv_table_ht_foreach_rwrap, &ctx);

}

struct nvt_foreach_ctx {
	nv_table_cb_t func;			/**< User callback */
	void *data;					/**< User additional argument data */
};

static void
nv_table_ht_foreach_wrap(gpointer ukey, gpointer value, gpointer data)
{
	nv_pair_t *nvp = value;
	struct nvt_foreach_ctx *ctx = data;

	nv_pair_check(nvp);
	(void) ukey;

	(*ctx->func)(nvp, ctx->data);
}

static void
nv_table_hl_foreach_wrap(void *key, void *data)
{
	nv_pair_t *nvp = key;
	struct nvt_foreach_ctx *ctx = data;

	nv_pair_check(nvp);

	(*ctx->func)(nvp, ctx->data);
}

/**
 * Iterate over table.
 *
 * @param nvt		the table
 * @param func		callback invoked for each item
 * @param data		user-defined argument
 */
void
nv_table_foreach(const nv_table_t *nvt, nv_table_cb_t func, void *data)
{
	struct nvt_foreach_ctx ctx;

	nv_table_check(nvt);

	ctx.func = func;
	ctx.data = data;

	if (nvt->ordered)
		hash_list_foreach(nvt->u.hl, nv_table_hl_foreach_wrap, &ctx);
	else
		g_hash_table_foreach(nvt->u.ht, nv_table_ht_foreach_wrap, &ctx);
}

/* vi: set ts=4 sw=4 cindent: */
