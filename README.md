## Synopsis

**kcgi-framework** is a set of file priming a [BCHS](https://learnbchs.org)
project using [kcgi](https://kristaps.bsd.lv/kcgi) and
[openradtool](https://kristaps.bsd.lv/openradtool).

The framework implements logging in and logging out over a session-based,
RESTful JSON API.  It includes the full application stack:

- database initialisation
- database interface (C language)
- REST interface (JSON)
- front-end formatting (JavaScript)

It also includes a skeleton [port](https://man.openbsd.org/ports) for
[OpenBSD](https://www.openbsd.org).

The existing code tries to follow best practises for all elements of the
stack: clean ([style(9)](https://man.openbsd.org/style.9)), secure C
code that's logically separated, HTML and JavaScript that are
well-formed and satisfy CSP policies, and so on.

All parts are documented as fully as possible, from the database schema
to the REST API.

The framework is portable for most modern UNIX systems as provided by
[oconfigure](https://github.com/kristapsdz/oconfigure).  This includes
OpenBSD, NetBSD, FreeBSD, Linux, Mac OS X, Solaris, and IllumOS.

You'll only use this framework *once* for your project---it's not
something that's installed.

## Installation

You'll need an up-to-date [kcgi](https://kristaps.bsd.lv/kcgi) and
[openradtool](https://kristaps.bsd.lv/openradtool).

1. Copy all files into your project directory.

   Then read and edit the [Makefile](Makefile): it will list all of the
   variables you'll need to set for your installation.  Override these
   in a *Makefile.local* or directly in the Makefile.

2. Review [yourprog.ort](yourprog.ort).  This describes your data model
   and how data passes into and from the database.  Its syntax is
   documented in
   [ort(5)](https://kristaps.bsd.lv/openradtool/ort.5.html).

3. Browse [main.c](main.c).  It uses the interfaces created by
   [ort-c-header(1)](https://kristaps.bsd.lv/openradtool/ort-c-header.1.html)
   to drive the REST API.

4.  Read [index.xml](index.xml) and [index.js](index.js), both of
    which interface the REST API.

You should probably use [TypeScript](https://www.typescriptlang.org/)
instead of JavaScript, but I'm keeping this simple.  Realistically that
only means installing the TypeScript compiler, adding some TypeScript to
JavaScript in the Makefile, and that's it.

Then deploy in the usual way:

```
% ./configure
% make
# make install
```

There are several additional targets:

- `installwww`: only HTML and JS sources

- `installapi`: REST [OAS2.0](https://www.openapis.org/) API docs

- `installcgi`: CGI script and a fresh copy of the database
  (**warning**: this will replace the existing database)

- `updatecgi`: install only the CGI script

## Users

If you want to create a user, use
[encrypt(1)](https://man.openbsd.org/encrypt.1) to generate a password
hash and insert those into the database prior to `make installcgi`.
(The hashing method depends upon the operating system.)

Assuming a user `foo@bar.com` and substituting `yourpassword` for your
password and `yourhash` for the output of the encryption:

```sh
% make
% echo 'yourpassword' | encrypt
% sqlite3 yourprog.db
sqlite> insert into user values ('foo@bar.com', 'yourhash', 1);
sqlite> .quit
```

You can also run this on your live database, of course.

## Package management

**kcgi-framework** includes the building blocks for an OpenBSD
[port](https://man.openbsd.org/ports.7) by including necessary files.

The port is in the [OpenBSD](openbsd) directory.  As-is, it installs the
CGI script in */var/www/cgi-bin* and the database and database
specification in */var/www/data*.

The port uses the `yourprog-upgrade` script, which generates the
difference between the database specification for the existing database
in */var/www/data* with the current version's specification.  It then
patches the database and installs the current specification.  This keeps
your database smoothly up to date.

It's possible to embed this directly into the port *PLIST*, if desired.

Of course, this is something you'll need to carefully test!  It uses
[ort-sqldiff(1)](https://kristaps.bsd.lv/openradtool/ort-sqldiff.1.html),
which has its limitations.

## License

All sources use the ISC (like OpenBSD) license.
See the [LICENSE.md](LICENSE.md) file for details.
