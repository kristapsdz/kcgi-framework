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
(function(root) {
	'use strict';

	/*
	 * Send a GET query to the give url.
	 * Invokes "setup" before running anything, "error" upon network
	 * error (with the HTTP error code and response text), and
	 * success with the response text on 200.
	 */
	function sendQuery(url, setup, error, success) 
	{
		var xmh = new XMLHttpRequest();
		var v;

		if (null !== setup)
			setup();

		xmh.onreadystatechange = function() {
			v = xmh.responseText;
			if (xmh.readyState === 4 && 
			    xmh.status === 200) {
				if (null !== success)
					success(v);
			} else if (xmh.readyState === 4) {
				if (null !== error)
					error(xmh.status, v);
			}
		};

		xmh.open('GET', url, true);
		xmh.send(null);
	}

	/*
	 * Send a POST query to the give url with the form.
	 * Invokes "setup" before running anything (given the form),
	 * "error" upon network error (with the form, HTTP error code,
	 * and response text), and success with the form and response
	 * text on 200.
	 */
	function sendForm(form, setup, error, success) 
	{
		var xmh = new XMLHttpRequest();
		var v;

		if (null !== setup)
			setup(form);

		xmh.onreadystatechange=function() {
			v = xmh.responseText;
			if (xmh.readyState === 4 && 
			    xmh.status === 200) {
				if (null !== success)
					success(form, v);
			} else if (xmh.readyState === 4) {
				if (null !== error)
					error(form, xmh.status, v);
			}
		};

		xmh.open(form.method, form.action, true);
		xmh.send(new FormData(form));
		return(false);
	}

	/*
	 * Shows (removes the "hide" class) a particular element.
	 * Accepts "root", which is an element or the identifier string
	 * of one.
	 */
	function show(root)
	{
		var e;

		e = typeof root === 'string' ?
		    document.getElementById(root) : root;
		if (null === e)
			return;
		if (e.classList.contains('hide'))
			e.classList.remove('hide');
	}

	/*
	 * Hide all classes named "name" under a particular element.
	 * Accepts "root", which is an element or the identifier string
	 * of one, under which (inclusive) all elements will be queried.
	 * See show().
	 */
	function showcl(root, name)
	{
		var list, i, sz, e;

		e = typeof root === 'string' ?
		    document.getElementById(root) : root;
		if (null === e)
			return;
		list = e.getElementsByClassName(name);
		for (i = 0, sz = list.length; i < sz; i++)
			show(list[i]);
	}

	/*
	 * Simple lookup function for element by name.
	 * Emits a console warning on error.
	 */
	function find(root)
	{
		var e;
		if (null === (e = document.getElementById(root)))
			console.log('cannot find: ' + root);
		return(e);
	}

	/*
	 * Replace all text within an element.
	 * Accepts "root", which is an element or the identifier string
	 * of one.
	 */
	function repl(root, text)
	{
		var e;

		e = typeof root === 'string' ?
		    document.getElementById(root) : root;
		if (null === e)
			return;
		while (e.firstChild)
			e.removeChild(e.firstChild);
		e.appendChild(document.createTextNode(text));
	}

	/*
	 * Replace all text within elements of class "name" under a
	 * particular element.
	 * Accepts "root", which is an element or the identifier string
	 * of one, under which (inclusive) all elements will be queried.
	 * See repl().
	 */
	function replcl(root, name, text)
	{
		var list, i, sz, e;

		e = typeof root === 'string' ?
		    document.getElementById(root) : root;
		if (null === e)
			return;
		list = e.getElementsByClassName(name);
		for (i = 0, sz = list.length; i < sz; i++)
			repl(list[i], text);
	}

	/*
	 * Hide (adds the "hide" class) a particular element.
	 * Accepts "root", which is an element or the identifier string
	 * of one.
	 */
	function hide(root)
	{
		var e;

		e = typeof root === 'string' ?
		    document.getElementById(root) : root;
		if (null === e)
			return;
		if ( ! e.classList.contains('hide'))
			e.classList.add('hide');
	}

	/*
	 * Show all classes named "name" under a particular element.
	 * Accepts "root", which is an element or the identifier string
	 * of one, under which (inclusive) all elements will be queried.
	 * See hide().
	 */
	function hidecl(root, name)
	{
		var list, i, sz, e;

		e = typeof root === 'string' ?
		    document.getElementById(root) : root;
		if (null === e)
			return;
		list = e.getElementsByClassName(name);
		for (i = 0, sz = list.length; i < sz; i++)
			hide(list[i]);
	}

	/*
	 * Generic setup for a JSON form.
	 * Hides the submit button (class "submit"), an error messages
	 * (class "error"), and shows the pending button (class
	 * "pending").
	 */
	function formSetup(e)
	{
		showcl(e, 'pending');
		hidecl(e, 'submit');
		hidecl(e, 'error');
	}
	
	/*
	 * Generic error for a JSON form.
	 * Shows the submit button (class "submit"), an particular error
	 * message (class "error" and the error code; or if greater than
	 * error 500, just 5xx), and hides the pending button (class
	 * "pending").
	 */
	function formError(e, code)
	{
		hidecl(e, 'pending');
		showcl(e, 'submit');
		if (code < 500)
			showcl(e, 'error' + code);
		else
			showcl(e, 'error5xx');
	}

	/*
	 * Generic success (doing nothing).
	 * Shows the submit button (class "submit") and hides the
	 * pending button (class "pending").
	 */
	function formSuccess(e, resp)
	{
		hidecl(e, 'pending');
		showcl(e, 'submit');
	}

	/*
	 * Success that reloads page.
	 * Invokes formSuccess().
	 */
	function formSuccessReload(e, resp)
	{
		formSuccess(e, resp);
		document.location.reload();
	}

	/*
	 * Generic form that does nothing except submit the form
	 * contents, do the usual button swapping, then reload on
	 * success.
	 * This function is asynchronous.
	 */
	function formReloadProc()
	{
		return(sendForm(this, formSetup, 
			formError, formSuccessReload));
	}

	/*
	 * Simply parse a JSON message from the "resp" string.
	 * Returns null on failure, the object on success.
	 */
	function formParse(resp)
	{
		var res;

		try  { 
			res = JSON.parse(resp);
		} catch (error) {
			console.log('JSON parse fail: ' + resp);
			return(null);
		}
		return(res);
	}

	/*
	 * Prepare for receiving the index page.
	 */
	function indexSetup()
	{
		show('loading');
		hide('loaded');
		hide('loggedin');
		hide('login');
	}

	/*
	 * The index.json page was not retrieved.
	 */
	function indexError(code, resp)
	{
		hide('loading');
		show('loaded');
		hide('loggedin');
		show('login');
	}

	/*
	 * The index.json file was retrieved.
	 * Format the page appropriately.
	 */
	function indexSuccess(resp)
	{
		var res, e;

		if (null === (res = formParse(resp))) {
			indexError(500, null);
			return;
		}

		hide('loading');
		show('loaded');
		show('loggedin');
		hide('login');
		replcl(document, 'user-email', res.user.email);
	}

	/*
	 * Configure the index page.
	 */
	function loadIndex()
	{
		/* Reset all existing forms. */

		hidecl(document, 'error');
		hidecl(document, 'pending');
		showcl(document, 'submit');

		/* Set all "submit" handlers. */

		find('modpassform').onsubmit = formReloadProc;
		find('modemailform').onsubmit = formReloadProc;
		find('loginform').onsubmit = formReloadProc;
		find('logoutform').onsubmit = formReloadProc;

		/* Actually try to load the user page. */

		sendQuery('@CGIURI@/index.json', 
			indexSetup, indexError, indexSuccess);
	}

	root.loadIndex = loadIndex;
})(this);

document.addEventListener('DOMContentLoaded', function() {
	loadIndex();
});
