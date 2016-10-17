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
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <kcgi.h>
#include <kcgijson.h>

#include "extern.h"

/*
 * Emit an empty JSON document.
 */
void
json_emptydoc(struct kreq *r)
{
	struct kjsonreq	 req;

	kjson_open(&req, r);
	kjson_obj_open(&req);
	kjson_obj_close(&req);
	kjson_close(&req);
}

/*
 * Formats a user object's members as JSON members.
 */
void
json_putuserdata(struct kjsonreq *req, const struct user *u)
{

	kjson_putstringp(req, "email", u->email);
	kjson_putintp(req, "id", u->id);
}

/*
 * Formats a user object as a JSON object.
 */
void
json_putuser(struct kjsonreq *req, const struct user *u)
{

	kjson_objp_open(req, "user");
	json_putuserdata(req, u);
	kjson_obj_close(req);
}

