use FiftyOneDegrees::PatternV3;
use JSON;
use Data::Dumper;
use feature qw/say/;
use Getopt::Long;

my $filename;
my $propertyList;
my $userAgentString;

my $r = GetOptions('filename|f=s' => \$filename,
                   'properties|p=s' => \$propertyList,
                   'ua=s' => \$userAgentString);

$userAgentString //= "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.114 Safari/537.36";

$propertyList //= "Id,BrowserName,BrowserVendor,BrowserVersion,CookiesCapable,IsTablet,IsMobile,IsCrawler";

die "DAT file does not exist! usage: $0 -f <dat filename>\n" unless -e ("$filename");

# Initialize Pattern library

# For pattern a dataset pointer is needed. This will be passd into other
# PatternV3 functions.
eval {

	my $dsPointer = FiftyOneDegrees::PatternV3::dataSetInitWithPropertyString($filename, $propertyList);
	my $json = FiftyOneDegrees::PatternV3::getMatch($dsPointer, $userAgentString);
	
	say "=== Data Output ===";
	my %properties = %{ decode_json($json) };
	# Loop through and print all returned properties
	while (my ($key, $value) = each %properties) {
		say $key . ": " . $value;
	}

	FiftyOneDegrees::PatternV3::destroyDataset($dsPointer);
};
if ($@) {
    die "Error: $@\n";
}
