use 5.010001;
use ExtUtils::MakeMaker;

# Name of the Perl module generated.
$moduleName = "FiftyOneDegrees::PatternV3";

# Call SWIG to build the Perl source files.
system("swig", "-perl", "-c++", "-module", $moduleName, "-outdir", "lib/FiftyOneDegrees", "-o", "../../src/pattern/51Degrees_perl.cxx", "../../src/pattern/51Degrees.i");

# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
WriteMakefile(
    NAME			=> $moduleName,
    VERSION			=> '3.2.16.5',
    LDFROM			=> '51Degrees.o threading.o city.o Match.o Provider.o Profiles.o 51Degrees_perl.o',
    OBJECT			=> '../../src/pattern/51Degrees.o ../../src/threading.o ../../src/cityhash/city.o ../../src/pattern/Match.o ../../src/pattern/Provider.o ../../src/pattern/Profiles.o ../../src/pattern/51Degrees_perl.o',
    INC				=> '-I../../src -I../../src/pattern',
    CC				=> "g++",
    LD				=> "g++",
    LIBS			=> ['-lm'],
    DEFINE 			=> "-DHTTP_HEADERS_PREFIXED",
    # OPTIMIZE		=> '-g', - Switch with O2 for debug compile
    OPTIMIZE		=> '-O2',
    PREREQ_PM		=> {JSON => 2.53 },
    ($] >= 5.005 ?	## Add these new keywords supported since 5.005
      (ABSTRACT		=> 'A wrapper for 51Degrees Device Detection Pattern library for detecting devices from their User-Agent.',
       AUTHOR		=> ['support@51degrees.com']) : ()),
);


