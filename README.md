## Synopsis

kcgi-framework is a set of files for starting a
[BCHS](https://learnbchs.org) project that uses
[kcgi](https://kristaps.bsd.lv/kcgi),
[ksql](https://kristaps.bsd.lv/ksql), and
[kwebapp](https://kristaps.bsd.lv/kwebapp).

The system, as is, minimally implements logging in and logging out over a
session-based, RESTful JSON API.  It has lots of documentation:

- initial database documentation via
  [sqliteconvert](https://kristaps.bsd.lv/sqliteconvert)
- RESTful documentation via [Swagger](https://swagger.io)
- well-documented C and JS source code

The existing code tries to follow best practises for all elements of the
stack: clean, secure C code that's logically separated, HTML and
JavaScript that are well-formed and satisfy CSP policies, and so on.

It's also portable within UNIX systems as provided by
[oconfigure](https://github.com/kristapsdz/oconfigure).

You'll only use this repository once as a primer for your project.  It's
not something that's installed.  I use this to quickly get started on a
project without needing to re-implement basis user management.

## Installation

This is a framework (or foundation), so you'll use this to get your
project started.  Begin by copying all files into your project
directory.  You'll also need [kcgi](https://kristaps.bsd.lv/kcgi) and
[ksql](https://kristaps.bsd.lv/ksql),
[kwebapp](https://kristaps.bsd.lv/kwebapp), and
[sqliteconvert](https://kristaps.bsd.lv/sqliteconvert) if you plan on
using the database documentation.

Begin by reading the [Makefile](Makefile): it will list all of the
variables you'll need to set for your installation.  Override these in a
`Makefile.local`.  The default values assume an OpenBSD system with a
stock install.   You'll probably need to set the following:

```
CPPFLAGS = -I/usr/local/include
LDFLAGS = -L/usr/local/lib
```

Then read the [main.c](main.c) and [yourprog.kwbp](yourprog.kwbp).
The former drives the output of the latter when pushed through
[kwebapp](https://kristaps.bsd.lv/kwebapp) in various ways.

Finally, read [index.xml](index.xml) and [index.js](index.js), both of
which drive the JSON backend.  It's all super-simple and self-contained.
([kwebapp](https://kristaps.bsd.lv/kwebapp) can also provide this, but I
erred on the side of simplicity.)

Before running `make`, you'll need to run `./configure` to generate the
compatibility layer.

The current [Makefile](Makefile) rules are as follows.

Run `make` to compile the sources.

Run `make installwww` to install the HTML and JS sources.

Run `make installapi` to install the Swagger RESTful documentation.

Run `make installcgi` to install the CGI script and a fresh copy of the
database.  *Warning*: this will replace the existing database.

Run `make updatecgi` to install only the CGI script.

## License

All sources use the ISC (like OpenBSD) license.
See the [LICENSE.md](LICENSE.md) file for details.
