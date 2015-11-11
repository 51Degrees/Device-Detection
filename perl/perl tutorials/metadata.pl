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

my $provider = new FiftyOneDegrees::PatternV3::Provider(
	$filename,
	$propertyList);

my $datasetPublishedDate = $provider->getDataSetPublishedDate();
my $datasetNextUpdateDate = $provider->getDataSetNextUpdateDate();
my $datasetVersion = $provider->getDataSetFormat();
my $datasetName = $provider->getDataSetName();
my $datasetDeviceCombinations = $provider->getDataSetDeviceCombinations();

say "Dataset Published Date: $datasetPublishedDate";
say "Dataset Next Update Date: $datasetNextUpdateDate";
say "Dataset Version: $datasetVersion";
say "Dataset Name: $datasetName";
say "Dataset Device Combinations: $datasetDeviceCombinations";
