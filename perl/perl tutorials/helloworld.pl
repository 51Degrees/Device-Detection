use FiftyOneDegrees::PatternV3;
use feature qw/say/;
use Getopt::Long;
use JSON;

my $filename;
my $propertyList;
my $userAgentString;

$filename = "51Degrees-LiteV3.2.dat";
my $propertyList = "BrowserName,PlatformName" .
	"HardwareVendor,IsMobile," .
	"ScreenPixelsWidth";

$userAgentString = "Mozilla/5.0 (Linux; Android 4.4.2; en-us; SAMSUNG " .
	"SCH-I545 Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) " .
	"Version/1.5 Chrome/28.0.1500.94 Mobile Safari/537.36";

my $provider = new FiftyOneDegrees::PatternV3::Provider(
	$filename,
	$propertyList);

my $match = $provider->getMatch($userAgentString);

my $properties = $provider->getAvailableProperties();
foreach $property (@$properties) {
	my $value = $match->getValue($property);
	say "$property: $value";
}


