/*	$Id$ */
/*
 * Copyright (c) 2016 Kristaps Dzonsons <kristaps@bsd.lv>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef EXTERN_H
#define EXTERN_H

/*
 * A user.
 * See the user table in the database schema.
 */
struct	user {
	char		*email; /* canonicalised e-mail */
	int64_t		 id; /* unique identifier */
};

__BEGIN_DECLS

void		 db_close(struct kreq *);
int		 db_open(struct kreq *, const char *);

struct user	*db_user_find(struct kreq *, 
			const char *, const char *);
void		 db_user_free(struct user *);
int		 db_user_mod_email(struct kreq *, 
			const struct user *, const char *);
void		 db_user_mod_pass(struct kreq *, 
			const struct user *, const char *);

void		 db_sess_del(struct kreq *, 
			const struct user *, int64_t, int64_t);
int64_t		 db_sess_new(struct kreq *, 
			int64_t, const struct user *);
struct user	*db_sess_resolve(struct kreq *, int64_t, int64_t);

__END_DECLS

#endif
