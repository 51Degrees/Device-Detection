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

using FiftyOne.Mobile.Detection.Provider.Interop;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace FiftyOne.UnitTests.HttpHeaders
{
    public abstract class Base : UnitTests.Base
    {
        private const int TEST_ITERATIONS = 20000;

        internal class Validation : Dictionary<string, Regex>
        {
            public void Add(string property, string pattern)
            {
                Add(property, new Regex(pattern, RegexOptions.Compiled));
            }
        }

        internal Utils.Results Process(string userAgentPattern, string devicePattern, Validation state)
        {
            var results = new FiftyOne.UnitTests.Utils.Results();
            var random = new Random(0);
            var httpHeaders = _wrapper.HttpHeaders.Where(i => i.Equals("User-Agent") == false).ToArray();

            // Loop through setting 2 user agent headers.
            var userAgentIterator = UserAgentGenerator.GetEnumerable(TEST_ITERATIONS, userAgentPattern).GetEnumerator();
            var deviceIterator = UserAgentGenerator.GetEnumerable(TEST_ITERATIONS, devicePattern).GetEnumerator();
            while(userAgentIterator.MoveNext() &&
                deviceIterator.MoveNext())
            {
                var headers = new NameValueCollection();
                switch(random.Next(3))
                {
                    case 0:
                        // Capitialise HTTP headers.
                        headers.Add(httpHeaders[random.Next(httpHeaders.Length)].ToUpperInvariant(), deviceIterator.Current);
                        headers.Add("User-Agent".ToUpperInvariant(), userAgentIterator.Current);
                        break;
                    case 1:
                        // Lower HTTP headers.
                        headers.Add(httpHeaders[random.Next(httpHeaders.Length)].ToLowerInvariant(), deviceIterator.Current);
                        headers.Add("User-Agent".ToLowerInvariant(), userAgentIterator.Current);
                        break;
                    default:
                        // Use standard unaltered formats.
                        headers.Add(httpHeaders[random.Next(httpHeaders.Length)], deviceIterator.Current);
                        headers.Add("User-Agent", userAgentIterator.Current);
                        break;
                }

                using (var matchResult = _wrapper.Match(headers))
                {
                    Validate(headers, matchResult, state);
                }
            }

            return results;
        }

        private static void Validate(NameValueCollection httpHeaders, IMatchResult matchResult, Validation validation)
        {
            foreach (var test in validation)
            {
                var value = matchResult[test.Key];
                if (test.Value.IsMatch(value) == false)
                {
                    var message = new StringBuilder();
                    message.AppendFormat(
                        "HttpHeader test failed for Property '{0}' and test '{1}' with result '{2}'.\r\n",
                        test.Key,
                        test.Value,
                        value);
                    for(int i = 0; i < httpHeaders.Count; i++)
                    {
                        message.AppendFormat(
                            "{0}-{1} {2}\r\n",
                            i,
                            httpHeaders.GetKey(i),
                            httpHeaders.GetValues(i).FirstOrDefault());
                    }
                    Assert.Fail(message.ToString());
                }
            }
        }
    }
}
