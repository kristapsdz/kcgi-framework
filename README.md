## Synopsis

kcgi-framework is a set of files for starting a [BCHS](https://learnbchs.org)
project that uses [kcgi](https://kristaps.bsd.lv/kcgi) and
[openradtool](https://kristaps.bsd.lv/openradtool).
It assumes you're using the *latest available* of these tools, so if your
operating system's package manager lags, you may need to install from source.

The system, as is, minimally implements logging in and logging out over a
session-based, RESTful JSON API.  It has lots of documentation:

- initial database documentation via
  [sqliteconvert](https://kristaps.bsd.lv/sqliteconvert)
- RESTful documentation via [Swagger](https://swagger.io)
- well-documented C and JS source code

The existing code tries to follow best practises for all elements of the
stack: clean ([style(9)](https://man.openbsd.org/style.9)), secure C code
that's logically separated, HTML and JavaScript that are well-formed and
satisfy CSP policies, and so on.

It's also portable within UNIX systems as provided by
[oconfigure](https://github.com/kristapsdz/oconfigure).

You'll only use this repository once as a primer for your project.  It's
not something that's installed.  I use this to quickly get started on a
project without needing to re-implement basis user management.

## Installation

This is a framework (or foundation), so you'll use this to get your
project started.  Begin by copying all files into your project
directory.  You'll also need [kcgi](https://kristaps.bsd.lv/kcgi),
[openradtool](https://kristaps.bsd.lv/openradtool), and
[sqliteconvert](https://kristaps.bsd.lv/sqliteconvert) if you plan on
using the database documentation.

Begin by reading the [Makefile](Makefile): it will list all of the
variables you'll need to set for your installation.  Override these in a
*Makefile.local*.  The default values assume an OpenBSD system with a
stock install.

Then read the [main.c](main.c) and [yourprog.kwbp](yourprog.kwbp).
The former drives the output of the latter when pushed through
[openradtool](https://kristaps.bsd.lv/openradtool) in various ways.

Finally, read [index.xml](index.xml) and [index.js](index.js), both of
which drive the JSON backend.  It's all super-simple and self-contained.
([openradtool](https://kristaps.bsd.lv/openradtool) can also provide
this, but I erred on the side of simplicity.)

(You should probably use [TypeScript](https://www.typescriptlang.org/)
instead of JavaScript, but I'm keeping this simple.  Realistically that
only means installing the compiler, adding some TypeScript to JavaScript
in the Makefile, and that's it.)

Before running `make`, you'll need to run `./configure` to generate the
compatibility layer.

The current [Makefile](Makefile) rules are as follows.

Run `make` to compile the sources.

Run `make installwww` to install the HTML and JS sources.

Run `make installapi` to install the Swagger RESTful documentation.

Run `make installcgi` to install the CGI script and a fresh copy of the
database.  *Warning*: this will replace the existing database.

Run `make updatecgi` to install only the CGI script.

If you want to create a user to play with, use
[encrypt(1)](https://man.openbsd.org/encrypt.1) to generate a password
hash and insert those into the database prior to `make installcgi`.
Assuming a user `foo@bar.com` and substituting `yourpassword` for your
password and `yourhash` for the output of the encryption:

```sh
% make
% echo 'yourpassword' | encrypt
% sqlite3 yourprog.db
sqlite> insert into user values ('foo@bar.com', 'yourhash', 1);
sqlite> .quit
```

## Package management

Most of my CGI scripts are managed by a package manager, not by
hand-installation.  kcgi-framework includes the building blocks for an
OpenBSD package by including necessary files.

The port files exist in the [OpenBSD](openbsd) directory.  As-is, they
will install the CGI script in */var/www/cgi-bin* and the database and
database specification in */var/www/data*.

The CGI script is installed with no permission bits.  This is because at
initial installation, there's no database yet.  And on upgrade, the
database may not be in sync with the CGI script.

To solve this, the package manager will run the `yourprog-upgrade`
script, which generates the difference between the database
specification for the existing database in */var/www/data* with the
current version's specification.  It then patches the database and
installs the current specification.  This keeps your database smoothly
up to date.

Of course, this is something you'll need to carefully test!  It uses
[ort-sqldiff(1)](https://kristaps.bsd.lv/openradtool/ort-sqldiff.1.html),
which has its limitations.

## License

All sources use the ISC (like OpenBSD) license.
See the [LICENSE.md](LICENSE.md) file for details.
