# *********************************************************************
# This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
# Copyright 2015 51Degrees Mobile Experts Limited, 5 Charlotte Close,
# Caversham, Reading, Berkshire, United Kingdom RG4 7BY
#
# This Source Code Form is the subject of the following patent
# applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
# Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
# European Patent Application No. 13192291.6; and
# United States Patent Application Nos. 14/085,223 and 14/085,301.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0.
#
# If a copy of the MPL was not distributed with this file, You can obtain
# one at http://mozilla.org/MPL/2.0/.
#
# This Source Code Form is "Incompatible With Secondary Licenses", as
# defined by the Mozilla Public License, v. 2.0.
# **********************************************************************

#!/usr/bin/env perl
{
# // Snippet Start
	package TestServer;

	use HTTP::Server::Simple::CGI;
	use base qw(HTTP::Server::Simple::CGI);
	use FiftyOneDegrees::TrieV3;
	use String::Buffer;

    # List of 51Degrees properties to make available. Where a property is not
    # supported in the provided data file it will not be available.
	my $propertyList = "BrowserName,BrowserVendor,BrowserVersion," .
                    "DeviceType,HardwareVendor,IsTablet,IsMobile," .
                    "IsCrawler,ScreenInchesDiagonal,ScreenPixelsWidth";

	# Replace with the following lines for Premium or Enterprise data.
	#
	# my $filename = "../../data/51Degrees-PremiumV3.2.trie";
	# my $filename = "../../data/51Degrees-EnterpriseV3.2.trie";
	#
	# Premium and Enterprise data files contain more properties, are updated
    # more frequently and are more accurate than the free Lite data.
	#
	# See https://51degrees.com/compare-data-options to get data files.
    #
    # Set the location of the source data file for the web server.
	my $filename = "../../data/51Degrees-LiteV3.2.trie";

    # Create a device detection provider with a cache for 50,000 User-Agents
    # and 20 concurrent detections.
    my $provider = new FiftyOneDegrees::TrieV3::Provider(
        $filename, $propertyList);

    # An array of the important HTTP headers in prefixed uppercase format
    # (i.e. HTTP_USER_AGENT rather than User-Agent) to send to the detection
    # algorithm.
    my $importantHttpHeaders = $provider->getHttpHeaders();

    # Array of all the properties available. May not match $propertyList as
    # properties may not be supported by the provided data file.
    my $properties = $provider->getAvailableProperties();

	# The number of rows in a table listing each property requested.
	my $propertyRows = (scalar (split(',', $propertyList)) + 1);

	# Build some HTML button snippets to add to the generated page.
	my $dataOptions;
	if ('Lite' == $provider->getDataSetName()) {
		$dataOptions = '<a class="button" target="_blank" href="https://51degrees.com/compare-data-options" title="Compare Premium and Enterprise Data Options">Compare Data Options</a>';
	}
	my $propertiesHyperLinkUA = '<a class="button" target="_blank" href="https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=example_trie_properties_ua&utm_campaign=perl-open-source" title="Review All Properties">More Properties</a>';
	my $methodHyperLinkHeaders = '<a class="button" target="_blank" href="https://51degrees.com/support/documentation/trie?utm_source=github&utm_medium=repository&utm_content=example_trie_headers&utm_campaign=perl-open-source" title="How Trie Device Detection Works">About Metrics</a>';
	my $propertiesHyperLinkHeaders = '<a class="button" target="_blank" href="https://51degrees.com/resources/property-dictionary?utm_source=github&utm_medium=repository&utm_content=example_trie_properties_headers&utm_campaign=perl-open-source" title="Review All Properties">More Properties</a>';
	my $propertyNotFound = '<a target="_blank" href="https://51degrees.com/compare-data-options?utm_source=github&utm_medium=repository&utm_content=example_trie_compare&utm_campaign=perl-open-source">Switch Data Set</a>';

    # Relate URL paths to response handlers.
    my %dispatch = (
        '/json' => \&resp_json,
    );

    # Listen for incoming requests performing device detection and providing
    # simple output for the example.
	sub handle_request {
		my $self = shift;
		my $cgi  = shift;   # CGI.pm object
		return if !ref $cgi;

		# Create a map with all relevant HTTP header names and values.
		my %headers = map { $_ => $cgi->http($_) } $cgi->http();
		my $matchingHttpHeaders = new FiftyOneDegrees::TrieV3::MapStringString();
		foreach $httpHeader (@$importantHttpHeaders) {
            my $value = $headers{$httpHeader};
            if ($value) {
                $matchingHttpHeaders->set($httpHeader, $value);
            }
        }

        print "HTTP/1.0 200 OK\r\n";

		my $handler = $dispatch{$cgi->path_info()};
        if (ref($handler) ne "CODE") {
            $handler = \&resp_default;
        }

        $handler->($cgi, $matchingHttpHeaders);
	}

	# Provides a JSON plan text response with properties for the requesting device.
	sub resp_json {
        my $cgi = shift;
		my $matchingHttpHeaders = shift;

		print $cgi->header('text/plain');

		# Get the JSON for the available properties for these headers.
		my $json = $provider->getMatchJson($matchingHttpHeaders);

		# Display the result.
		print $json;
	}

	# Outputs a standard HTML table with the match results.
	sub resp_match {
		my $match = shift;

        foreach $property (split(',', $propertyList)) {
            my $values = $match->getValues($property);
			print '<tr><td><a target="_blank" href="https://51degrees.com/resources/property-dictionary#' . $property .
					' title="Read About ' . $property . '">' . $property . '</a></td><td>';
			if (scalar @$values > 0) {
				print join(',', @$values);
			}
			else {
				print $propertyNotFound;
			}
			print '</td></tr>';
        }
        print '</table>';
	}

	# Outputs details about the requesting device.
	sub resp_default {
		my $cgi = shift;
		my $matchingHttpHeaders = shift;

		print $cgi->header('text/html');

        print '<!doctype html>';
        print '<html>';
        print '<link rel="stylesheet" type="text/css" href="https://51degrees.com/Demos/examples.css" class="inline">';
        print '<body>';
        print '<div class="content">';
        print '<p><img src="https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx?utm_source=github&utm_medium=repository&utm_content=server&utm_campaign=perl-open-source"></p>';
        print '<h1>Perl Trie - Device Detection Server Example</h1>';

        print '<table>';
        print '<tr><th colspan="3">Data Set Information</th></tr>';
        print '<tr><td>Name</td><td>' . $provider->getDataSetName() . '</td><td rowspan="6">' . $dataOptions . '</td></tr>';
        print '<tr><td>Format</td><td>' . "Not available for Trie." . '</td></tr>';
        print '<tr><td>Published Date</td><td>' . "Not available for Trie." . '</td></tr>';
        print '<tr><td>Next Update Date</td><td>' . "Not available for Trie." . '</td></tr>';
        print '<tr><td>Signature Count</td><td>' . "Not available for Trie." . '</td></tr>';
        print '<tr><td>Device Combinations</td><td>' . "Not available for Trie." . '</td></tr>';
        print '</table>';

        # Display the matching device details for the User-Agent.
		my $userAgent = $matchingHttpHeaders->get("HTTP_USER_AGENT");
        print '<table>';
        print '<tr><th colspan="2">Match from User-Agent</th></tr>';
        print '<tr><td>User-Agent</td><td>' . $userAgent . '</td></tr>';
        print '</table>';

	my $match = $provider->getMatch($userAgent);
        print '<table>';
        print '<tr><th colspan="2">Device Properties</th><td rowspan="' . $propertyRows . '">' . $propertiesHyperLinkUA . '</td></tr>';
		resp_match($match);

        # Display the matching device details for the HTTP headers.
		print '<table>';
        print '<tr><th colspan="2">Match with HTTP Headers</th></tr>';
        print '<tr><th colspan="2">Relevant HTTP Headers</th></tr>';
        foreach $headerName (@$importantHttpHeaders) {
			print '<tr><td>' . $headerName . '</td><td>';
			if ($matchingHttpHeaders->has_key($headerName)) {
				print $matchingHttpHeaders->get($headerName);
			} else {
				print '<i>header not set</i>';
			}
			print '</td></tr>';
		}
        print '</table>';

	my $match = $provider->getMatch($matchingHttpHeaders);
        print '<table>';
        print '<tr><th colspan="2">Device Properties</th><td rowspan="' . $propertyRows . '">' . $propertiesHyperLinkHeaders . '</td></tr>';
		resp_match($match);

		print '</div>';
		print '</body>';
		print '</html>';
	}
}

# start the server on port 8080
my $pid = TestServer->new(8080)->background();
print "Use 'kill $pid' to stop server.\n";
# // Snippet End
