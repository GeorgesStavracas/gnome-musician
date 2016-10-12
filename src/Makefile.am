CLEANFILES =
DISTCLEANFILES =
BUILT_SOURCES =
EXTRA_DIST =

noinst_LTLIBRARIES = libgnome-musician.la

libgnome_musician_la_SOURCES = \
	musician-gp4-parser.c \
	musician-gp4-parser.h \
	musician-gpt-input-stream.c \
	musician-gpt-input-stream.h \
	musician-gpt-parser.c \
	musician-gpt-parser.h \
	musician-gpt-song.c \
	musician-gpt-song.h \
	musician-gpt-song-private.h \
	$(NULL)

libgnome_musician_la_CFLAGS = \
	$(GNOME_MUSICIAN_CFLAGS) \
	$(NULL)

libgnome_musician_la_LIBADD = \
	$(GNOME_MUSICIAN_LIBS) \
	$(NULL)

nodist_libgnome_musician_la_SOURCES = \
	musician-enums.c \
	musician-enums.h \
	$(NULL)

glib_enum_h = musician-enums.h
glib_enum_c = musician-enums.c
glib_enum_headers = musician-gpt-types.h
include $(top_srcdir)/build-aux/Makefile.am.enums

-include $(top_srcdir)/git.mk
