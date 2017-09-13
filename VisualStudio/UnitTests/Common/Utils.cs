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
using System.IO;
using System.Threading;
using System.Threading.Tasks;

namespace FiftyOne.UnitTests
{
    public static class Utils
    {
        public class Memory
        {
            private readonly long StartMemory;

            public long TotalMemory = 0;

            public int MemorySamples = 0;

            internal Memory()
            {
                StartMemory = GC.GetTotalMemory(true);
            }

            public double AverageMemoryUsed
            {
                get { return ((TotalMemory / MemorySamples) - StartMemory) / (double)(1024 * 1024); }
            }

            internal void Reset()
            {
                TotalMemory = 0;
                MemorySamples = 0;
            }
        }

        public class Results
        {
            public readonly DateTime StartTime;

            public int Count = 0;

            public long CheckSum = 0;

            public TimeSpan ElapsedTime
            {
                get { return DateTime.UtcNow - StartTime; }
            }

            public TimeSpan AverageTime
            {
                get { return new TimeSpan(ElapsedTime.Ticks / Count); }
            }

            public Results()
            {
                StartTime = DateTime.UtcNow;
            }
        }

        /// <summary>
        /// Passed a match to perform what ever tests are required.
        /// </summary>
        /// <param name="match">Match of a detection.</param>
        /// <param name="results">The results data for the loop.</param>
        /// <param name="state">State used by the method.</param>
        public delegate void ProcessMatch(Results results, IMatchResult match, object state);

        /// <summary>
        /// In a single thread loops through the useragents in the file
        /// provided perform a match with the data set provided passing
        /// control back to the method provided for further processing.
        /// </summary>
        /// <param name="provider"></param>
        /// <param name="userAgents"></param>
        /// <param name="method"></param>
        /// <param name="state"></param>
        /// <param name="silent"></param>
        /// <returns>Counts for each of the methods used</returns>
        internal static Results DetectLoopSingleThreaded(IWrapper provider, IEnumerable<string> userAgents, ProcessMatch method, object state, bool silent = false)
        {
            var results = new Results();
            foreach (var line in userAgents)
            {
                try
                {
                    using (var match = provider.Match(line.Trim()))
                    {
                        method(results, match, state);
                    }
                }
                catch (AccessViolationException ex)
                {
                    Console.WriteLine(line);
                    Console.WriteLine(ex.Message);
                    Console.WriteLine(ex.StackTrace);
                }
                results.Count++;
            }
            if (!silent)
            {
                ReportTime(results);
            }
            return results;
        }
        
        /// <summary>
        /// Using multiple threads loops through the useragents in the file
        /// provided perform a match with the data set provided passing
        /// control back to the method provided for further processing.
        /// </summary>
        /// <param name="provider"></param>
        /// <param name="userAgents"></param>
        /// <param name="method"></param>
        /// <param name="state"></param>
        /// <param name="silent"></param>
        /// <returns>Counts for each of the methods used</returns>
        internal static Results DetectLoopMultiThreaded(IWrapper provider, IEnumerable<string> userAgents, ProcessMatch method, object state, bool silent = false)
        {
            var results = new Results();
            Parallel.ForEach(userAgents, line =>
            {
                try
                {
                    using (var match = provider.Match(line.Trim()))
                    {
                        method(results, match, state);
                    }
                    Interlocked.Increment(ref results.Count);
                }
                catch(AccessViolationException ex)
                {
                    Console.WriteLine(line);
                    Console.WriteLine(ex.Message);
                    Console.WriteLine(ex.StackTrace);
                }
            });
            if (!silent) {
                ReportTime(results);
            }
            return results;
        }

        internal static void ReportChecksum(Results results)
        {
            Console.WriteLine("Checksum: '{0}'", results.CheckSum);
        }

        internal static void ReportTime(Results results)
        {
            Console.WriteLine("Total of '{0:0.00}'s for '{1}' tests.",
                results.ElapsedTime.TotalSeconds,
                results.Count);
            Console.WriteLine("Average '{0:0.000}'ms per test.",
                results.ElapsedTime.TotalMilliseconds / results.Count);
        }
                
        public static void MonitorMemory(Results results, SortedList<string, List<string>> properties, object state)
        {
            if (results.Count % 1000 == 0)
            {
                Interlocked.Increment(ref ((Memory)state).MemorySamples);
                Interlocked.Add(ref ((Memory)state).TotalMemory, GC.GetTotalMemory(true));
            }
        }

        /// <summary>
        /// Returns gets all the property values and updates the check sum for
        /// the values.
        /// </summary>
        /// <param name="results"></param>
        /// <param name="match"></param>
        /// <param name="state"></param>
        public static void GetAllProperties(Results results, IMatchResult match, object state)
        {
            int checkSum = 0;
            foreach (var propertyName in (IEnumerable<string>)state)
            {
                checkSum += match[propertyName].GetHashCode();
            }
            Interlocked.Add(ref results.CheckSum, checkSum);
        }
        
        internal static void CheckFileExists(string dataFile)
        {
            if (File.Exists(dataFile) == false)
            {
                Assert.Inconclusive(
                    "Data file '{0}' could not be found. " +
                    "See https://51degrees.com/compare-data-options to complete this test.",
                    new FileInfo(dataFile).FullName);
            }
        }
    }
}