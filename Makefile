.SUFFIXES: .html .in.xml .xml .js .min.js .db .sql .png
.PHONY: clean distclean

include Makefile.configure

# Path to static media.
# HTDOCS is the file-system path; HTURI is the URL path.

HTDOCS = /var/www/htdocs
HTURI = 

# Path to the CGI script.
# CGIBIN is the file-system path; HTURI is the URL path.

CGIBIN = /var/www/cgi-bin
CGIURI = /cgi-bin/yourprog

# Path to the database
# DATADIR is the file-system path; RDDIR is in the chroot.
# If there's no chroot, these are the same.

DATADIR = /var/www/data
RDDIR = /data

# Uncomment this to disable -static linking.
# This is set to -static by configure for supporting systems.

#LDADD_STATIC =

# If on an HTTPS-only installation, should be "-DSECURE".

SECURE = -DSECURE

# File-system path where Swagger API is installed.

APIDOCS = /var/www/htdocs/api-docs

# Override these with an optional local file.

sinclude Makefile.local

OBJS		 = compats.o db.o json.o valids.o main.o
HTMLS		 = index.html
JSMINS		 = index.min.js
DEPS_PKG	 = sqlbox kcgi-json
STATIC_PKG	!= [ -z "$(LDADD_STATIC)" ] || echo "--static"
CFLAGS_PKG	!= pkg-config --cflags $(DEPS_PKG)
LDADD_PKG	!= pkg-config --libs $(STATIC_PKG) $(DEPS_PKG)

CFLAGS		+= $(CFLAGS_PKG)
LDADD		+= $(LDADD_PKG) $(LDADD_CRYPT)
CFLAGS		+= -DDATADIR=\"$(RDDIR)\"
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
	$(CC) $(LDADD_STATIC) -o $@ $(OBJS) $(LDFLAGS) $(LDADD)

$(OBJS): extern.h

swagger.json: swagger.in.json
	@rm -f $@
	sed -e "s!@VERSION@!$(VERSION)!g" swagger.in.json >$@
	@chmod 400 $@

