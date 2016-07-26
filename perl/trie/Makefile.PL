use strict;
use warnings;

use FindBin;
use File::Spec;
use lib File::Spec->catdir($FindBin::Bin, qw(.. lib));
use FiftyOneDegrees::MakeMakerConfig;

my $maker = FiftyOneDegrees::MakeMakerConfig->new(
    name    => 'FiftyOneDegrees::TrieV3',
    type    => 'trie',
    version => '3.2.6.4',
);

CORE::say $maker->type;

$maker->prepare(
    ldfrom  => '51Degrees.o Match.o Provider.o 51Degrees_perl.o threading.o'
);
