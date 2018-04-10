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
Getting started example of using 51Degrees Hash Trie device detection. 
The example shows how to:
<ol>
<li>Instantiate the 51Degrees device detection provider.
<p><pre class="prettyprint lang-go">
var provider = FiftyOneDegreesTrieV3.NewProvider(dataFile)
</pre></p>
<li>Produce a match for a single HTTP User-Agent header
<p><pre class="prettyprint lang-go">
var match = provider.GetMatch(userAgent)
</pre></p>
<li>Extract the value of the IsMobile property as a boolean
<p><pre class="prettyprint lang-go">
if match.GetValue("IsMobile") == "True"{
	return true
}
return false
</pre></p>
</ol>
This example assumes you have the 51Degrees Go API installed correctly,.
</tutorial>
*/

// Snippet Start
package main

import (
	"fmt"
	"./src/trie"
)

// Location of data file.
var dataFile = "../data/51Degrees-LiteV3.4.trie"

// Provides access to device detection functions.
var provider =
	FiftyOneDegreesTrieV3.NewProvider(dataFile)

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

// isMobile function. Takes a User-Agent as an argument, carries out a
// match and returns a boolean value for the IsMobile property of the
// matched device.
func isMobile(userAgent string) bool{
	var match = provider.GetMatch(userAgent)
	if match.GetValue("IsMobile") == "True"{
		return true
	}
	return false
}

func main() {
	fmt.Println("Mobile User-Agent: ", mobileUserAgent)

	// Determines whether the mobile User-Agent is a mobile device.
	if isMobile(mobileUserAgent){
	fmt.Println("   Mobile")
	}else{
	fmt.Println("   Non-Mobile")}

	// Determines whether the desktop User-Agent is a mobile device.
	fmt.Println("Desktop User-Agent: ", desktopUserAgent)
	if isMobile(desktopUserAgent) {
		fmt.Println("   Mobile")
	}else {
		fmt.Println("   Non-Mobile")
	}

	// Determines whether the MediaHub User-Agent is a mobile device.
	fmt.Println("Media Hub User-Agent: ", mediaHubUserAgent)
	if isMobile(mediaHubUserAgent) {
		fmt.Println("   Mobile")
	}else {
		fmt.Println("   Non-Mobile")
	}
}
// Snippet End