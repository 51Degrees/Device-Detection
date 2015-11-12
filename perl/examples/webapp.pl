#!/usr/bin/env perl
=tutorial
=cut
{
	package TestServer;

	use HTTP::Server::Simple::CGI;
	use base qw(HTTP::Server::Simple::CGI);
	use FiftyOneDegrees::PatternV3;

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

	my $properties = $provider->getAvailableProperties();

	my $importantHttpHeaders = $provider->getHttpHeaders();
	
	my %dispatch = ();
		sub handle_request {
			my $self = shift;
			my $cgi  = shift;   # CGI.pm object
			return if !ref $cgi;

			# Create a map with all relevant HTTP header names and values.
			my %headers = map { $_ => $cgi->http($_) } $cgi->http();
			my $matchingHttpHeaders = new FiftyOneDegrees::PatternV3::MapStringString();
			foreach $httpHeader (@$importantHttpHeaders) {
				my $value = $headers{$httpHeader};
				if ($value) {
					$matchingHttpHeaders->set($httpHeader, $value);
				}
			}

			my $match = $provider->getMatch($matchingHttpHeaders);
			my $isMobile = $match->getValue("IsMobile");
			if ($isMobile eq "True") {
				print "Mobile Site\n";
			}
			else {
				print "Desktop Site\n";
			}
		}
}

my $pid = TestServer->new(8080)->background();
print "Use 'kill $pid' to stop server.\n";
