noinst_PROGRAMS = test-gpt-parser

test_gpt_parser_SOURCES = test-gpt-parser.c
test_gpt_parser_CFLAGS = \
	$(GNOME_MUSICIAN_CFLAGS) \
	-DTESTS_SRCDIR=\""$(srcdir)"\" \
	-I$(top_srcdir)/src
test_gpt_parser_LDADD = \
	$(GNOME_MUSICIAN_LIBS) \
	$(top_builddir)/src/libgnome-musician.la

-include $(top_srcdir)/git.mk
