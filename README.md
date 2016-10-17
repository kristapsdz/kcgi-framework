## Synopsis

kcgi-framework is a set of files for starting a
[BCHS](https://learnbchs.org) project that uses
[kcgi](https://kristaps.bsd.lv/kcgi) and
[ksql](https://kristaps.bsd.lv/ksql).

The system, as is, minimally implements logging in and logging out over a
session-based, RESTful JSON API.  It has the following features:

- initial database documentation via
  [sqliteconvert](https://kristaps.bsd.lv/sqliteconvert)
- RESTful documentation via [Swagger](https://swagger.io)
- well-documented C and JS source code

You'll only use this repository once as a primer for your project.  It's
not something that's installed.  I use this to quickly get started on a
project without needing to re-implement basis user management.

## Installation

This is a framework (or foundation), so you'll use this to get your
project started.  Begin by copying all files into your project
directory.  You'll need GNU make if you're on a BSD system.  You'll also
need [kcgi](https://kristaps.bsd.lv/kcgi) and
[ksql](https://kristaps.bsd.lv/ksql), and
[sqliteconvert](https://kristaps.bsd.lv/sqliteconvert) if you plan on
using the database documentation.

Begin by reading the [GNUmakefile](GNUmakefile): it will list all of the
variables you'll need to set for your installation.  For running and
testing on many systems, I set the defaults and override these values
with a local GNUmakefile.local (source during compilation) on
non-production systems.

Then read the [main.c](main.c) file, which interacts with the database
and orchestrates JSON responses.

Finally, read [index.xml](index.xml) and [index.js](index.js), both of
which drive the JSON backend.  It's all super-simple and self-contained.

Run `make` to compile the sources.

Run `make installwww` to install the HTML and JS sources.

Run `make installapi` to install the Swagger RESTful documentation.

Run `make installcgi` to install the CGI script and a fresh copy of the
database.  *Warning*: this will replace the existing database.

Run `make updatecgi` to install only the CGI script.

## License

All sources use the ISC (like OpenBSD) license.
See the [LICENSE.md](LICENSE.md) file for details.
