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

package Tutorials;

import FiftyOneDegreesTrieV3.FiftyOneDegreesTrieV3;
import FiftyOneDegreesTrieV3.Match;
import FiftyOneDegreesTrieV3.Provider;

import java.io.IOException;
import java.net.URL;

/**
 * <tutorial>
 * Getting started example of using 51Degrees device detection. The example
 * shows how to:
 * <ol>
 *  <li>
 *  Load the 51Degrees Hash Trie library
 *  <pre class="prettyprint lang-java">
 *  <code>
 *      res = FiftyOneDegreesTrieV3.class.getResource("/FiftyOneDegreesTrieV3.so");
 *      System.load(res.getPath());
 *  </code>
 *  </pre>
 *  </li>
 *  <li>Instantiate 51Degrees detection provider
 *  <pre class="prettyprint lang-java">
 *  <code>
 *      provider = new Provider("path/to/51Degrees/data/file.trie");
 *  </code>
 *  </pre>
 *  </li>
 *  <li>Pass in a single HTTP User-Agent header
 *  <pre class="prettyprint lang-java">
 *  <code>
 *      Match match = provider.match(userAgent);
 *  </code>
 *  </pre>
 *  </li>
 *  <li>Extract the value of the IsMobile Property
 *  <pre class="prettyprint lang-java">
 *  <code>
 *      match.getValue("IsMobile").toString();
 *  </code>
 *  </pre>
 *  </li>
 * </ol>
 * main assumes it is being run with a working directory at root of
 * project or of this module.
 * </tutorial>
 */
public class GettingStarted {

    // Snippet Start
    // User-Agent string of an iPhone mobile device.
    private static String mobileUserAgent = "Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) " +
            "AppleWebKit/537.51.2 (KHTML, like Gecko) 'Version/7.0 Mobile/11D167 Safari/9537.53";

    // User-Agent string of Firefox Web browser version 41 on desktop.
	private static String desktopUserAgent = "Mozilla/5.0 (Windows NT 6.3; WOW64; rv:41.0) " +
            "Gecko/20100101 Firefox/41.0";

    // User-Agent string of a MediaHub device.
	private static String mediaHubUserAgent = ("Mozilla/5.0 (Linux; Android 4.4.2; X7 Quad Core " +
            "Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 " +
            "Chrome/30.0.0.0 Safari/537.36");

    /**
     * Main entry point for this example. For each of the User-Agents defined
     * in this class:
     * <ol>
     * <li>invokes {@link Match#getValue(java.lang.String)} method; and
     * <li>prints results.
     * </ol>
     *
     * Result in this case will be either True or False, depending on whether
     * the User-Agent belongs to a mobile device or a non-mobile device.
     *
     * @param args command line arguments, not used.
     * @throws java.io.IOException if there is a problem accessing the data file
     * that will be used for device detection.
     */
    public static void main(String[] args) throws IOException {
        // Load the C/C++ native library. Uncomment dll line for windows and so line in linux.
        URL res = FiftyOneDegreesTrieV3.class.getResource("/FiftyOneDegreesTrieV3.dll");
        // URL res = FiftyOneDegreesTrieV3.class.getResource("/FiftyOneDegreesTrieV3.so");
        System.load(res.getPath());

        // Create a new provider.
        Provider provider = new Provider("../../data/51Degrees-LiteV3.4.trie");

        System.out.println("Starting Getting Started Example.");

        // Carries out a match for a mobile User-Agent.
        Match match = provider.getMatch(mobileUserAgent);
        System.out.println("\nMobile User-Agent: " + mobileUserAgent);
        System.out.println("   IsMobile: " + match.getValue("IsMobile"));

        // Carries out a match for a desktop User-Agent.
        match = provider.getMatch(desktopUserAgent);
        System.out.println("\nDesktop User-Agent: " + desktopUserAgent);
        System.out.println("   IsMobile: " + match.getValue("IsMobile"));

        // Carries out a match for a media hub User-Agent.
        match = provider.getMatch(mediaHubUserAgent);
        System.out.println("\nMediaHub User-Agent: " + mediaHubUserAgent);
        System.out.println("   IsMobile: " + match.getValue("IsMobile"));

    }
    // Snippet End
}
