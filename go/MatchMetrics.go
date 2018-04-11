/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright 2017 51Degrees Mobile Experts Limited, 5 Charlotte Close,
 * Caversham, Reading, Berkshire, United Kingdom RG4 7BY
 *
 * This Source Code Form is the subject of the following patents and patent
 * applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
 * Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
 * European Patent No. 2871816;
 * European Patent Application No. 17184134.9;
 * United States Patent Nos. 9,332,086 and 9,350,823; and
 * United States Patent Application No. 15/686,066.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.
 *
 * If a copy of the MPL was not distributed with this file, You can obtain
 * one at http://mozilla.org/MPL/2.0/.
 *
 * This Source Code Form is "Incompatible With Secondary Licenses", as
 * defined by the Mozilla Public License, v. 2.0.
 ********************************************************************** */

/*
<tutorial>
Getting started example of using 51Degrees device detection match metrics
information. The example shows how to:
<ol>
<li>Instantiate the 51Degrees device detection provider.
<p><pre class="prettyprint lang-go">
var provider = FiftyOneDegreesPatternV3.NewProvider(dataFile)
</pre></p>
<li>Produce a match for a single HTTP User-Agent header
<p><pre class="prettyprint lang-go">
var match = provider.GetMatch(userAgent)
</pre></p>
<li>Obtain device Id: consists of four components separated by a hyphen
symbol: Hardware-Platform-Browser-IsCrawler where each Component
represents an ID of the corresponding Profile.
<p><pre class="prettyprint lang-go">match.GetDeviceId()</pre>
<li>Obtain match method: provides information about the
algorithm that was used to perform detection for a particular User-Agent.
For more information on what each method means please see:
<a href="https://51degrees.com/support/documentation/pattern">
How device detection works</a>
<p><pre class="prettyprint lang-go">match.GetMethod()</pre>
<li>Obtain difference:  used when detection method is not Exact or None.
This is an integer value and the larger the value the less confident the
detector is in this result.
<p><pre class="prettyprint lang-go">match.GetDifference()</pre>
<li>Obtain signature rank: an integer value that indicates how popular
the device is. The lower the rank the more popular the signature.
<p><pre class="prettyprint lang-go">match.GetRank()</pre>
</ol>
This example assumes you have the 51Degrees Go API installed correctly.
</tutorial>
*/

// Snippet Start
package main

import (
	"fmt"
	"./src/pattern"
)

var matchMethod = []string {
   	"NONE",  
	"EXACT",
	"NUMERIC",
	"NEAREST",
	"CLOSEST"}

// Location of data file.
var dataFile = "../data/51Degrees-LiteV3.2.dat"

// Provides access to device detection functions.
var provider =
	FiftyOneDegreesPatternV3.NewProvider(dataFile)

// Which properties to retrieve
var properties = []string{"IsMobile", "PlatformName", "PlatformVersion"}

// User-Agent string of an iPhone mobile device.
var mobileUserAgent = "Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) " +
	"AppleWebKit/537.51.2 (KHTML, like Gecko) 'Version/7.0 Mobile/11D167 " +
	"Safari/9537.53"

// User-Agent string of Firefox Web browser version 41 on desktop.
var desktopUserAgent = "Mozilla/5.0 (Windows NT 6.3; WOW64; rv:41.0) " +
	"Gecko/20100101 Firefox/41.0"

// User-Agent string of a MediaHub device.
var mediaHubUserAgent = "Mozilla/5.0 (Linux; Android 4.4.2; X7 Quad Core " +
	"Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 " +
	"Chrome/30.0.0.0 Safari/537.36"

// output_metrics function. Takes a match object as an argument and
// prints the match metrics relating to the specific match.
func outputMetrics(match FiftyOneDegreesPatternV3.Match) {
	fmt.Println("   Id: ", match.GetDeviceId())
	fmt.Println("   Match Method: ", matchMethod[match.GetMethod()])
	fmt.Println("   Difference: ", match.GetDifference())
	fmt.Println("   Rank: ", match.GetRank())
}

func main() {
	fmt.Println("Starting Getting Started Match Metrics Example.\n")

	// Carries out a match with a mobile User-Agent.
	fmt.Println("Mobile User-Agent: ", mobileUserAgent)
	var match = provider.GetMatch(mobileUserAgent)
	outputMetrics(match)

	// Carries out a match with a desktop User-Agent.
	fmt.Println("Desktop User-Agent: ", desktopUserAgent)
	match = provider.GetMatch(desktopUserAgent)
	outputMetrics(match)

	// Carries out a match with a MediaHub User-Agent.
	fmt.Println("Media Hub User-Agent: ", mediaHubUserAgent)
	match = provider.GetMatch(mediaHubUserAgent)
	outputMetrics(match)
}
// Snippet End
