use 5.010001;
use ExtUtils::MakeMaker;
# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
WriteMakefile(
    NAME              => 'FiftyOneDegrees::TrieV3',
<<<<<<< HEAD
    VERSION_FROM      => 'lib/FiftyOneDegrees/TrieV3.pm', # finds $VERSION
    OBJECT            => 'src/trie/51Degrees.o src/51Degrees_wrap.o src/snprintf/snprintf.o',
    LDFROM          => '51Degrees.o 51Degrees_wrap.o snprintf.o',
    #C                 => ['trie/51Degrees.c', '51Degrees_wrap.c', 'snprintf/snprintf.c'],
=======
    VERSION      => '3.2.0.0',
    OBJECT            => '../../src/trie/51Degrees.o src/51Degrees_wrap.o',
    LDFROM          => '51Degrees.o 51Degrees_wrap.o',
    #C                 => ['../../src/trie/51Degrees.c', '51Degrees_wrap.c'],
>>>>>>> V32
    CC                 => "gcc",
    LD                 => "gcc",
    LIBS            => ['-lm'],
    PREREQ_PM         => {}, # e.g., Module::Name => 1.1
    ($] >= 5.005 ?     ## Add these new keywords supported since 5.005
<<<<<<< HEAD
      (ABSTRACT_FROM  => 'lib/FiftyOneDegrees/TrieV3.pm', # retrieve abstract from module
       AUTHOR         => 'bstokes@namimedia.com') : ()),
=======
      (ABSTRACT  => 'A wrapper for 51Degrees Device Detection Trie library for detecting devices from their useragent.',
       AUTHOR         => 'iftikhar@51degrees.com') : ()),
>>>>>>> V32
);


