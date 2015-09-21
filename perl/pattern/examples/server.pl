#!/usr/bin/perl
{
	package TestServer;

	use HTTP::Server::Simple::CGI;
	use base qw(HTTP::Server::Simple::CGI);
	use FiftyOneDegrees::PatternV3;
	use String::Buffer;
	use JSON;

	my $propertyList //= "Id,BrowserName,BrowserVendor,BrowserVersion,DeviceType,HardwareVendor,IsTablet,IsMobile,IsCrawler,ScreenInchesDiagonal";

	my $filename //= "../../../data/51Degrees-LiteV3.2.dat";

	my $dsPointer = FiftyOneDegrees::PatternV3::dataSetInitWithPropertyString(
		$filename,
		$propertyList,
		10, # 10 concurrent detections
		10000 # Cache size of 10,000
	);

	my %importantHttpHeaders = ();
	my $importantHttpHeaderIndex = 0;

	my $importantHttpHeader = FiftyOneDegrees::PatternV3::getHttpHeaderName($dsPointer, $importantHttpHeaderIndex);
	while ($importantHttpHeader ne "") {
		$importantHttpHeaders{$importantHttpHeader} = $importantHttpHeader;
		$importantHttpHeaderIndex++;
		$importantHttpHeader = FiftyOneDegrees::PatternV3::getHttpHeaderName($dsPointer, $importantHttpHeaderIndex);
	}

	sub handle_request {
		my $self = shift;
		my $q  = shift;   # CGI.pm object
		return if !ref $q;

		print "HTTP/1.0 200 OK\r\n";
		print $q->header('text/plain');

		# Create a string with all relevant HTTP header names and values.
		print "Found the following important HTTP headers:\n";
		my %headers = map { $_ => $q->http($_) } $q->http();
		my $matchHeaders = String::Buffer->new();
		while (my ($key, $value) = each %headers) {
			if ($importantHttpHeaders{$key}) {
				$matchHeaders->writeln("$key: $value");
				print "$key: $value\r\n";
			}
		}

		# Get the properties for these headers.
		my $json = FiftyOneDegrees::PatternV3::getMatchWithHeaders($dsPointer, $matchHeaders->flush());
		my %properties = %{ decode_json($json) };

		# Output the properties and values.
		print "\nDetected the following properties:\n";
		while (my ($key, $value) = each %properties) {
			print "$key: $value\r\n";
		}
	}
}

# start the server on port 8080
my $pid = TestServer->new(8080)->background();
print "Use 'kill $pid' to stop server.\n";
