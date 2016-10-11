noinst_LTLIBRARIES = libgnome-musician.la

libgnome_musician_la_SOURCES = \
	musician-gpt-input-stream.c \
	musician-gpt-input-stream.h \
	musician-gpt-parser.c \
	musician-gpt-parser.h \
	$(NULL)

libgnome_musician_la_CFLAGS = \
	$(GNOME_MUSICIAN_CFLAGS) \
	$(NULL)

libgnome_musician_la_LIBADD = \
	$(GNOME_MUSICIAN_LIBS) \
	$(NULL)

-include $(top_srcdir)/git.mk
