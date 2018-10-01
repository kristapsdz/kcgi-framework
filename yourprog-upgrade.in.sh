#! /bin/sh

set -e

if [ ! -f "@DATADIR@/yourprog.db" ]
then
	mkdir -p "@DATADIR@"
	echo "@DATADIR@/yourprog.db: installing new"
	kwebapp-sql "@SHAREDIR@/yourprog/yourprog.kwbp" | sqlite3 "@DATADIR@/yourprog.db"
	chown www "@DATADIR@/yourprog.db"
	chmod 600 "@DATADIR@/yourprog.db"
	install -m 0444  "@SHAREDIR@/yourprog/yourprog.kwbp" "@DATADIR@/yourprog.kwbp"
	chmod 555 "@CGIBIN@/yourprog"
	exit 0
fi

TMPFILE=`mktemp` || exit 1
trap "rm -f $TMPFILE" ERR EXIT

echo "@DATADIR@/yourprog.db: patching existing"

( echo "BEGIN EXCLUSIVE TRANSACTION;" ; \
  kwebapp-sqldiff "@DATADIR@/yourprog.kwbp"  "@SHAREDIR@/yourprog/yourprog.kwbp" ; \
  echo "COMMIT TRANSACTION;" ; ) > $TMPFILE

if [ $? -ne 0 ]
then
	echo "@DATADIR@/yourprog.db: patch aborted" 1>&2
	exit 1
fi

sqlite3 "@DATADIR@/yourprog.db" < $TMPFILE
install -m 0444  "@SHAREDIR@/yourprog/yourprog.kwbp" "@DATADIR@/yourprog.kwbp"
chmod 555 "@CGIBIN@/yourprog"
rm -f "@DATADIR@/yourprog-upgrade.sql"
echo "@DATADIR@/yourprog.db: patch success"
