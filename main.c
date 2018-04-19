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
#include "config.h"

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

/*
 * Start with five pages.
 * As you add more pages, you'll start by giving them an identifier key
 * in this enum.
 */
enum	page {
	PAGE_INDEX,
	PAGE_LOGIN,
	PAGE_LOGOUT,
	PAGE_USER_MOD_EMAIL,
	PAGE_USER_MOD_PASS,
	PAGE__MAX
};

enum	key {
	KEY_EMAIL,
	KEY_PASS,
	KEY_SESSTOK,
	KEY_SESSID,
	KEY__MAX
};

static const struct kvalid keys[KEY__MAX] = {
	{ kvalid_email, "email" }, /* KEY_EMAIL */
	{ kvalid_stringne, "pass" }, /* KEY_PASS */
	{ kvalid_uint, "stok" }, /* KEY_SESSTOK */
	{ kvalid_int, "sid" }, /* KEY_SESSID */
};

static const char *const pages[PAGE__MAX] = {
	"index", /* PAGE_INDEX */
	"login", /* PAGE_LOGIN */
	"logout", /* PAGE_LOGOUT */
	"usermodemail", /* PAGE_USER_MOD_EMAIL */
	"usermodpass", /* PAGE_USER_MOD_PASS */
};

/*
 * Fill out all HTTP secure headers.
 * Use the existing document's MIME type.
 */
static void
http_alloc(struct kreq *r, enum khttp code)
{

	khttp_head(r, kresps[KRESP_STATUS], 
		"%s", khttps[code]);
	khttp_head(r, kresps[KRESP_CONTENT_TYPE], 
		"%s", kmimetypes[r->mime]);
	khttp_head(r, "X-Content-Type-Options", "nosniff");
	khttp_head(r, "X-Frame-Options", "DENY");
	khttp_head(r, "X-XSS-Protection", "1; mode=block");
}

/*
 * Fill out all headers with http_alloc() then start the HTTP document
 * body (no more headers after this point!)
 */
static void
http_open(struct kreq *r, enum khttp code)
{

	http_alloc(r, code);
	khttp_body(r);
}

static void
json_emptydoc(struct kreq *r)
{
	struct kjsonreq	 req;

	kjson_open(&req, r);
	kjson_obj_open(&req);
	kjson_obj_close(&req);
	kjson_close(&req);
}

/*
 * Process an e-mail address change.
 * Raises HTTP 400 if not all fields exist or if the e-mail address is
 * already taken by the system.
 * Raises HTTP 200 on success.
 */
static void
sendmodemail(struct kreq *r, const struct user *u)
{
	struct kpair	*kp;

	if (NULL != (kp = r->fieldmap[KEY_EMAIL])) {
		db_user_update_email(r->arg, kp->parsed.s, u->id);
		http_open(r, KHTTP_200);
	} else
		http_open(r, KHTTP_400);

	json_emptydoc(r);
}

/*
 * Process a password change.
 * Raises HTTP 400 if not all fields exist.
 * Raises HTTP 200 on success.
 */
static void
sendmodpass(struct kreq *r, const struct user *u)
{
	struct kpair	*kp;

	if (NULL != (kp = r->fieldmap[KEY_PASS])) {
		db_user_update_pass(r->arg, kp->parsed.s, u->id);
		http_open(r, KHTTP_200);
	} else
		http_open(r, KHTTP_400);

	json_emptydoc(r);
}

/*
 * Retrieve user information.
 * Raises HTTP 200 on success and the JSON of the user.
 */
static void
sendindex(struct kreq *r, const struct user *u)
{
	struct kjsonreq	 req;

	http_open(r, KHTTP_200);
	kjson_open(&req, r);
	kjson_obj_open(&req);
	json_user_obj(&req, u);
	kjson_obj_close(&req);
	kjson_close(&req);
}

/*
 * Log in the given user by their e-mail and password.
 * Creates a new session.
 * Returns HTTP 400 if missing parameters, bad user, bad password, etc.
 * Returns HTTP 200 with empty JSON body and cookie headers.
 */
