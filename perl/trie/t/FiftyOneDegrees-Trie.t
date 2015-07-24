# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl FiftyOneDegrees-Trie.t'

#########################

use Test::More tests => 1;
use JSON;
use feature qw/say/;

BEGIN { use_ok('FiftyOneDegrees::Trie') };

#########################

# Insert your test code below, the Test::More module is use()ed here so read
# its man page ( perldoc Test::More ) for help writing this test script.

say "Please be sure you have a valid Trie Lite or Premium data file.";
say "Contact 51Degrees.mobi for details."; 
