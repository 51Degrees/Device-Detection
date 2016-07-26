use strict;
use warnings;

use FindBin;
use File::Spec;
use lib File::Spec->catdir($FindBin::Bin, qw(.. lib));
use FiftyOneDegrees::MakeMakerConfig;

my $maker = FiftyOneDegrees::MakeMakerConfig->new(
    name    => 'FiftyOneDegrees::PatternV3',
    type    => 'pattern',
    version => '3.2.6.4',
    ldfrom  => '51Degrees.o Match.o Provider.o Profiles.o 51Degrees_perl.o city.o threading.o'
);

$maker->prepare(
    PREREQ_PM => {
        JSON => '2.53'
    },
);
