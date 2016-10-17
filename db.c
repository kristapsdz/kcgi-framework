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
#include <inttypes.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <kcgi.h>
#include <kcgijson.h>
#include <ksql.h>

#include "extern.h"

enum	stmt {
	STMT_SESS_DEL,
	STMT_SESS_GET,
	STMT_SESS_NEW,
	STMT_USER_GET,
	STMT_USER_LOOKUP,
	STMT_USER_MOD_EMAIL,
	STMT_USER_MOD_HASH,
	STMT__MAX
};

/*
 * The fields we'll extract (in general) from the user table.
 */
#define	USER	"user.id,user.email"

static	const char *const stmts[STMT__MAX] = {
	/* STMT_SESS_DEL */
	"DELETE FROM sess WHERE id=? AND token=? AND userid=?",
	/* STMT_SESS_GET */
	"SELECT " USER " FROM sess "
		"INNER JOIN user ON user.id=sess.userid "
		"WHERE sess.id=? AND sess.token=?",
	/* STMT_SESS_NEW */
	"INSERT INTO sess (token,userid) VALUES (?,?)",
	/* STMT_USER_GET */
	"SELECT " USER " FROM user WHERE id=?",
	/* STMT_USER_LOOKUP */
	"SELECT " USER ",hash FROM user WHERE email=?",
	/* STMT_USER_MOD_EMAIL */
	"UPDATE user SET email=? WHERE id=?",
	/* STMT_USER_MOD_HASH */
	"UPDATE user SET hash=? WHERE id=?",
};

/*
 * Free the contents of the "struct user".
 */
static void
db_user_unfill(struct user *p)
{

	if (NULL == p)
		return;
	free(p->email);
}

/*
 * Free the object of the "struct user".
 */
void
db_user_free(struct user *p)
{

	db_user_unfill(p);
	free(p);
}

/*
 * Zero and fill the "struct user" from the database.
 * Sets "pos", if non-NULL, to be the current index in the database
 * columns using the USER column definition macro.
 */
static void
db_user_fill(struct user *p, struct ksqlstmt *stmt, size_t *pos)
{
	size_t	 i = 0;

	if (NULL == pos)
		pos = &i;

	memset(p, 0, sizeof(struct user));
	p->id = ksql_stmt_int(stmt, (*pos)++);
	p->email = kstrdup(ksql_stmt_str(stmt, (*pos)++));
}

/*
 * Create a new user session with a random token.
 * Returns the identifier of the new session.
 */
int64_t
db_sess_new(struct kreq *r, int64_t token, const struct user *u)
{
	struct ksqlstmt	*stmt;
	int64_t		 id;

	ksql_stmt_alloc(r->arg, &stmt, 
		stmts[STMT_SESS_NEW], 
		STMT_SESS_NEW);
	ksql_bind_int(stmt, 0, token);
	ksql_bind_int(stmt, 1, u->id);
	ksql_stmt_step(stmt);
	ksql_stmt_free(stmt);
	ksql_lastid(r->arg, &id);
	kutil_info(r, u->email, "new session");
	return(id);
}

/*
 * Look up a given user with an e-mail and password.
 * The password is hashed in a system-dependent way.
 * Returns the user object or NULL if no user was found or the password
 * was incorrect.
 */
struct user *
db_user_find(struct kreq *r, const char *email, const char *pass)
{
	struct ksqlstmt	*stmt;
	int		 rc;
	size_t		 i;
	const char	*hash;
	struct user	*user;

	ksql_stmt_alloc(r->arg, &stmt, 
		stmts[STMT_USER_LOOKUP], 
		STMT_USER_LOOKUP);
	ksql_bind_str(stmt, 0, email);
	if (KSQL_ROW != ksql_stmt_step(stmt)) {
		ksql_stmt_free(stmt);
		return(NULL);
	}
	i = 0;
	user = kmalloc(sizeof(struct user));
	db_user_fill(user, stmt, &i);
	hash = ksql_stmt_str(stmt, i);
#ifdef __OpenBSD__
	rc = crypt_checkpass(pass, hash) < 0 ? 0 : 1;
#else
	rc = 0 == strcmp(hash, pass);
#endif
	ksql_stmt_free(stmt);
	if (0 == rc) {
		db_user_free(user);
		user = NULL;
	}
	return(user);
}

