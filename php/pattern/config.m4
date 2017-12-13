PHP_REQUIRE_CXX()
PHP_ADD_LIBRARY(stdc++, , FIFTYONEDEGREESPATTERNV3_LIBADD)

PHP_ARG_ENABLE(FiftyOneDegreesPatternV3, whether to enable 51Degrees Detector,
[ --enable-FiftyOneDegreesPatternV3   Enable 51Degrees Device Detection])

if test "$PHP_FIFTYONEDEGREESPATTERNV3" = "yes"; then
  AC_DEFINE(HAVE_FIFTYONEDEGREESPATTERNV3, 1, [Whether you have 51Degrees Detector Enabled])
  CFLAGS="$CFLAGS -lrt"
  PHP_SUBST([CFLAGS])
  PHP_SUBST(FIFTYONEDEGREESPATTERNV3_LIBADD)

  if test -n "$PHP7"; then
    WRAP_SOURCE="php7"
  else
    WRAP_SOURCE="php5"
  fi
  
  if test -n "$SWIG"; then
    SWIG_COMMAND="swig -c++ -$WRAP_SOURCE -outdir src/$WRAP_SOURCE -o src/$WRAP_SOURCE/51Degrees_PHP.cpp src/pattern/51Degrees.i"
  else
    SWIG_COMMAND=""
  fi

  AC_CONFIG_COMMANDS_PRE(mkdir src)
  AC_CONFIG_COMMANDS_PRE(mkdir src/$WRAP_SOURCE)
  AC_CONFIG_COMMANDS_PRE(mkdir src/pattern)
  AC_CONFIG_COMMANDS_PRE(mkdir src/cityhash)
  AC_CONFIG_COMMANDS_PRE(mkdir includes)
  AC_CONFIG_COMMANDS_PRE(cp ../../src/pattern/* src/pattern)
  AC_CONFIG_COMMANDS_PRE(cp ../../src/threading.* src)
  AC_CONFIG_COMMANDS_PRE(cp ../../src/cityhash/city.* src/cityhash)

  AC_CONFIG_COMMANDS_PRE($SWIG_COMMAND)

  AC_CONFIG_COMMANDS_PRE(cp src/$WRAP_SOURCE/* src/pattern/)

  PHP_NEW_EXTENSION(FiftyOneDegreesPatternV3, src/cityhash/city.c src/threading.c src/pattern/51Degrees.c src/pattern/51Degrees_PHP.cpp src/pattern/Provider.cpp src/pattern/Match.cpp src/pattern/Profiles.cpp, $ext_shared, ,,"yes")
fi
