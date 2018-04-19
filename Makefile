.SUFFIXES: .html .in.xml .xml .js .min.js .db .sql .png
.PHONY: clean distclean

include Makefile.configure

# The default installation is for a default-install OpenBSD box that's
# running HTTPS (only).

# File-system location (directory) of static media.
# See HTURI for the web-visible component.
HTDOCS = /var/www/htdocs

# URL location (path) of static media.
# See HTDOCS.
HTURI = 

# File-system location (directory) of CGI script.
# See CGIURI.
CGIBIN = /var/www/cgi-bin

# File-system location of database.
# See RDDIR.
DATADIR = /var/www/data

# Web-server relative location of system log file.
# This will have all logging messages by the system.
LOGFILE = /logs/system.log
LOGFFILE = /var/www/logs/system.log

# Link options.
# If on a static architecture, STATIC is -static; otherwise empty.
# I use /usr/local for kcgi and ksql, hence using them here.
STATIC = -static

# Web-server relative location of DATADIR.
# See DATADIR.
RDDIR = /data

# URL location (filename) of CGI script.
CGIURI = /cgi-bin/yourprog

# This is the name of the binary we're going to build.
TARGET = yourprog

# If on an HTTPS-only installation, should be "-DSECURE".
SECURE = -DSECURE

# File-system location (directory) of Swagger API.
APIDOCS = /var/www/htdocs/api-docs

# Override these with an optional local file.
sinclude Makefile.local

OBJS		 = compats.o db.o json.o valids.o main.o
HTMLS		 = index.html
JSMINS		 = index.min.js
CPPFLAGS	+= -DLOGFILE=\"$(LOGFILE)\"
CPPFLAGS	+= -DDATADIR=\"$(RDDIR)\"
VERSION		 = 0.0.2

all: yourprog yourprog.db $(HTMLS) $(JSMINS)

api: swagger.json schema.png schema.html

installwww: all
	mkdir -p $(HTDOCS)
	install -m 0444 $(HTMLS) $(JSMINS) $(HTDOCS)

installapi: api
	mkdir -p $(APIDOCS)
	install -m 0444 schema.html schema.png swagger.json $(APIDOCS)

updatecgi: all
	mkdir -p $(CGIBIN)
	$(INSTALL_PROGRAM) yourprog $(CGIBIN)

installcgi: updatecgi
	mkdir -p $(DATADIR)
	rm -f $(DATADIR)/yourprog.db
	install -m 666 yourprog.db $(DATADIR)
	chmod 0777 $(DATADIR)

clean:
	rm -f yourprog $(HTMLS) $(JSMINS) $(OBJS) yourprog.db
	rm -f swagger.json schema.html schema.png 
	rm -f db.c json.c valids.c extern.h yourprog.sql

distclean: clean
	rm -f config.log config.h Makefile.configure

schema.html: yourprog.sql
	sqliteconvert yourprog.sql >$@

schema.png: yourprog.sql
	sqliteconvert -i yourprog.sql >$@

db.c: yourprog.kwbp
	kwebapp-c-source -Ibvj -s -h extern.h yourprog.kwbp >$@

json.c: yourprog.kwbp
	kwebapp-c-source -Ibvj -j -Nb -h extern.h yourprog.kwbp >$@

valids.c: yourprog.kwbp
	kwebapp-c-source -Ibvj -v -Nb -h extern.h yourprog.kwbp >$@

extern.h: yourprog.kwbp
	kwebapp-c-header -jsv yourprog.kwbp >$@

yourprog.sql: yourprog.kwbp
	kwebapp-sql yourprog.kwbp >$@

.sql.db:
	@rm -f $@
	sqlite3 $@ < $<

.js.min.js .xml.html:
	sed -e "s!@HTURI@!$(HTURI)!g" \
	    -e "s!@VERSION@!$(VERSION)!g" \
	    -e "s!@CGIURI@!$(CGIURI)!g" $< >$@

yourprog: $(OBJS)
	$(CC) $(STATIC) -o $@ $(OBJS) $(LDFLAGS) -lkcgi -lkcgijson -lz -lksql -lsqlite3

$(OBJS): extern.h

swagger.json: swagger.in.json
	@rm -f $@
	sed -e "s!@VERSION@!$(VERSION)!g" swagger.in.json >$@
	@chmod 400 $@

