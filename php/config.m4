PHP_ARG_ENABLE(fiftyone_degrees_detector, whether to enable 51Degrees.mobi Detector,
[ --enable-fiftyone_degrees_detector   Enable 51Degrees.mobi Device Detection])

if test "$PHP_FIFTYONE_DEGREES_DETECTOR" = "yes"; then
  AC_DEFINE(HAVE_FIFTYONE_DEGREES_DETECTOR, 1, [Whether you have 51Degrees.mobi Detector Enabled])
  PHP_SUBST(FIFTYONE_DEGREES_DETECTOR_LIBADD)
  PHP_NEW_EXTENSION(fiftyone_degrees_detector, fiftyone_degrees_v3_extension.c src/pattern/51Degrees.c src/snprintf/snprintf.c, $ext_shared,, -D HAVE_SNPRINTF)
fi
