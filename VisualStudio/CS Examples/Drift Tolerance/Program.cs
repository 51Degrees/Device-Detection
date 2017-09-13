/**
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright (c) 2017 51Degrees Mobile Experts Limited, 5 Charlotte Close,
 * Caversham, Reading, Berkshire, United Kingdom RG4 7BY
 * 
 * This Source Code Form is the subject of the following patent
 * applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
 * Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
 * European Patent Application No. 13192291.6; and
 * United States Patent Application Nos. 14/085,223 and 14/085,301.
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.
 * 
 * If a copy of the MPL was not distributed with this file, You can obtain
 * one at http://mozilla.org/MPL/2.0/.
 * 
 * This Source Code Form is "Incompatible With Secondary Licenses", as
 * defined by the Mozilla Public License, v. 2.0.
 */
/*
<tutorial>
Drift Tolerance example of using 51Degrees device detection. The example shows 
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
    <li>Set the drift tolerance in the provider.
    <p><code>
        provider.setDrift(1);
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

namespace FiftyOne.Example.Illustration.CSharp.DriftTolerance
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
            string properties = "Id,IsMobile";
            // User-Agent string of an Android mobile device.
            string userAgent = "Mozilla/5.0 (Linux; Android 4.2.1; Galaxy " +
                "Nexus Build/JOP40D) AppleWebKit/535.19 (KHTML, like Gecko) " +
                "Chrome/18.0.1025.166 Mobile Safari/535.19";
            // An altered User-Agent which will be matched differently without
            // allowing for drift, as it is not a real User-Agent.
            string alteredUserAgent = "_" + userAgent;

            // Use path to the data file and a list of properties to create 
            // provider.
            Provider provider = new Provider(fileName, properties);

            Console.WriteLine("Starting Drift Tolerance Example.");

            // Carry out a match for the known User-Agent.
            Console.WriteLine(String.Format(
                "\nOriginal User-Agent: '{0}'",
                userAgent));
            detect(provider, userAgent, 0, "True");

            // Carry out a match for the unknown (and non-existent) User-Agent.
            Console.WriteLine(String.Format(
                "\nAltered User-Agent: '{0}'",
                alteredUserAgent));
            detect(provider, alteredUserAgent, 0, "False");

            // Carry out a match for the unknown User-Agent while allowing a
            // character drift of 1 (i.e. the amount the original User-Agent
            // hash been shifted to the right by adding the space).
            Console.WriteLine(String.Format(
                "\nAltered User-Agent: '{0}'",
                alteredUserAgent));
            detect(provider, alteredUserAgent, 1, "True");
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
        /// <param name="drift">
        /// Drift tolerance to allow when matching.
        /// </param>
        /// <param name="expected">
        /// Used for test purposes only. Contains the expected value for the 
        /// IsMobile property.
        /// </param>
        public static void detect(Provider provider,
                          string userAgent,
                          int drift,
                          string expected)
        {
            Match match;
            string DeviceId, IsMobile;

            using (match = provider.getMatchWithTolerances(
                userAgent,
                drift,
                0))
            {
                DeviceId = match.getValue("Id");
                IsMobile = match.getValue("IsMobile");
                Assert.AreEqual(expected, IsMobile);
                if (!String.IsNullOrEmpty(match.getUserAgent()))
                    Console.WriteLine(
                        String.Format("Matched Substrings: '{0}'",
                        match.getUserAgent()));
                Console.WriteLine("   DeviceId: " + DeviceId);
                Console.WriteLine("   IsMobile: " + IsMobile);
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
