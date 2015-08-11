use FiftyOneDegrees::PatternV3;
use JSON;
use Data::Dumper;
use feature qw/say/;
use Getopt::Long;
use Cwd;

my $filename;
my $propertyList;
my $userAgentString;

#local variable declarations
my $cacheSize = 2;
my $poolSize = 5;
my $json;


# my $r = GetOptions('filename|f=s' => \$filename,
#                   'properties|p=s' => \$propertyList,
#                   'ua=s' => \$userAgentString);

$filename //= "/home/fod/Device-Detection/data/51Degrees-LiteV3.2.dat";
my $pwd = cwd();

$userAgentString1 //= "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.114 Safari/537.36";

$userAgentString = "Mozilla/5.0 (Linux; U; Android 4.1.2; de-de; GT-N8020 Build/JZO54K) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Safari/534.30";

$propertyList //= "Id,BrowserName,BrowserVendor,BrowserVersion,CookiesCapable,IsTablet,IsMobile,IsCrawler";

die "DATa file does not exist! usage: $filename -f <dat filename>\n" unless -e ("$filename");

# Initialize Pattern library

# For pattern a dataset pointer is needed. This will be passd into other
# PatternV3 functions.
eval {

	my $dsPointer = FiftyOneDegrees::PatternV3::dataSetInitWithPropertyString($filename, $propertyList);
# Need to create a cache and get a pointer to it
        my $cachePointer = FiftyOneDegrees::PatternV3::cacheCreate($dsPointer,$cacheSize);
# Need to create a pool and get a pointer to it
        my $poolPointer = FiftyOneDegrees::PatternV3::poolCreate($dsPointer, $cachePointer, $poolSize);
# Need to get a workset from the pool
#        my $wsPointer = FiftyOneDegrees::PatternV3::worksetGet($poolPointer);
# Test the getMatch function using the useragent string
#	FiftyOneDegrees::PatternV3::getMatch($wsPointer, $userAgentString);
# Output the results using the old JSON method	
#        FiftyOneDegrees::PatternV3::createJSON($wsPointer);
#        my $jsout = FiftyOneDegrees::PatternV3::processJSON($wsPointer, $json);
	say "=== JSON Data Output ===";
#	my %properties = %{ decode_json($json) };
#	# Loop through and print all returned properties
#	while (my ($key, $value) = each %properties) {
#		say $key . ": " . $value;
#	}
#        FiftyOneDegrees::PatternV3::freeJSON($json);
# Output the results using CSV method
#        my $csv;
#        FiftyOneDegrees::PatternV3::createCSV($wsPointer);
#        my $csvout = FiftyOneDegrees::PatternV3::processCSV($wsPointer, $csv);
#        FiftyOneDegrees::PatternV3::freeCSV($csv);
#        say "=== CSV Data Output ===";
#        say $csv;
#        FiftyOneDegrees::PatternV3::freeCSV($csv);
#        FiftyOneDegrees::PatternV3::worksetRelease($poolPointer,$wsPointer);
# Test the getMatchWithHeadersArray function using the multiple headers
#        my @httpHeaderNames = ("Name1 ", "Name2");
#        my @httpHeaderValues = ("Value1 ", "Value2");
#        my $wsPointer = FiftyOneDegrees::PatternV3::worksetGet($poolPointer);
#        FiftyOneDegrees::PatternV3::getMatchWithHeadersArray($wsPointer, @httpHeaderNames, @httpHeaderValues, 2);
# Output the results using the old JSON method	
#        FiftyOneDegrees::PatternV3::createJSON($wsPointer);
#        my $jsout = FiftyOneDegrees::PatternV3::processJSON($wsPointer, $json);
#	say "=== JSON Data Output ===";
#	my %properties = %{ decode_json($json) };
#	# Loop through and print all returned properties
#	while (my ($key, $value) = each %properties) {
#		say $key . ": " . $value;
#	}
#        FiftyOneDegrees::PatternV3::freeJSON($json);
# Output the results using CSV method
#        my $csv;
#        FiftyOneDegrees::PatternV3::createCSV($wsPointer);
#        my $csvout = FiftyOneDegrees::PatternV3::processCSV($wsPointer, $csv);
#        FiftyOneDegrees::PatternV3::freeCSV($csv);
#        say "=== CSV Data Output ===";
#        say $csv;
#        FiftyOneDegrees::PatternV3::freeCSV($csv);
#        FiftyOneDegrees::PatternV3::worksetRelease($poolPointer,$wsPointer);

# Test the getMatchWithHeadersString function using the multiple headers
#        my $httpHeaders = ("Name1 Name2 Value1 Value2");
#        my $wsPointer = FiftyOneDegrees::PatternV3::worksetGet($poolPointer);
#        FiftyOneDegrees::PatternV3::getMatchWithHeadersString($wsPointer, $httpHeaders);
# Output the results using the old JSON method	
#        FiftyOneDegrees::PatternV3::createJSON($wsPointer);
#        my $jsout = FiftyOneDegrees::PatternV3::processJSON($wsPointer, $json);
#	say "=== JSON Data Output ===";
#	my %properties = %{ decode_json($json) };
#	# Loop through and print all returned properties
#	while (my ($key, $value) = each %properties) {
#		say $key . ": " . $value;
#	}
#        FiftyOneDegrees::PatternV3::freeJSON($json);
# Output the results using CSV method
#        my $csv;
#        FiftyOneDegrees::PatternV3::createCSV($wsPointer);
#        my $csvout = FiftyOneDegrees::PatternV3::processCSV($wsPointer, $csv);
#        FiftyOneDegrees::PatternV3::freeCSV($csv);
#        say "=== CSV Data Output ===";
#        say $csv;
#        FiftyOneDegrees::PatternV3::freeCSV($csv);
#        FiftyOneDegrees::PatternV3::worksetRelease($poolPointer,$wsPointer);

        FiftyOneDegrees::PatternV3::freePool($poolPointer);
        FiftyOneDegrees::PatternV3::freeCache($cachePointer);
	FiftyOneDegrees::PatternV3::freeDataset($dsPointer);
};
if ($@) {
    die "Error: $@\n";
}
