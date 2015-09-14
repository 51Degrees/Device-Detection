use 5.010001;
use ExtUtils::MakeMaker;
# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
WriteMakefile(
    NAME              => 'FiftyOneDegrees::TrieV3',
    VERSION      => '3.2.0.0',
    OBJECT            => '../../src/trie/51Degrees.o src/51Degrees_wrap.o',
    LDFROM          => '51Degrees.o 51Degrees_wrap.o',
    #C                 => ['../../src/trie/51Degrees.c', '51Degrees_wrap.c'],
    CC                 => "gcc",
    LD                 => "gcc",
    LIBS            => ['-lm'],
    PREREQ_PM         => {}, # e.g., Module::Name => 1.1
    ($] >= 5.005 ?     ## Add these new keywords supported since 5.005
      (ABSTRACT  => 'A wrapper for 51Degrees Device Detection Trie library for detecting devices from their useragent.',
       AUTHOR         => 'iftikhar@51degrees.com') : ()),
);


