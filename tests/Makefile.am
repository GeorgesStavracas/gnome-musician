check_PROGRAMS =

# GPT Parser
check_PROGRAMS += test-gpt-parser

test_gpt_parser_SOURCES = test-gpt-parser.c

test_gpt_parser_CFLAGS = \
	$(GNOME_MUSICIAN_CFLAGS) \
	-DTESTS_SRCDIR=\""$(abs_srcdir)"\" \
	-I$(top_srcdir)/src \
	-I$(top_builddir)/src \
	$(NULL)

test_gpt_parser_LDADD = \
	$(GNOME_MUSICIAN_LIBS) \
	$(top_builddir)/src/libgnome-musician.la \
	$(NULL)

TESTS = $(check_PROGRAMS)

-include $(top_srcdir)/git.mk
