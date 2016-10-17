.SUFFIXES: .html .in.xml .xml .js .min.js .db .sql

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

# Compilation and link options.
# If on a static architecture, STATIC is -static; otherwise empty.
# I use /usr/local for kcgi and ksql, hence using them here.
STATIC = -static
CFLAGS += -I/usr/local/include
LDFLAGS += -L/usr/local/lib

# Web-server relative location of DATADIR.
# See DATADIR.
RDDIR = /data

# Name of installed CGI script, since some servers like to have ".cgi"
# appended to everything.
# See TARGET.
CGINAME = yourprog

# This is the name of the binary we're going to build.
# It differs from CGINAME in that it's the install source.
TARGET = yourprog

DATABASE = yourprog.db

# URL location (filename) of CGI script.
# See CGIBIN and CGINAME.
CGIURI = /cgi-bin/$(CGINAME)

# If on an HTTPS-only installation, should be "-DSECURE".
SECURE = -DSECURE

# File-system location (directory) of Swagger API.
APIDOCS = /var/www/htdocs/api-docs

# Override these with an optional local file.
sinclude GNUmakefile.local

OBJS	 = main.o
HTMLS	 = index.html
JSMINS	 = index.min.js
CFLAGS	+= -g -W -Wall -O2 $(SECURE)
CFLAGS	+= -DLOGFILE=\"$(LOGFILE)\"
CFLAGS	+= -DDATADIR=\"$(RDDIR)\"
CFLAGS	+= -DDATABASE=\"$(DATABASE)\"
VERSION	 = 0.0.1

all: $(TARGET) $(TARGET).db $(HTMLS) $(JSMINS)

api: swagger.json schema.png schema.html

installwww: all
	mkdir -p $(HTDOCS)
	install -m 0444 $(HTMLS) $(JSMINS) $(HTDOCS)

installapi: api
	mkdir -p $(APIDOCS)
	install -m 0444 schema.html schema.png swagger.json $(APIDOCS)

updatecgi: all
	mkdir -p $(CGIBIN)
	install -m 0555 $(TARGET) $(CGIBIN)/$(CGINAME)

installcgi: updatecgi
	mkdir -p $(DATADIR)
	rm -f $(DATADIR)/$(TARGET).db
	rm -f $(DATADIR)/$(TARGET).db-wal
	rm -f $(DATADIR)/$(TARGET).db-shm
	install -m 0666 $(TARGET).db $(DATADIR)
	chmod 0777 $(DATADIR)

clean:
	rm -f $(TARGET) $(HTMLS) $(JSMINS) $(OBJS) $(TARGET).db
	rm -f swagger.json schema.html schema.png
	rm -rf $(TARGET).dSYM

.sql.html:
	sqliteconvert $< >$@

.sql.png:
	sqliteconvert -i $< >$@

.sql.db:
	@rm -f $@
	sqlite3 $@ < $<

.js.min.js .xml.html:
	sed -e "s!@HTURI@!$(HTURI)!g" \
	    -e "s!@VERSION@!$(VERSION)!g" \
	    -e "s!@CGIURI@!$(CGIURI)!g" $< >$@

$(TARGET): $(OBJS)
	$(CC) $(STATIC) -o $@ $(OBJS) $(LDFLAGS) -lkcgi -lkcgijson -lz -lksql -lsqlite3

swagger.json: swagger.in.json
	@rm -f $@
	sed -e "s!@VERSION@!$(VERSION)!g" swagger.in.json >$@
	@chmod 400 $@

