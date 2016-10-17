## Synopsis

kcgi-framework is a set of files for starting a
[BCHS](https://learnbchs.org) project that uses
[kcgi](https://kristaps.bsd.lv/kcgi) and
[ksql](https://kristaps.bsd.lv/ksql).

The system, as is, supports only logging in and logging out over a
session-based, RESTful JSON API.  It has the following features:

- secure HTTP headers
- initial database documentation via
  [sqliteconvert](https://kristaps.bsd.lv/sqliteconvert)
- RESTful documentation via [Swagger](https://swagger.io)

## Installation

This is a framework (or foundation), so you'll use this to get your
project started.

Begin by reading the [GNUmakefile](GNUmakefile): it will list all of the
variables you'll need to set for your installation.  For running and
testing on many systems, I usually override these values with a local
GNUmakefile.local, which is sourced during compilation.

Then read the [main.c](main.c) file, which interacts with the database
and orchestrates JSON responses.

Finally, read [index.xml](index.xml) and [index.js](index.js), both of
which drive the JSON backend.

## License

All sources use the ISC (like OpenBSD) license.
See the [LICENSE.md](LICENSE.md) file for details.