/*
 * Resolve a user from session information.
 * Returns the user object or NULL if no session was found.
 */
struct user *
db_sess_resolve(struct kreq *r, int64_t id, int64_t token)
{
	struct ksqlstmt	*stmt;
	struct user	*u = NULL;

	if (-1 == id || -1 == token)
		return(NULL);

	ksql_stmt_alloc(r->arg, &stmt, 
		stmts[STMT_SESS_GET], 
		STMT_SESS_GET);
	ksql_bind_int(stmt, 0, id);
	ksql_bind_int(stmt, 1, token);
	if (KSQL_ROW == ksql_stmt_step(stmt)) {
		u = kmalloc(sizeof(struct user));
		db_user_fill(u, stmt, NULL);
	}
	ksql_stmt_free(stmt);
	return(u);
}

/*
 * Deletes the session (if any) associated with the given id and token,
 * and managed by the given user.
 */
void
db_sess_del(struct kreq *r, 
	const struct user *u, int64_t id, int64_t token)
{
	struct ksqlstmt	*stmt;

	ksql_stmt_alloc(r->arg, &stmt, 
		stmts[STMT_SESS_DEL], 
		STMT_SESS_DEL);
	ksql_bind_int(stmt, 0, id);
	ksql_bind_int(stmt, 1, token);
	ksql_bind_int(stmt, 2, u->id);
	ksql_stmt_step(stmt);
	ksql_stmt_free(stmt);
	kutil_info(r, u->email, "session deleted");
}

/*
 * Modify a user's password.
 */
void
db_user_mod_pass(struct kreq *r, 
	const struct user *u, const char *pass)
{
	struct ksqlstmt	*stmt;
	char		 hash[64];

#ifdef __OpenBSD__
	crypt_newhash(pass, "blowfish,a", hash, sizeof(hash));
#else
	strlcpy(hash, pass, sizeof(hash));
#endif
	ksql_stmt_alloc(r->arg, &stmt,
		stmts[STMT_USER_MOD_HASH],
		STMT_USER_MOD_HASH);
	ksql_bind_str(stmt, 0, hash);
	ksql_bind_int(stmt, 1, u->id);
	ksql_stmt_step(stmt);
	ksql_stmt_free(stmt);
	kutil_info(r, u->email, "changed password");
}

/*
 * Modify a user's e-mail.
 */
int
db_user_mod_email(struct kreq *r, 
	const struct user *u, const char *email)
{
	struct ksqlstmt	*stmt;
	enum ksqlc	 c;

	ksql_stmt_alloc(r->arg, &stmt, 
		stmts[STMT_USER_MOD_EMAIL], 
		STMT_USER_MOD_EMAIL);
	ksql_bind_str(stmt, 0, email);
	ksql_bind_int(stmt, 1, u->id);
	c = ksql_stmt_cstep(stmt);
	ksql_stmt_free(stmt);
	if (KSQL_CONSTRAINT != c)
		kutil_info(r, u->email, "changed email: %s", email);
	return(KSQL_CONSTRAINT != c);
}

/*
 * Open the database and stash the resulting handle in the d
 */
int
db_open(struct kreq *r, const char *file)
{
	struct ksqlcfg	 cfg;
	struct ksql	*sql;

	/* Configure normal database except with foreign keys. */

	memset(&cfg, 0, sizeof(struct ksqlcfg));
	cfg.flags = KSQL_EXIT_ON_ERR |
		    KSQL_FOREIGN_KEYS |
		    KSQL_SAFE_EXIT;
	cfg.err = ksqlitemsg;
	cfg.dberr = ksqlitedbmsg;

	/* Allocate database. */

	if (NULL == (sql = ksql_alloc(&cfg)))
		return(0);

	ksql_open(sql, file);
	r->arg = sql;
	return(1);
}

/*
 * Close the database stashed in the kreq's argument.
 */
void
db_close(struct kreq *r)
{

	ksql_free(r->arg);
	r->arg = NULL;
}
