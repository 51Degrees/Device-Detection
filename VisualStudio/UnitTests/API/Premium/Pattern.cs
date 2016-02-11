/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited. 
 * Copyright 2015 51Degrees Mobile Experts Limited, 5 Charlotte Close,
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
 * This Source Code Form is “Incompatible With Secondary Licenses”, as
 * defined by the Mozilla Public License, v. 2.0.
 * ********************************************************************* */

using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using FiftyOne.UnitTests;
using System.Collections.Specialized;

namespace UnitTests.API.Enterprise
{
    [TestClass]
    public class Pattern : PatternBase
    {
        [TestInitialize]
        public void Initialise()
        {
            if (_wrapper == null) { _wrapper = CreateWrapper(); }
        }

        [TestCleanup]
        public void CleanUp()
        {
            Dispose();
        }

        [TestMethod]
        public void PatternPatternAPI_NullUserAgent()
        {
            using (var result = _wrapper.Match((string)null))
            {
                Console.WriteLine(result.ToString());
            }
        }

        [TestMethod]
        public void PatternPatternAPI_EmptyUserAgent()
        {
            using (var result = _wrapper.Match(String.Empty))
            {
                Console.WriteLine(result.ToString());
            }
        }

        [TestMethod]
        public void PatternPatternAPI_LongUserAgent()
        {
            var userAgent = String.Join(" ", UserAgentGenerator.GetEnumerable(10, 10));
            using (var result = _wrapper.Match(userAgent))
            {
                Console.WriteLine(result.ToString());
            }
        }

        [TestMethod]
        public void PremiumPatternAPI_HttpHeaders()
        {
            var headers = new NameValueCollection();
            foreach (var header in _wrapper.HttpHeaders)
            {
                headers.Add(header, UserAgentGenerator.GetRandomUserAgent(0));
            }
            using (var result = _wrapper.Match(headers))
            {
                foreach (var property in _wrapper.AvailableProperties)
                {
                    Console.WriteLine("{0}: {1}", property, result[property]);
                }
            }
        }

        [TestMethod]
        public void PremiumPatternAPI_FindProfiles()
        {
            string[] properties = new string[3] { "IsTablet", "BrowserName", "HardwareVendor" };
            string[,] values = new string[3, 2] { { "True", "False" }, { "Firefox", "Chrome" }, { "Samsung", "Dell" } };
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    Console.WriteLine("Testing " + properties[i] + " " + values[i, j]);
                    var profiles = _wrapper.FindProfiles(properties[i], values[i, j]);
                    for (int k = 0; k < profiles.getCount(); k++)
                    {
                        Assert.IsNotNull(profiles.getProfileId(k));
                        Assert.IsTrue(profiles.getProfileIndex(k) >= 0);
                        Assert.IsTrue(profiles.getProfileId(k) >= 0);
                    }
                }
            }
        }

        [TestMethod]
        public void PremiumPatternAPI_FindProfilesInvalidProperty()
        {
            var profiles = _wrapper.FindProfiles("NOTAPROPERTY", "True");
            Assert.AreEqual(profiles.getCount(), 0);
        }

        [TestMethod]
        public void PremiumPatternAPI_FindProfilesInvalidValue()
        {
            foreach (var propertyName in _wrapper.AvailableProperties)
            {
                var profiles = _wrapper.FindProfiles(propertyName, "NOTAVALUE");
                Assert.AreEqual(profiles.getCount(), 0);
            }
        }

        [TestMethod]
        public void PremiumPatternAPI_FindProfilesOverload()
        {
            var profiles = _wrapper.FindProfiles("PlatformName", "Android");
            string[] androidVersions = { "4.4.4", "5.1" };
            foreach (string valueName in androidVersions)
            {
                Console.WriteLine("Testing Android " + valueName);
                profiles = _wrapper.FindProfiles("PlatformVersion", valueName);
                Assert.IsTrue(profiles.getCount() > 0);
            }
            profiles = _wrapper.FindProfiles("PlatformName", "Windows");
            string[] windowsVersions = { "10", "8.1", "7" };
            foreach (string valueName in windowsVersions)
            {
                Console.WriteLine("Testing Windows " + valueName);
                profiles = _wrapper.FindProfiles("PlatformVersion", valueName);
                Assert.IsTrue(profiles.getCount() > 0);
            }
            profiles = _wrapper.FindProfiles("PlatformName", "OSX");
            string[] osxVersions = { "10.11", "10.10", "10.9" };
            foreach (string valueName in osxVersions)
            {
                Console.WriteLine("Testing OSX " + valueName);
                profiles = _wrapper.FindProfiles("PlatformVersion", valueName);
                Assert.IsTrue(profiles.getCount() > 0);
            }
        }

        protected override string DataFile
        {
            get { return Constants.PREMIUM_PATTERN_V32; }
        }
    }
}
