# $ OpenBSD$

COMMENT =	your program description

DISTNAME =	yourprog-0.0.3

CATEGORIES =	misc

HOMEPAGE =	https://...

MAINTAINER =	you <youremail>

# ISC
PERMIT_PACKAGE =	Yes

# uses pledge()
WANTLIB += c sqlite3 z

BUILD_DEPENDS =	www/openradtool \
		www/kcgi \
		databases/sqlbox

RUN_DEPENDS =	www/openradtool

MASTER_SITES =	https://...

SUBST_VARS = VARBASE

CONFIGURE_STYLE = simple

NO_TEST =	Yes

.include <bsd.port.mk>
