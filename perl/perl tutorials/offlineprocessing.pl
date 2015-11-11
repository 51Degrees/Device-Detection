=tutorial
<tutorial>
<ol>
<li>Imports the detector and settings modules,
<li>Sets the data file, properties, cache size and pool size,
<li>Initializes the provider,
<li>Opens an input file with a list of User-Agents, and an output file,
<li>Writes a header to the output file with the property names in '|'
separated CSV format
<li>For the first 20 User-Agents in the input file, a match is performed and
the User-Agent along with the values for chosen properties are output to
the CSV.
</ol>
</tutorial>
=cut
# // Snippet Start
use FiftyOneDegrees::PatternV3;
use feature qw/say/;
use Getopt::Long;
use JSON;

my $filename;
my $propertyList;
my $userAgentString;

$filename = "51Degrees-LiteV3.2.dat";
my $propertyList = "IsMobile,BrowserName";
my $provider = new FiftyOneDegrees::PatternV3::Provider(
	$filename,
	$propertyList);
my $properties = $provider->getAvailableProperties();

open my $file_in, "20000 User Agents.csv";
open file_out, ">output.csv";

print file_out "User-Agent";
foreach $property (@$properties) {
	print file_out "|".$property;
}
print file_out "\n";

while( my $userAgent = <$file_in>) {
	chomp $userAgent;
	my $match = $provider->getMatch($userAgent);
	print file_out $userAgent;
	foreach $property (@$properties) {
		print file_out "|".$match->getValue($property);
	}
	print file_out "\n";
	last if $. == 20;
}
close $file_in;
close file_out;
# // Snippet End
