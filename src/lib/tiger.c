/*
 * $Id$
 *
 * Copyright (c) 2003, Jeroen Asselman
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

/* (PD) 2001 The Bitzi Corporation
 * Please see file COPYING or http://bitzi.com/publicdomain 
 * for more info.
 *
 * Created and released into the public domain by Eli Biham
 *
 * $Bitzi: tiger.c,v 1.4 2004/02/01 06:19:31 gojomo Exp $
 */

/**
 * @ingroup lib
 * @file
 *
 * Tiger hash.
 *
 * This file comes from http://www.cs.technion.ac.il/~biham/Reports/Tiger/
 *
 * Inclusion in gtk-gnutella is:
 *
 * @author Jeroen Asselman
 * @date 2003
 */

#include "common.h"

RCSID("$Id$");

#include "endian.h"
#include "misc.h"
#include "base32.h"
#include "tiger.h"
#include "override.h"		/* Must be the last header included */

/*
 * The following macro denotes that an optimization
 * for 64-bit CPUs is required. It is used only for
 * optimization of time. Otherwise it does nothing.
 */
#if (G_MAXULONG > 0xffffffffUL)
#define OPTIMIZE_FOR_64BIT
#endif

/* The following macro denotes that an optimization    */
/* for Alpha is required. It is used only for          */
/* optimization of time. Otherwise it does nothing.    */
#ifdef __alpha
#define OPTIMIZE_FOR_ALPHA
#endif

/* NOTE that this code is NOT FULLY OPTIMIZED for any  */
/* machine. Assembly code might be much faster on some */
/* machines, especially if the code is compiled with   */
/* gcc.                                                */

/* The number of passes of the hash function.          */
/* Three passes are recommended.                       */
/* Use four passes when you need extra security.       */
/* Must be at least three.                             */
#define PASSES 3

#include "tiger_sboxes.h"

#define U64_FROM_2xU32(hi, lo) (((guint64) (hi) << 32) | (lo))

#define t1 (tiger_sboxes)
#define t2 (&tiger_sboxes[256])
#define t3 (&tiger_sboxes[256*2])
#define t4 (&tiger_sboxes[256*3])

#define save_abc \
      aa = a; \
      bb = b; \
      cc = c;

#ifdef OPTIMIZE_FOR_ALPHA
/* This is the official definition of round */
#define round(a,b,c,x,mul) \
      c ^= x; \
      a -= t1[((c)>>(0*8))&0xFF] ^ t2[((c)>>(2*8))&0xFF] ^ \
	   t3[((c)>>(4*8))&0xFF] ^ t4[((c)>>(6*8))&0xFF] ; \
      b += t4[((c)>>(1*8))&0xFF] ^ t3[((c)>>(3*8))&0xFF] ^ \
	   t2[((c)>>(5*8))&0xFF] ^ t1[((c)>>(7*8))&0xFF] ; \
      b *= mul;
#else
/* This code works faster when compiled on 32-bit machines */
/* (but works slower on Alpha) */
#define round(a,b,c,x,mul) \
      c ^= x; \
      a -= t1[(guint8)(c)] ^ \
           t2[(guint8)(((guint32)(c))>>(2*8))] ^ \
	   t3[(guint8)((c)>>(4*8))] ^ \
           t4[(guint8)(((guint32)((c)>>(4*8)))>>(2*8))] ; \
      b += t4[(guint8)(((guint32)(c))>>(1*8))] ^ \
           t3[(guint8)(((guint32)(c))>>(3*8))] ^ \
	   t2[(guint8)(((guint32)((c)>>(4*8)))>>(1*8))] ^ \
           t1[(guint8)(((guint32)((c)>>(4*8)))>>(3*8))]; \
      b *= mul;
#endif

#define pass(a,b,c,mul) \
      round(a,b,c,x0,mul) \
      round(b,c,a,x1,mul) \
      round(c,a,b,x2,mul) \
      round(a,b,c,x3,mul) \
      round(b,c,a,x4,mul) \
      round(c,a,b,x5,mul) \
      round(a,b,c,x6,mul) \
      round(b,c,a,x7,mul)

#define key_schedule \
      x0 -= x7 ^ U64_FROM_2xU32(0xA5A5A5A5UL, 0xA5A5A5A5UL); \
      x1 ^= x0; \
      x2 += x1; \
      x3 -= x2 ^ ((~x1)<<19); \
      x4 ^= x3; \
      x5 += x4; \
      x6 -= x5 ^ ((~x4)>>23); \
      x7 ^= x6; \
      x0 += x7; \
      x1 -= x0 ^ ((~x7)<<19); \
      x2 ^= x1; \
      x3 += x2; \
      x4 -= x3 ^ ((~x2)>>23); \
      x5 ^= x4; \
      x6 += x5; \
      x7 -= x6 ^ U64_FROM_2xU32(0x01234567UL,  0x89ABCDEFUL);

#define feedforward \
      a ^= aa; \
      b -= bb; \
      c += cc;

#ifdef OPTIMIZE_FOR_ALPHA
/* The loop is unrolled: works better on Alpha */
#define compress \
      save_abc \
      pass(a,b,c,5) \
      key_schedule \
      pass(c,a,b,7) \
      key_schedule \
      pass(b,c,a,9) \
      for(pass_no=3; pass_no<PASSES; pass_no++) { \
        key_schedule \
	pass(a,b,c,9) \
	tmpa=a; a=c; c=b; b=tmpa;} \
      feedforward
