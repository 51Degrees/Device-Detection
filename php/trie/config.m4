PHP_REQUIRE_CXX()
PHP_ADD_LIBRARY(stdc++, , FIFTYONEDEGREESTRIEV3_LIBADD)

PHP_ARG_ENABLE(FiftyOneDegreesTrieV3, whether to enable 51Degrees Detector,
[ --enable-FiftyOneDegreesTrieV3   Enable 51Degrees Device Detection])

if test "$PHP_FIFTYONEDEGREESTRIEV3" = "yes"; then
  AC_CONFIG_COMMANDS_PRE(mkdir src)
  AC_CONFIG_COMMANDS_PRE(mkdir src/trie)
  AC_CONFIG_COMMANDS_PRE(mkdir src/cityhash)
  AC_CONFIG_COMMANDS_PRE(cp ../../src/trie/* src/trie)
  AC_CONFIG_COMMANDS_PRE(cp ../../src/threading.* src)
  AC_CONFIG_COMMANDS_PRE(cp ../../src/cityhash/city.* src/cityhash)
  AC_CONFIG_COMMANDS_PRE(swig -php -c++ -o src/trie/51Degrees_PHP.cpp src/trie/51Degrees.i)
  AC_CONFIG_COMMANDS_PRE(mkdir includes)
  AC_CONFIG_COMMANDS_PRE(cp src/trie/FiftyOneDegreesTrieV3.php includes/)

  AC_DEFINE(HAVE_FIFTYONEDEGREESTRIEV3, 1, [Whether you have 51Degrees Detector Enabled])
  PHP_SUBST(FIFTYONEDEGREESTRIEV3_LIBADD)

  PHP_NEW_EXTENSION(FiftyOneDegreesTrieV3, src/cityhash/city.c src/threading.c src/trie/51Degrees.c src/trie/51Degrees_PHP.cpp src/trie/Provider.cpp src/trie/Match.cpp, $ext_shared, ,,"yes")
fi
