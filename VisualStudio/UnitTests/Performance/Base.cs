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

using FiftyOne.Mobile.Detection.Provider.Interop;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Linq;

namespace FiftyOne.UnitTests.Performance
{
    public abstract class Base : UnitTests.Base
    {
        /// <summary>
        /// Time taken to initialise the data set for the tests.
        /// </summary>
        protected TimeSpan _testInitializeTime;

        protected abstract int MaxInitializeTime { get; }

        protected virtual void InitializeTime()
        {
            Assert.IsTrue(_testInitializeTime.TotalMilliseconds < MaxInitializeTime,
                String.Format("Initialisation time greater than '{0}' ms", MaxInitializeTime));
            Console.WriteLine("{0:0.00}ms", _testInitializeTime.TotalMilliseconds);
        }

        protected Utils.Results BadUserAgentsMulti(int maxDetectionTime)
        {
            var results = Utils.DetectLoopMultiThreaded(
                _wrapper,
                UserAgentGenerator.GetBadUserAgents(),
                Utils.DoNothing,
                maxDetectionTime);
            return results;
        }

        protected Utils.Results BadUserAgentsSingle(int maxDetectionTime)
        {
            var results = Utils.DetectLoopSingleThreaded(
                _wrapper,
                UserAgentGenerator.GetBadUserAgents(),
                Utils.DoNothing,
                maxDetectionTime);
            return results;
        }

        protected Utils.Results RandomUserAgentsMulti(int maxDetectionTime)
        {
            var results = Utils.DetectLoopMultiThreaded(
                _wrapper,
                UserAgentGenerator.GetRandomUserAgents(),
                Utils.DoNothing,
                maxDetectionTime);
            return results;
        }

        protected Utils.Results RandomUserAgentsSingle(int maxDetectionTime)
        {
            var results = Utils.DetectLoopSingleThreaded(
                _wrapper,
                UserAgentGenerator.GetRandomUserAgents(),
                Utils.DoNothing,
                maxDetectionTime);
            return results;
        }

        protected Utils.Results UniqueUserAgentsMulti(int maxDetectionTime)
        {
            var results = Utils.DetectLoopMultiThreaded(
                _wrapper,
                UserAgentGenerator.GetUniqueUserAgents(),
                Utils.DoNothing,
                maxDetectionTime);
            return results;
        }

        protected Utils.Results UniqueUserAgentsSingle(int maxDetectionTime)
        {
            var results = Utils.DetectLoopSingleThreaded(
                _wrapper,
                UserAgentGenerator.GetUniqueUserAgents(),
                Utils.DoNothing,
                maxDetectionTime);
            return results;
        }
    }
}
