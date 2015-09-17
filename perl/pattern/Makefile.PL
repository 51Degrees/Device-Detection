use 5.010001;
use ExtUtils::MakeMaker;
# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
WriteMakefile(
    NAME              => 'FiftyOneDegrees::PatternV3',
<<<<<<< HEAD
    VERSION      => '1.0.1.4',
    OBJECT            => 'src/pattern/51Degrees.o src/51Degrees_wrap.o src/snprintf/snprintf.o',
    LDFROM          => '51Degrees.o 51Degrees_wrap.o snprintf.o',
    #C                 => ['pattern/51Degrees.c', '51Degrees_wrap.c', 'snprintf/snprintf.c'],
    CC                 => "gcc",
    LD                 => "gcc",
    LIBS            => ['-lm'],
    PREREQ_PM         => {JSON => 2.90 },
    ($] >= 5.005 ?     ## Add these new keywords supported since 5.005
      (ABSTRACT  => 'A wrapper for 51Degrees Device Detection Pattern library for detecting devices from their useragent.',
       AUTHOR         => ['tom@51degrees.com', 'bstokes@namimedia.com']) : ()),
=======
    VERSION      => '3.2.0.0',
    OBJECT            => '../../src/pattern/51Degrees.o src/51Degrees_wrap.o ../../src/cityhash/city.o',
    LDFROM          => '51Degrees.o 51Degrees_wrap.o city.o',
    #C                 => ['../../src/pattern/51Degrees.c', 'src/51Degrees_wrap.c', '../../src/cityhash/city.c'],
    CC                 => "gcc",
    LD                 => "gcc",
    LIBS            => ['-lm'],
    PREREQ_PM         => {JSON => 2.53 },
    ($] >= 5.005 ?     ## Add these new keywords supported since 5.005
      (ABSTRACT  => 'A wrapper for 51Degrees Device Detection Pattern library for detecting devices from their useragent.',
       AUTHOR         => ['iftikhar@51degrees.com']) : ()),
>>>>>>> V32
);


