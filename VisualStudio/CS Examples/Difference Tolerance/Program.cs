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
Difference Tolerance example of using 51Degrees device detection. The example shows 
how to:
<ol>
    <li>Initialise detector with path to the 51Degrees device data file and 
    a list of properties.
    <p><code>
        string properties = "IsMobile";<br />
        string fileName = args[0];<br />
        Provider provider = new Provider(FileName, properties);
    </code></p>
    <li>Match a single HTTP User-Agent header string.
    <p><code>
        Match match;
        using (match = provider.getMatch(userAgent)) {
            // Do something with match result.
        }
    </code></p>
    <li>Set the difference tolerance in the provider.
    <p><code>
        provider.setDifference(1);
    </code></p>
    <li>Extract the value of the IsMobile property.
    <p><code>
        string IsMobile;
        IsMobile = match.getValue("IsMobile");
    </code></p>
    <li> Dispose of the Provider releasing the resources.
    <p><code>
        provider.Dispose();
    </code></p>
</ol>
<p>
    This tutorial assumes you are building this example using Visual Studio. 
    You should supply path to the 51Degrees device data file that you wish to 
    use as a command line argument.
</p>
<p>
    By default the API is distributed with Lite data which is free to use for 
    both the non-commercial and commercial purposes. Lite data file contains 
    over 60 properties. For more properties like DeviceType, PriceBand and 
    HardwareVendor check out the Premium and Enterprise data files:
    https://51degrees.com/compare-data-options
</p>
<p>
    Passing a list of properties to the provider constructor limits the number 
    of properties in the dataset the provider wraps to only the chosen 
    properties. Not providing a list or providing an empty list will cause the 
    dataset to be created with all available properties:
    <br /><code>
        Provider provider = new Provider(fileName, "");
        Provider provider = new Provider(fileName);
    </code>
</p>
<p>
    Provider wraps the dataset which is the data file loaded into memory and 
    ready to be used through the methods exposed by the Provider. Failing to 
    dispose of the provider when it is no longer required means the memory 
    allocated for the provider  and the dataset will never be released.
</p>
</tutorial>
*/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using FiftyOne.Mobile.Detection.Provider.Interop.Trie;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace FiftyOne.Example.Illustration.CSharp.DifferenceTolerance
{
    public class Program
    {
        // Snippet Start
        /// <summary>
        /// Runs the program.
        /// </summary>
        /// <param name="fileName">
        /// Path to the 51Degrees device data file.
        /// </param>
        public static void Run(string fileName)
        {
            // Comma-separated list of properties.
            string properties = "PlatformName,PlatformVersion";
            // User-Agent template.
            string userAgent = "Mozilla/5.0 (Linux; Android 4.2.{0}; nl-nl; " +
                "SAMSUNG GT-I9505 Build/JDQ39) AppleWebKit/535.19 (KHTML, " +
                "like Gecko) Version/1.0 Chrome/18.0.1025.308 Mobile " +
                "Safari/535.19";
            // User-Agent of an Android 4.2.2 device.
            string originalUserAgent = String.Format(userAgent, "2");
            // User-Agent of an Android 4.2.3 device (which does not exist).
            string alteredUserAgent = String.Format(userAgent, "3");

            // Use path to the data file and a list of properties to create 
            // provider.
            Provider provider = new Provider(fileName, properties);

            Console.WriteLine("Starting Difference Tolerance Example.");

            // Carry out a match for the known User-Agent.
            Console.WriteLine(String.Format(
                "\nOriginal User-Agent: '{0}'",
                originalUserAgent));
            detect(provider, originalUserAgent, 0, "4.2.2");

            // Carry out a match for the unknown (and non-existent) User-Agent.
            Console.WriteLine(String.Format(
                "\nAltered User-Agent: '{0}'",
                alteredUserAgent));
            detect(provider, alteredUserAgent, 0, null);

            // Carry out a match for the unknown User-Agent while allowing an
            // ASCII difference of 1 (i.e. the difference between 4.2.2 and
            // 4.2.3).
            Console.WriteLine(String.Format(
                "\nAltered User-Agent: '{0}'",
                alteredUserAgent));
            detect(provider, alteredUserAgent, 1, "4.2.2");
        }

        /// <summary>
        /// Performs detection by invoking the getMatch method of the provider 
        /// and disposing of the match result object after printing result and 
        /// performing Assert.
        /// </summary>
        /// <remarks>
        /// <param name="provider">
        /// FiftyOne Provider that enables methods to interact with the 
        /// 51Degrees device data file.
        /// </param>
        /// <param name="userAgent">
        /// A string containing the HTTP User-Agent to identify.
        /// </param>
        /// <param name="difference">
        /// The difference tolerance to allow when matching.
        /// </param>
        /// <param name="expected">
        /// Used for test purposes only. Contains the expected value for the 
        /// PlatformVersion property, or null if it should not be checked.
        /// </param>
        public static void detect(Provider provider,
                          string userAgent,
                          int difference,
                          string expected)
        {
            Match match;
            string PlatformName, PlatformVersion;

            using (match = provider.getMatchWithTolerances(
                userAgent,
                0,
                difference))
            {
                PlatformName = match.getValue("PlatformName");
                PlatformVersion = match.getValue("PlatformVersion");
                if (expected != null)
                    Assert.AreEqual(expected, PlatformVersion);
#if DEBUG
                Console.WriteLine(
                    String.Format("Matched Substrings: '{0}'",
                    match.getUserAgent()));
#endif
                Console.WriteLine(String.Format("   Platform: {0} {1}",
                    PlatformName,
                    PlatformVersion));
            }
        }
        // Snippet End

        static void Main(string[] args)
        {
            string fileName = args.Length > 0 ? args[0] :
                "../../../../../../data/51Degrees-LiteV3.4.trie";
            Run(fileName);

            // Wait for a character to be pressed.
            Console.ReadKey();

        }
    }
}
