/*
 * Copyright (c) 2003 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/*-
 * Copyright (c) 1991, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)mpool.h	8.2 (Berkeley) 7/14/94
 * $FreeBSD: src/include/mpool.h,v 1.9 2002/03/23 17:24:53 imp Exp $
 */

#ifndef _MPOOL_H_
#define _MPOOL_H_

#include <sys/queue.h>

/*
 * The memory pool scheme is a simple one.  Each in-memory page is referenced
 * by a bucket which is threaded in up to two of three ways.  All active pages
 * are threaded on a hash chain (hashed by page number) and an lru chain.
 * Inactive pages are threaded on a free chain.  Each reference to a memory
 * pool is handed an opaque MPOOL cookie which stores all of this information.
 */
#define	HASHSIZE	128
#define	HASHKEY(pgno)	((pgno - 1) % HASHSIZE)

/* The BKT structures are the elements of the queues. */
typedef struct _bkt {
	TAILQ_ENTRY(_bkt) hq;		/* hash queue */
	TAILQ_ENTRY(_bkt) q;		/* lru queue */
	void    *page;			/* page */
	pgno_t   pgno;			/* page number */

#define	MPOOL_DIRTY	0x01		/* page needs to be written */
#define	MPOOL_PINNED	0x02		/* page is pinned into memory */
	u_int8_t flags;			/* flags */
} BKT;

typedef struct MPOOL {
	TAILQ_HEAD(_lqh, _bkt) lqh;	/* lru queue head */
					/* hash queue array */
	TAILQ_HEAD(_hqh, _bkt) hqh[HASHSIZE];
	pgno_t	curcache;		/* current number of cached pages */
	pgno_t	maxcache;		/* max number of cached pages */
	pgno_t	npages;			/* number of pages in the file */
	u_long	pagesize;		/* file page size */
	int	fd;			/* file descriptor */
					/* page in conversion routine */
	void    (*pgin)(void *, pgno_t, void *);
					/* page out conversion routine */
	void    (*pgout)(void *, pgno_t, void *);
	void	*pgcookie;		/* cookie for page in/out routines */
#ifdef STATISTICS
	u_long	cachehit;
	u_long	cachemiss;
	u_long	pagealloc;
	u_long	pageflush;
	u_long	pageget;
	u_long	pagenew;
	u_long	pageput;
	u_long	pageread;
	u_long	pagewrite;
#endif
} MPOOL;

__BEGIN_DECLS
MPOOL	*mpool_open(void *, int, pgno_t, pgno_t);
void	 mpool_filter(MPOOL *, void (*)(void *, pgno_t, void *),
	    void (*)(void *, pgno_t, void *), void *);
void	*mpool_new(MPOOL *, pgno_t *);
void	*mpool_get(MPOOL *, pgno_t, u_int);
int	 mpool_put(MPOOL *, void *, u_int);
int	 mpool_sync(MPOOL *);
int	 mpool_close(MPOOL *);
#ifdef STATISTICS
void	 mpool_stat(MPOOL *);
#endif
__END_DECLS

#endif
