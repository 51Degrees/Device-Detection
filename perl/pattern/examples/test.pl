use FiftyOneDegrees::PatternV3;
use JSON;
use Data::Dumper;
use feature qw/say/;
use Getopt::Long;
use Cwd;

my $filename;
my $propertyList;
my $userAgentString;

# my $r = GetOptions('filename|f=s' => \$filename,
#                   'properties|p=s' => \$propertyList,
#                   'ua=s' => \$userAgentString);

$filename //= "/home/fod/Device-Detection/data/51Degrees-LiteV3.2.dat";
my $pwd = cwd();

$userAgentString //= "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.114 Safari/537.36";

$propertyList //= "Id,BrowserName,BrowserVendor,BrowserVersion,CookiesCapable,IsTablet,IsMobile,IsCrawler";

die "DATa file does not exist! usage: $filename -f <dat filename>\n" unless -e ("$filename");

# Initialize Pattern library

# For pattern a dataset pointer is needed. This will be passd into other
# PatternV3 functions.
eval {

	my $dsPointer = FiftyOneDegrees::PatternV3::dataSetInitWithPropertyString($filename, $propertyList);
        my $cachePointer = FiftyOneDegrees::PatternV3::cacheInitWithDataSet($dsPointer);
        my $poolPointer = FiftyOneDegrees::PatternV3::poolInitWithDataSet($dsPointer, $cachePointer );
	my $json = FiftyOneDegrees::PatternV3::getMatch($poolPointer, $userAgentString);
	
	say "=== Data Output ===";
	my %properties = %{ decode_json($json) };
	# Loop through and print all returned properties
	while (my ($key, $value) = each %properties) {
		say $key . ": " . $value;
	}
        FiftyOneDegrees::PatternV3::freeJSON($json);
        FiftyOneDegrees::PatternV3::freePool($poolPointer);
        FiftyOneDegrees::PatternV3::freeCache($cachePointer);
	FiftyOneDegrees::PatternV3::freeDataset($dsPointer);
};
if ($@) {
    die "Error: $@\n";
}
