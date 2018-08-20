AM_CPPFLAGS = -Wall -Werror

vmoddir = @VMOD_DIR@
vmod_LTLIBRARIES = libvmod_fiftyonedegrees.la

libvmod_fiftyonedegrees_la_LDFLAGS = -module -export-dynamic -avoid-version -shared

if FIFTYONEDEGREES_PATTERN
libvmod_fiftyonedegrees_la_SOURCES = \
	vmod_fiftyonedegrees.c \
	vmod_fiftyonedegrees_pattern.c \
	src/pattern/51Degrees.c \
	src/threading.c \
	src/cityhash/city.c
AM_CPPFLAGS += -DFIFTYONEDEGREES_PATTERN
datafile = $(abs_top_builddir)/data/51Degrees-LiteV3.2.dat
endif
if FIFTYONEDEGREES_HASH
libvmod_fiftyonedegrees_la_SOURCES = \
	vmod_fiftyonedegrees.c \
	vmod_fiftyonedegrees_hash.c \
	src/trie/51Degrees.c \
	src/threading.c
AM_CPPFLAGS += -DFIFTYONEDEGREES_HASH
datafile = $(abs_top_builddir)/data/51Degrees-LiteV3.4.trie
endif

nodist_libvmod_fiftyonedegrees_la_SOURCES = \
	vcc_if.c \
	vcc_if.h

vmod_fiftyonedegrees.lo: vcc_if.c vcc_if.h

vcc_if.c: vcc_if.h

vcc_if.h: @VMODTOOL@ $(top_srcdir)/src/vmod_fiftyonedegrees.vcc
	@VMODTOOL@ $(top_srcdir)/src/vmod_fiftyonedegrees.vcc

VMOD_TESTS = $(top_srcdir)/src/tests/*.vtc
.PHONY: $(VMOD_TESTS)

$(top_srcdir)/src/tests/*.vtc: libvmod_fiftyonedegrees.la
	@VARNISHTEST@ -Dvarnishd=@VARNISHD@ -Dvmod_fod_datafile=$(datafile) -Dvmod_topbuild=$(abs_top_builddir) $@

check: $(VMOD_TESTS)

EXTRA_DIST = \
	vmod_fiftyonedegrees.vcc \
	$(VMOD_TESTS)

CLEANFILES = \
	$(builddir)/vcc_if.c \
	$(builddir)/vcc_if.h \
	$(builddir)/vmod_fiftyonedegrees.rst \
	$(builddir)/vmod_fiftyonedegrees.man.rst
