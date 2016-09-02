# searching ppc directory as a fallback to avoid unnecessary code duplication
.PATH: ${.CURDIR}/ppc/gen

MDSRCS +=	\
     abs.s	\
     ecvt.c	\
     ffs.s	\
     fp.h	\
     icacheinval.s \
     mcount.s	\
     setjmperr.c

SUPPRESSSRCS += memcpy.c memmove.c memset.c