static void
sendlogin(struct kreq *r)
{
	int64_t		 sid, token;
	struct kpair	*kpi, *kpp;
	char		 buf[64];
	struct user	*u;
	const char	*secure;

	if (NULL == (kpi = r->fieldmap[KEY_EMAIL]) ||
	    NULL == (kpp = r->fieldmap[KEY_PASS])) {
		http_open(r, KHTTP_400);
		json_emptydoc(r);
		return;
	}

	u = db_user_get_creds(r->arg, kpi->parsed.s, kpp->parsed.s);

	if (NULL == u) {
		http_open(r, KHTTP_400);
		json_emptydoc(r);
		return;
	} 

	token = arc4random();
	sid = db_sess_insert(r->arg, u->id, token);
	kutil_epoch2str
		(time(NULL) + 60 * 60 * 24 * 365,
		 buf, sizeof(buf));
#ifdef SECURE
	secure = " secure;";
#else
	secure = "";
#endif
	khttp_head(r, kresps[KRESP_STATUS], 
		"%s", khttps[KHTTP_200]);
	khttp_head(r, kresps[KRESP_SET_COOKIE],
		"%s=%" PRId64 ";%s HttpOnly; path=/; expires=%s", 
		keys[KEY_SESSTOK].name, token, secure, buf);
	khttp_head(r, kresps[KRESP_SET_COOKIE],
		"%s=%" PRId64 ";%s HttpOnly; path=/; expires=%s", 
		keys[KEY_SESSID].name, sid, secure, buf);
	khttp_body(r);
	json_emptydoc(r);
	db_user_free(u);
}

/*
 * Log out the given user by deleting the user's session (if found) and
 * invalidating the client-side session.
 * Returns HTTP 200 with empty JSON body.
 */
static void
sendlogout(struct kreq *r, const struct sess *s)
{
	const char	*secure;
	char		 buf[32];

	kutil_epoch2str(0, buf, sizeof(buf));
#ifdef SECURE
	secure = " secure;";
#else
	secure = "";
#endif
	http_alloc(r, KHTTP_200);
	khttp_head(r, kresps[KRESP_SET_COOKIE],
		"%s=; path=/;%s HttpOnly; expires=%s", 
		keys[KEY_SESSTOK].name, secure, buf);
	khttp_head(r, kresps[KRESP_SET_COOKIE],
		"%s=; path=/;%s HttpOnly; expires=%s", 
		keys[KEY_SESSID].name, secure, buf);
	khttp_body(r);
	json_emptydoc(r);
	db_sess_delete_id(r->arg, s->id, s->token);
}

int
main(void)
{
	struct kreq	 r;
	enum kcgi_err	 er;
	struct sess	*s;

	/* Log into a separate logfile (not system log). */

	kutil_openlog(LOGFILE);

	/* Actually parse HTTP document. */

	er = khttp_parse(&r, keys, KEY__MAX, 
		pages, PAGE__MAX, PAGE_INDEX);

	if (KCGI_OK != er) {
		fprintf(stderr, "HTTP parse error: %d\n", er);
		return(EXIT_FAILURE);
	}

#if HAVE_PLEDGE
	if (-1 == pledge("stdio rpath cpath wpath flock fattr", NULL)) {
		kutil_warn(&r, NULL, "pledge");
		khttp_free(&r);
		return(EXIT_FAILURE);
	}
#endif

	/*
	 * Front line of defence: make sure we're a proper method, make
	 * sure we're a page, make sure we're a JSON file.
	 */

	if (KMETHOD_GET != r.method && 
	    KMETHOD_POST != r.method) {
		http_open(&r, KHTTP_405);
		khttp_free(&r);
		return(EXIT_SUCCESS);
	} else if (PAGE__MAX == r.page || 
	           KMIME_APP_JSON != r.mime) {
		http_open(&r, KHTTP_404);
		khttp_puts(&r, "Page not found.");
		khttp_free(&r);
		return(EXIT_SUCCESS);
	}

	if (NULL == (r.arg = db_open(DATADIR "/yourprog.db"))) {
		http_open(&r, KHTTP_500);
		json_emptydoc(&r);
		khttp_free(&r);
		return(EXIT_SUCCESS);
	}

	/* 
	 * Assume we're logging in with a session and grab the session
	 * from the database.
	 * This is our first database access.
	 */

	s = db_sess_get_creds(r.arg,
		NULL != r.cookiemap[KEY_SESSID] ?
		r.cookiemap[KEY_SESSID]->parsed.i : -1,
		NULL != r.cookiemap[KEY_SESSTOK] ?
		r.cookiemap[KEY_SESSTOK]->parsed.i : -1);

	/* User authorisation. */

	if (PAGE_LOGIN != r.page && NULL == s) {
		http_open(&r, KHTTP_403);
		json_emptydoc(&r);
		db_close(r.arg);
		khttp_free(&r);
		return(EXIT_SUCCESS);
	}

	switch (r.page) {
	case (PAGE_INDEX):
		sendindex(&r, &s->user);
		break;
	case (PAGE_LOGIN):
		sendlogin(&r);
		break;
	case (PAGE_LOGOUT):
		sendlogout(&r, s);
		break;
	case (PAGE_USER_MOD_EMAIL):
		sendmodemail(&r, &s->user);
		break;
	case (PAGE_USER_MOD_PASS):
		sendmodpass(&r, &s->user);
		break;
	default:
		abort();
	}

	db_sess_free(s);
	db_close(r.arg);
	khttp_free(&r);
	return(EXIT_SUCCESS);
}
