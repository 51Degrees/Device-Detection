AM_CPPFLAGS = @VMOD_INCLUDES@ -Wall -Werror

vmoddir = @VMOD_DIR@
vmod_LTLIBRARIES = libvmod_fiftyonedegrees.la

libvmod_fiftyonedegrees_la_LDFLAGS = -module -export-dynamic -avoid-version -shared

libvmod_fiftyonedegrees_la_SOURCES = \
	vmod_fiftyonedegrees.c \
	src/pattern/51Degrees.c \
	src/threading.c \
	src/cityhash/city.c

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
	@VARNISHTEST@ -Dvarnishd=@VARNISHD@ -Dvmod_topbuild=$(abs_top_builddir) $@

check: $(VMOD_TESTS)

EXTRA_DIST = \
	vmod_fiftyonedegrees.vcc \
	$(VMOD_TESTS)

CLEANFILES = \
	$(builddir)/vcc_if.c \
	$(builddir)/vcc_if.h \
	$(builddir)/vmod_fiftyonedegrees.rst \
	$(builddir)/vmod_fiftyonedegrees.man.rst
