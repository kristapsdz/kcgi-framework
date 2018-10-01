# $ OpenBSD$

COMMENT =	your program description

DISTNAME =	yourprog-0.0.3

CATEGORIES =	misc

HOMEPAGE =	https://...

MAINTAINER =	you <youremail>

# ISC
PERMIT_PACKAGE_CDROM =	Yes

# uses pledge()
WANTLIB += c

BUILD_DEPENDS =	www/kwebapp \
		www/kcgi \
		databases/ksql

MASTER_SITES =	https://...

MAKE_ENV += 	CPPFLAGS="${CPPFLAGS} -I${LOCALBASE}/include" \
		LDFLAGS="${LDFLAGS} -L${LOCALBASE}/lib"

NO_TEST =	Yes

.include <bsd.port.mk>
