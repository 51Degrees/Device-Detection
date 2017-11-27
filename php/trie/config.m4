PHP_REQUIRE_CXX()
PHP_ADD_LIBRARY(stdc++, , FIFTYONEDEGREESTRIEV3_LIBADD)

PHP_ARG_ENABLE(FiftyOneDegreesTrieV3, whether to enable 51Degrees Detector,
[ --enable-FiftyOneDegreesTrieV3   Enable 51Degrees Device Detection])

if test "$PHP_FIFTYONEDEGREESTRIEV3" = "yes"; then
  AC_DEFINE(HAVE_FIFTYONEDEGREESTRIEV3, 1, [Whether you have 51Degrees Detector Enabled])
  CFLAGS="$CFLAGS -lrt"
  PHP_SUBST([CFLAGS])
  PHP_SUBST(FIFTYONEDEGREESTRIEV3_LIBADD)

  if test -n "$PHP7"; then
    WRAP_SOURCE="php7"
  else
    WRAP_SOURCE="php5"
  fi
  
  if test -n "$SWIG"; then
    SWIG_COMMAND="swig -c++ -$WRAP_SOURCE -outdir src/$WRAP_SOURCE -o src/$WRAP_SOURCE/51Degrees_PHP.cpp src/trie/51Degrees.i"
  else
    SWIG_COMMAND=""
  fi

  AC_CONFIG_COMMANDS_PRE(mkdir src)
  AC_CONFIG_COMMANDS_PRE(mkdir src/$WRAP_SOURCE)
  AC_CONFIG_COMMANDS_PRE(mkdir src/trie)
  AC_CONFIG_COMMANDS_PRE(mkdir includes)
  AC_CONFIG_COMMANDS_PRE(cp ../../src/trie/* src/trie)
  AC_CONFIG_COMMANDS_PRE(cp ../../src/threading.* src)

  AC_CONFIG_COMMANDS_PRE($SWIG_COMMAND)

  AC_CONFIG_COMMANDS_PRE(cp src/$WRAP_SOURCE/* src/trie/)

  PHP_NEW_EXTENSION(FiftyOneDegreesTrieV3, src/threading.c src/trie/51Degrees.c src/trie/51Degrees_PHP.cpp src/trie/Provider.cpp src/trie/Match.cpp, $ext_shared, ,,"yes")
fi
