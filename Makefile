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

# Link options.
# If on a static architecture, STATIC is -static; otherwise empty.
# I use /usr/local for kcgi and sqlbox, hence using them here.
STATIC = -static

# Web-server relative location of DATADIR.
# See DATADIR.
RDDIR = /data

# URL location (filename) of CGI script.
CGIURI = /cgi-bin/yourprog

# If on an HTTPS-only installation, should be "-DSECURE".
SECURE = -DSECURE

# File-system location (directory) of Swagger API.
APIDOCS = /var/www/htdocs/api-docs

# Override these with an optional local file.
sinclude Makefile.local

OBJS		 = compats.o db.o json.o valids.o main.o
HTMLS		 = index.html
JSMINS		 = index.min.js
CPPFLAGS	+= -DDATADIR=\"$(RDDIR)\"
VERSION		 = 0.0.4

all: yourprog yourprog.db yourprog-upgrade $(HTMLS) $(JSMINS)

api: swagger.json schema.png schema.html

yourprog-upgrade: yourprog-upgrade.in.sh
	sed -e "s!@DATADIR@!$(DATADIR)!g" \
	    -e "s!@CGIBIN@!$(CGIBIN)!g" \
	    -e "s!@SHAREDIR@!$(SHAREDIR)!g" yourprog-upgrade.in.sh >$@

install: all
	mkdir -p $(DESTDIR)$(SHAREDIR)/yourprog
	mkdir -p $(DESTDIR)$(SBINDIR)
	mkdir -p $(DESTDIR)$(CGIBIN)
	mkdir -p $(DESTDIR)$(HTDOCS)
	$(INSTALL_DATA) $(HTMLS) $(JSMINS) $(DESTDIR)$(HTDOCS)
	$(INSTALL_DATA) yourprog.kwbp $(DESTDIR)$(SHAREDIR)/yourprog
	$(INSTALL_PROGRAM) yourprog $(DESTDIR)$(CGIBIN)
	$(INSTALL_PROGRAM) yourprog-upgrade $(DESTDIR)$(SBINDIR)

uninstall:
	rm -f $(DESTDIR)$(SHAREDIR)/yourprog/yourprog.kwbp
	rmdir $(DESTDIR)$(SHAREDIR)/yourprog
	rm -f $(DESTDIR)$(CGIBIN)/yourprog
	rm -f $(DESTDIR)$(SBINDIR)/yourprog-upgrade

installwww: all
	mkdir -p $(HTDOCS)
	$(INSTALL_DATA) $(HTMLS) $(JSMINS) $(HTDOCS)

installapi: api
	mkdir -p $(APIDOCS)
	$(INSTALL_DATA) schema.html schema.png swagger.json $(APIDOCS)

updatecgi: all
	mkdir -p $(CGIBIN)
	$(INSTALL_PROGRAM) yourprog $(CGIBIN)

installcgi: updatecgi
	mkdir -p $(DATADIR)
	rm -f $(DATADIR)/yourprog.db
	install -m 666 yourprog.db $(DATADIR)
	chmod 0777 $(DATADIR)

clean:
	rm -f yourprog yourprog-upgrade $(HTMLS) $(JSMINS) $(OBJS) yourprog.db
	rm -f swagger.json schema.html schema.png 
	rm -f db.c json.c valids.c extern.h yourprog.sql

distclean: clean
	rm -f config.log config.h Makefile.configure

schema.html: yourprog.sql
	sqliteconvert yourprog.sql >$@

schema.png: yourprog.sql
	sqliteconvert -i yourprog.sql >$@

db.c: yourprog.kwbp
	ort-c-source -Idvj -h extern.h yourprog.kwbp >$@

json.c: yourprog.kwbp
	ort-c-source -Idvj -j -Nd -h extern.h yourprog.kwbp >$@

valids.c: yourprog.kwbp
	ort-c-source -Idvj -v -Nd -h extern.h yourprog.kwbp >$@

extern.h: yourprog.kwbp
	ort-c-header -jv yourprog.kwbp >$@

yourprog.sql: yourprog.kwbp
	ort-sql yourprog.kwbp >$@

.sql.db:
	@rm -f $@
	sqlite3 $@ < $<

.js.min.js .xml.html:
	sed -e "s!@HTURI@!$(HTURI)!g" \
	    -e "s!@VERSION@!$(VERSION)!g" \
	    -e "s!@CGIURI@!$(CGIURI)!g" $< >$@

yourprog: $(OBJS)
	$(CC) $(STATIC) -o $@ $(OBJS) $(LDFLAGS) -lkcgi -lkcgijson -lz -lsqlbox -lsqlite3 -lm -lpthread

$(OBJS): extern.h

swagger.json: swagger.in.json
	@rm -f $@
	sed -e "s!@VERSION@!$(VERSION)!g" swagger.in.json >$@
	@chmod 400 $@

