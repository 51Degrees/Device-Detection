PHP_REQUIRE_CXX()
PHP_ADD_LIBRARY(stdc++, , FIFTYONEDEGREESPATTERNV3_LIBADD)

PHP_ARG_ENABLE(FiftyOneDegreesPatternV3, whether to enable 51Degrees Detector,
[ --enable-FiftyOneDegreesPatternV3   Enable 51Degrees Device Detection])

if test "$PHP_FIFTYONEDEGREESPATTERNV3" = "yes"; then
  AC_CONFIG_COMMANDS_PRE(mkdir src)
  AC_CONFIG_COMMANDS_PRE(mkdir src/pattern)
  AC_CONFIG_COMMANDS_PRE(mkdir src/cityhash)
  AC_CONFIG_COMMANDS_PRE(cp ../../src/pattern/* src/pattern)
  AC_CONFIG_COMMANDS_PRE(cp ../../src/threading.h src)
  AC_CONFIG_COMMANDS_PRE(cp ../../src/cityhash/city.* src/cityhash)
  AC_CONFIG_COMMANDS_PRE(swig2.0 -php -c++ -o src/pattern/51Degrees_PHP.cpp src/pattern/51Degrees.i)
  AC_CONFIG_COMMANDS_PRE(mkdir includes)
  AC_CONFIG_COMMANDS_PRE(cp src/pattern/FiftyOneDegreesPatternV3.php includes/)

  AC_DEFINE(HAVE_FIFTYONEDEGREESPATTERNV3, 1, [Whether you have 51Degrees Detector Enabled])
  PHP_SUBST(FIFTYONEDEGREESPATTERNV3_LIBADD)

  PHP_NEW_EXTENSION(FiftyOneDegreesPatternV3, src/cityhash/city.c src/pattern/51Degrees.c src/pattern/51Degrees_PHP.cpp src/pattern/Provider.cpp src/pattern/Match.cpp src/pattern/Profiles.cpp, $ext_shared, ,,"yes")
fi
