/**
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright (c) 2015 51Degrees Mobile Experts Limited, 5 Charlotte Close,
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
Reload Data File example demonstrates:
<ol>
    <li> Instantiate the 51Degrees device detection provider with various 
    settings.
    <li> Run device detection in a number of parallel threads.
    <li> Read a file with User-Agent strings line by line and perform device 
    detection for each.
    <li> Reload the dataset in the main thread while the background threads 
    perform device detection.
    <li> Calculate hash of a Match object.
</ol>
<p>
    When creating a new Provider you can specify the following parameters: 
    path to the 51Degrees data file, size of pool, size of cache and a list 
    of properties that the underlying dataset should be initialised with.
    This example demonstrates how to initialise the Provider with a path to 
    the 51Degrees device data file and a comma-separated list of properties.
    <br/><code>
        Provider provider;
        provider = new Provider(deviceDataFile, propertiesToUse);
    </code>
</p>
<p>
    The provider is thread safe and can be used for device detection by as many 
    threads as you need. It is important to remember that each Match object 
    is using a workset from the workset pool, therefore you need to make sure 
    that match is disposed of when the current detection is complete:
    <br /><code>
        using (match = provider.getMatch(line)) 
        {
            // Compute hash for this match.
            hash ^= getHash(match);
            // Update count of processed User-Agent lines.
            recordsProcessed++;
        }
    </code>
    Not disposing of Match will prevent worksets from being returned to the 
    pool. Once the pool is exhausted the program may stall.
</p>
<p>
    Please also keep in mind that you should set the size of the pool to be 
    at lease the same as the number of background threads that will make use 
    of device detection. By default the pool is created with 20 worksets.
</p>
<p>
    The data file reload is designed not to disrupt the device detection 
    process and this example demonstrates this by running the reload multiple 
    times in the main thread while the background threads perform device 
    detection and calculate the hash value for each match.
</p>
<p>
    If you ever need to calculate a hash of the Match object you should use 
    the values of all available properties:
    <br /><code>
        long hash = 0L;
        foreach(var property in provider.getAvailableProperties()) 
        {
            hash += match.getValue(property).GetHashCode();
        }
    </code>
</p>
<p>
    If your environment only allows one thread, then the reload functionality 
    will disrupt the detection process.
</p>
</tutorial>
*/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using FiftyOne.Mobile.Detection.Provider.Interop.Pattern;
using System.IO;
using System.Threading;
using System.Collections.Concurrent;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace FiftyOne.Example.Illustration.CSharp.Reload_Data_File
{
    public class Program
    {
        // Snippet Start

        // A memory-resident data file initialised with specified properties, 
        // a cache and a workset pool.
        static Provider provider;
        // Location of the 51Degrees data file.
        string deviceDataFile;
        // Location of the file containing User-Agent strings.
        string userAgentsFile;
        // A list of comma-separated properties to initialise provider with.
        string propertiesToUse;
        // Indicates how many threads have finished executing.
        static int threadsFinished = 0;
        // Contains hash codes from threads.
        ConcurrentBag<long> cb;

        /// <summary>
        /// Performs dataset reload tests. A number of threads run in the 
        /// background constantly performing device detection while the main 
        /// thread does the data file reloads.
        /// 
        /// The main thread will carry on doing the dataset reloads while at 
        /// least one thread has not finished.
        /// </summary>
        public void Run()
        {
            // Threads that run device detection in the background.
            int numberOfThreads = 4;
            // Contains references to background threads.
            Thread[] threads;

            Console.WriteLine("Starting the Reload Data File Example.");

            provider = new Provider(deviceDataFile, propertiesToUse);
            threads = new Thread[numberOfThreads];
            // Start detection threads.
            for (int i = 0; i < numberOfThreads; i++)
            {
                threads[i] = new Thread(new ThreadStart(threadPayload));
                threads[i].Start();
            }

            // Reload data file until at least one thread is still not done.
            while (threadsFinished < numberOfThreads)
            {
                provider.reloadFromFile();
                Console.WriteLine("Provider reloaded.");
                Thread.Sleep(1000);
            }

            // Wait for all detection threads to complete.
            for (int i = 0; i < numberOfThreads; i++)
            {
                threads[i].Join();
            }

            // Release resources held by the provider.
            provider.Dispose();

            // Perform the test.
            if (!cb.IsEmpty)
            {
                long first, current;
                cb.TryTake(out first);
                while (!cb.IsEmpty)
                {
                    cb.TryTake(out current);
                    Assert.IsTrue(first == current, "Hash values are not equal.");
                }
            }
        }

        /// <summary>
        /// Represents payload for a thread. Function opens the file with 
        /// User-Agent strings and runs device detection for each User-Agent.
        /// Hash is computed for each detection and the XORed with the 
        /// existing hash to verify that reloading the dataset did not affect 
        /// the detection.
        /// </summary>
        public void threadPayload()
        {
            // Local variables.
            long hash = 0L;
            int recordsProcessed = 0;
            Match match;

            // Open file containing User-Agent strings for read.
            using (FileStream fs = File.Open(userAgentsFile, 
                    FileMode.Open, 
                    FileAccess.Read, 
                    FileShare.ReadWrite))
            using (BufferedStream bs = new BufferedStream(fs))
            using (StreamReader sr = new StreamReader(bs))
            {
                // Read next line.
                string line;
                while ((line = sr.ReadLine()) != null)
                {
                    // Performs detection. Disposes of match.
                    using (match = provider.getMatch(line)) 
                    {
                        // Compute hash for this match.
                        hash ^= getHash(match);
                        // Update count of processed User-Agent lines.
                        recordsProcessed++;
                    }
                }
            }
            // When thread is finished increment threadsFinished counter and
            // Report on the progress
            cb.Add(hash);
            Interlocked.Increment(ref threadsFinished);
            Console.WriteLine("Thread complete with hash code: " + hash + 
                              " and records processed: " + recordsProcessed);
        }

        /// <summary>
        /// Computes a hash based on values of the 51Degrees properties that 
        /// were passed as part of the Match object. Only property values are 
        /// used to compute hash.
        /// </summary>
        /// <param name="match">
        /// Object containing 51Degrees device detection results.
        /// </param>
        /// <returns>
        /// Hash value of the provided Match object.
        /// </returns>
        public static long getHash(Match match)
        {
            long hash = 0L;
            foreach(var property in provider.getAvailableProperties()) 
            {
                hash += match.getValue(property).GetHashCode();
            }
            return hash;
        }

        /// <summary>
        /// Main entry point for this program.
        /// </summary>
        /// <param name="args">
        /// 0: location of the 51Degrees device data file. The data file must 
        /// be of version 3.2. Lite data file can be found within the data 
        /// folder.
        /// 1: A file containing User-Agent strings. Does not have to be a CSV 
        /// file. Can have any extension as long as one line contains exactly 
        /// one User-Agent.
        /// 2: A string of comma-separated properties.
        /// All parameters are optional.
        /// </param>
        public static void Main(string[] args)
        {
            string fileName = args.Length > 0 ? args[0] : "../../../../../../data/51Degrees-LiteV3.2.dat";
            string userAgents = args.Length > 1 ? args[1] : "../../../../../../data/20000 User Agents.csv";
            string properties = args.Length > 2 ? args[2] : "IsMobile,BrowserName";
            Program program = new Program(fileName, userAgents, properties);
            program.Run();

            // Report the end of the program and exit.
            Console.WriteLine("Program execution complete. Press Enter to exit.");
            Console.ReadKey();
        }

        /// <summary>
        /// Constructs the new instance of the Program with provided parameters.
        /// </summary>
        /// <param name="deviceDataFile"> 
        /// Location of the 51Degrees data file.
        /// </param>
        /// <param name="userAfentsFile"> 
        /// Location of the file with User-Agent strings.
        /// </param>
        /// <param name="propertiesToUse"> 
        /// Comma-separated string of properties to initialise the Provider 
        /// with.
        /// </param>
        public Program(string deviceDataFile,
                       string userAgentsFile,
                       string propertiesToUse)
        {
            this.deviceDataFile = deviceDataFile;
            this.userAgentsFile = userAgentsFile;
            this.propertiesToUse = propertiesToUse;
            cb = new ConcurrentBag<long>();
        }
        // Snippet End
    }
}
