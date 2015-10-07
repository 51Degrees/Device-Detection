using FiftyOne.Mobile.Detection.Provider.Interop;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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
 * This Source Code Form is "Incompatible With Secondary Licenses", as
 * defined by the Mozilla Public License, v. 2.0.
 ********************************************************************** */

namespace Console_Interop
{
    class Program
    {

        static void Main(string[] args)
        {
            // Initialise the pattern provider with a list of 4 properties.
            using (var pattern = new PatternWrapper(
                new FileInfo("..\\..\\..\\..\\..\\data\\51Degrees-LiteV3.2.dat").FullName,
                    new[] { "Id", "DeviceType", "IsMobile", "ScreenPixelsWidth", "ScreenPixelsHeight" }))
            {

                // Initialise the trie provider with a data file and a list of 4 properties.
                using (var trie = new TrieWrapper(
                    new FileInfo("..\\..\\..\\..\\..\\data\\51Degrees-LiteV3.2.trie").FullName,
                    new[] { "Id", "DeviceType", "IsMobile", "ScreenPixelsWidth", "ScreenPixelsHeight" }))
                {

                    // IMPORTANT: For a full list of properties see: https://51degrees.com/resources/property-dictionary

                    using (var reader = new FileInfo(args.Length > 0 ? args[0] : "..\\..\\..\\..\\..\\data\\20000 User Agents.csv").OpenText())
                    {
                        var start = DateTime.UtcNow;
                        Console.WriteLine("Started -> {0}", start);
                        var line = reader.ReadLine();
                        while (line != null)
                        {
                            using (var patternResults = pattern.Match(line.Trim()))
                            {
                                Output(pattern, (PatternWrapper.MatchResult)patternResults);
                            }
                            using (var trieResults = trie.Match(line.Trim()))
                            {
                                Output(trie, trieResults);
                            }
                            line = reader.ReadLine();
                        }
                        Console.WriteLine("Elapsed Time -> {0} seconds", (DateTime.UtcNow - start).TotalSeconds);
                    }
                }
            }
            Console.ReadKey();
        }

        private static void Output(PatternWrapper wrapper, PatternWrapper.MatchResult results)
        {
            Output((IWrapper)wrapper, (IMatchResult)results);
            Console.WriteLine("Rank -> {0}", results.Rank);
            Console.WriteLine("Difference -> {0}", results.Difference);
            Console.WriteLine("Method -> {0}", results.Method);
        }

        private static void Output(IWrapper wrapper, IMatchResult results)
        {
            Console.WriteLine(wrapper.GetType().Name);
            Console.WriteLine("UserAgent -> {0}", results.UserAgent);
            foreach (var item in wrapper.AvailableProperties)
            {
                Console.WriteLine("{0} -> {1}",
                    item,
                    results[item]);
            }
        }
    }
}