#else
/* loop: works better on PC and Sun (smaller cache?) */
#define compress \
      save_abc \
      for(pass_no=0; pass_no<PASSES; pass_no++) { \
        if(pass_no != 0) {key_schedule} \
	pass(a,b,c,(pass_no==0?5:pass_no==1?7:9)); \
	tmpa=a; a=c; c=b; b=tmpa;} \
      feedforward
#endif

#define tiger_compress_macro(str, state) \
{ \
  guint64 a, b, c, tmpa; \
  guint64 aa, bb, cc; \
  guint64 x0, x1, x2, x3, x4, x5, x6, x7; \
  int pass_no; \
\
  a = state[0]; \
  b = state[1]; \
  c = state[2]; \
\
  x0=str[0]; x1=str[1]; x2=str[2]; x3=str[3]; \
  x4=str[4]; x5=str[5]; x6=str[6]; x7=str[7]; \
\
  compress; \
\
  state[0] = a; \
  state[1] = b; \
  state[2] = c; \
}

/* The compress function is a function. Requires smaller cache?    */
static inline void
tiger_compress(const guint64 *data, guint64 state[3])
{
  tiger_compress_macro(data, state);
}

#ifdef OPTIMIZE_FOR_ALPHA
/* The compress function is inlined: works better on Alpha.        */
/* Still leaves the function above in the code, in case some other */
/* module calls it directly.                                       */
#define tiger_compress(str, state) tiger_compress_macro((str), (state))
#endif /* OPTIMIZE_FOR_ALPHA */

void
tiger(gconstpointer data, guint64 length, guchar hash[24])
{
  guint64 i, j, res[3];
  const guint8 *data_u8 = data;
  union {
    guint64 u64[8];
    guint8 u8[64];
  } temp;

  res[0] = U64_FROM_2xU32(0x01234567UL, 0x89ABCDEFUL);
  res[1] = U64_FROM_2xU32(0xFEDCBA98UL, 0x76543210UL);
  res[2] = U64_FROM_2xU32(0xF096A5B4UL, 0xC3B2E187UL);

#if G_BYTE_ORDER == G_BIG_ENDIAN
  for (i = length; i >= 64; i -= 64) {
    for (j = 0; j < 64; j++) {
      temp.u8[j ^ 7] = data_u8[j];
    }
    tiger_compress(temp.u64, res);
    data_u8 += 64;
  }
#else	/* !BIG ENDIAN */
  if ((gulong) data & 7) {
    for (i = length; i >= 64; i -= 64) {
      memcpy(temp.u64, data_u8, 64);
      tiger_compress(temp.u64, res);
      data_u8 += 64;
    }
  } else {
    for (i = length; i >= 64; i -= 64) {
      tiger_compress((gconstpointer) data_u8, res);
      data_u8 += 64;
    }
  }
#endif	/* BIG ENDIAN */

#if G_BYTE_ORDER == G_BIG_ENDIAN
  for (j = 0; j < i; j++) {
    temp.u8[j ^ 7] = data_u8[j];
  }

  temp.u8[j ^ 7] = 0x01;
  j++;
  for (; j & 7; j++) {
    temp.u8[j ^ 7] = 0;
  }
#else
  for(j = 0; j < i; j++) {
    temp.u8[j] = data_u8[j];
  }

  temp.u8[j++] = 0x01;
  for (; j & 7; j++) {
    temp.u8[j] = 0;
  }
#endif

  if (j > 56) {
    for (; j < 64; j++) {
      temp.u8[j] = 0;
    }
    tiger_compress(temp.u64, res);
    j = 0;
  }

  for (; j < 56; j++) {
    temp.u8[j] = 0;
  }
  temp.u64[7] = length << 3;
  tiger_compress(temp.u64, res);

  for (i = 0; i < 3; i++) {
    poke_le64(&hash[i * 8], res[i]);
  }
}

/* vi: set ai et sts=2 sw=2 cindent: */
/**
 * Runs some test cases to check whether the implementation of the tiger
 * hash algorithm is alright.
 */
void
tiger_check(void)
{
	static const gchar zeros[1025];
    static const struct {
		const char *r;
		const char *s;
		size_t len;
	} tests[] = {
		{ "QMLU34VTTAIWJQM5RVN4RIQKRM2JWIFZQFDYY3Y", "\0" "1", 2 },
		{ "LWPNACQDBZRYXW3VHJVCJ64QBZNGHOHHHZWCLNQ", zeros, 1 },
		{ "VK54ZIEEVTWNAUI5D5RDFIL37LX2IQNSTAXFKSA", zeros, 2 },
		{ "KIU5YUNESS4RH6HAJRGHFHETZOFSMDFE52HKTVY", zeros, 8 },
		{ "Z5PUAX6MEZB6EWYXFCSLMMUMZEFIQPOEWX3BA6Q", zeros, 255 },
		{ "D6UXHPOSAGHITCD4VVRHJQ4PCKIWY2WEHPJOUWY", zeros, 1024 },
		{ "CMKDYROZKSC6VTM4I7LSMMHPAE4UG3FXPXZGGKY", zeros, sizeof zeros },
	};
	guint i;

	for (i = 0; i < G_N_ELEMENTS(tests); i++) {
		guchar hash[24];
		gchar buf[40];
		gboolean ok;

		memset(buf, 0, sizeof buf);	
		tiger(tests[i].s, tests[i].len, hash);
		base32_encode_into(cast_to_gpointer(hash), sizeof hash,
			buf, sizeof buf);
		buf[G_N_ELEMENTS(buf) - 1] = '\0';

		ok = 0 == strcmp(tests[i].r, buf);
		if (!ok) {
			g_warning("i=%u, buf=\"%s\"", i, buf);
			g_assert_not_reached();
		}
	}
}

/* vi: set ts=4 sw=4 cindent: */
