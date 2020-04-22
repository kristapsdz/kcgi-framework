#! /bin/sh

ORT="@SHAREDIR@/yourprog/yourprog.ort"

args=$(getopt f: $*)
if [ $? -ne 0 ]
then
	echo 'Usage: ...'
	exit 2
fi
set -- "$args"
while [ $# -ne 0 ]
do
	case "$1" in
	-f)
		ORT="$2"; shift; shift;;
	--)
		shift; break;;
        esac
done

if [ ! -f "@DATADIR@/yourprog.db" ]
then
	# If the database doesn't exist, obviously nothing's running.
	# Simply install it and exit.
	set -e
	mkdir -p "@DATADIR@"
	echo "@DATADIR@/yourprog.db: installing new"
	ort-sql "$ORT" | sqlite3 "@DATADIR@/yourprog.db"
	chown www "@DATADIR@/yourprog.db"
	chmod 600 "@DATADIR@/yourprog.db"
	install -m 0400 "$ORT" "@DATADIR@/yourprog.ort"
	exit 0
fi

cmp -s "$ORT" "@DATADIR@/yourprog.ort"
if [ $? -eq 0 ]
then
	echo "@DATADIR@/yourprog.db: already up to date"
	exit 0
fi

set -e
TMPFILE=$(mktemp) || exit 1
trap "rm -f $TMPFILE" 0

echo "@DATADIR@/yourprog.db: patching existing"

( echo "BEGIN EXCLUSIVE TRANSACTION;" ; \
  ort-sqldiff "@DATADIR@/yourprog.ort"  "$ORT" ; \
  echo "COMMIT TRANSACTION;" ; ) > "$TMPFILE"

if [ $? -ne 0 ]
then
	echo "@DATADIR@/yourprog.db: patch aborted" 1>&2
	exit 1
fi

sqlite3 "@DATADIR@/yourprog.db" < "$TMPFILE"
install -m 0400 "$ORT" "@DATADIR@/yourprog.ort"
rm -f "@DATADIR@/yourprog-upgrade.sql"
echo "@DATADIR@/yourprog.db: patch success"
