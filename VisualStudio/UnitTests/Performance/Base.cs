/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited. 
 * Copyright © 2014 51Degrees Mobile Experts Limited, 5 Charlotte Close,
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

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;

namespace FiftyOne.UnitTests.Performance
{
    public abstract class Base : UnitTests.Base
    {
        /// <summary>
        /// Time taken to initialise the data set for the tests.
        /// </summary>
        protected TimeSpan _testInitializeTime;

        /// <summary>
        /// Maximum amount of time to initialise the wrapper.
        /// </summary>
        protected abstract int MaxInitializeTime { get; }

        /// <summary>
        /// Checks that the initialisation time was within tolerances.
        /// </summary>
        protected virtual void InitializeTime()
        {
            Assert.IsTrue(_testInitializeTime.TotalMilliseconds < MaxInitializeTime,
                String.Format("Initialisation time greater than '{0}' ms", MaxInitializeTime));
            Console.WriteLine("{0:0.00}ms", _testInitializeTime.TotalMilliseconds);
        }

        protected Utils.Results BadUserAgentsMulti(int guidanceTime)
        {
            var results = Utils.DetectLoopMultiThreaded(
                _wrapper,
                UserAgentGenerator.GetBadUserAgents(),
                Utils.GetAllProperties,
                RequiredProperties);
            Assert.IsTrue(results.AverageTime.TotalMilliseconds < guidanceTime,
                String.Format("Average time of '{0:0.000}' ms exceeded guidance time of '{1}' ms",
                    results.AverageTime.TotalMilliseconds,
                    guidanceTime));
            return results;
        }

        protected Utils.Results BadUserAgentsSingle(int guidanceTime)
        {
            var results = Utils.DetectLoopSingleThreaded(
                _wrapper,
                UserAgentGenerator.GetBadUserAgents(),
                Utils.GetAllProperties,
                RequiredProperties);
            Assert.IsTrue(results.AverageTime.TotalMilliseconds < guidanceTime,
                String.Format("Average time of '{0:0.000}' ms exceeded guidance time of '{1}' ms",
                    results.AverageTime.TotalMilliseconds,
                    guidanceTime));
            return results;
        }

        protected Utils.Results RandomUserAgentsMulti(int guidanceTime)
        {
            var results = Utils.DetectLoopMultiThreaded(
                _wrapper,
                UserAgentGenerator.GetRandomUserAgents(),
                Utils.GetAllProperties,
                RequiredProperties);
            Assert.IsTrue(results.AverageTime.TotalMilliseconds < guidanceTime,
                String.Format("Average time of '{0:0.000}' ms exceeded guidance time of '{1}' ms",
                    results.AverageTime.TotalMilliseconds,
                    guidanceTime));
            return results;
        }

        protected Utils.Results RandomUserAgentsSingle(int guidanceTime)
        {
            var results = Utils.DetectLoopSingleThreaded(
                _wrapper,
                UserAgentGenerator.GetRandomUserAgents(),
                Utils.GetAllProperties,
                RequiredProperties);
            Assert.IsTrue(results.AverageTime.TotalMilliseconds < guidanceTime,
                String.Format("Average time of '{0:0.000}' ms exceeded guidance time of '{1}' ms",
                    results.AverageTime.TotalMilliseconds,
                    guidanceTime));
            return results;
        }

        protected Utils.Results UniqueUserAgentsMulti(int guidanceTime)
        {
            var results = Utils.DetectLoopMultiThreaded(
                _wrapper,
                UserAgentGenerator.GetUniqueUserAgents(),
                Utils.GetAllProperties,
                RequiredProperties);
            Assert.IsTrue(results.AverageTime.TotalMilliseconds < guidanceTime,
                String.Format("Average time of '{0:0.000}' ms exceeded guidance time of '{1}' ms",
                    results.AverageTime.TotalMilliseconds,
                    guidanceTime));
            return results;
        }

        protected Utils.Results UniqueUserAgentsSingle(int guidanceTime)
        {
            var results = Utils.DetectLoopSingleThreaded(
                _wrapper,
                UserAgentGenerator.GetUniqueUserAgents(),
                Utils.GetAllProperties,
                RequiredProperties);
            Assert.IsTrue(results.AverageTime.TotalMilliseconds < guidanceTime,
                String.Format("Average time of '{0:0.000}' ms exceeded guidance time of '{1}' ms",
                    results.AverageTime.TotalMilliseconds,
                    guidanceTime));
            return results;
        }
    }
}