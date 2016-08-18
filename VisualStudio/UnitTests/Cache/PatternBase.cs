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

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using FiftyOne.Mobile.Detection.Provider.Interop;

namespace FiftyOne.UnitTests.Cache
{
    public abstract class PatternBase : UnitTests.PatternBase
    {
        protected static int _iterations = 10;
        protected static int[] _cacheSizes = { 100, 1000, 20000 };

        protected IWrapper GetFreshWrapper(int cacheSize)
        {
            if (_wrapper != null)
            {
                _wrapper.Dispose();
            }
            return CreateWrapper(cacheSize);
        }

        protected System.TimeSpan[] SingleThreadedPass(int cacheSize)
        {
            var times = new System.TimeSpan[2];

            var userAgents = UserAgentGenerator.GetEnumerable(cacheSize);
            _wrapper = GetFreshWrapper(cacheSize);

            var emptyResults = Utils.DetectLoopSingleThreaded(
                _wrapper,
                userAgents,
                Utils.GetAllProperties,
                RequiredProperties,
                true);

            times[0] = emptyResults.AverageTime;

            Assert.AreEqual(_wrapper.CacheMisses, cacheSize * 2,
                String.Format("Cache misses ({0}) was not equal to cache size" +
                " ({1}) when the cache was empty.",
                _wrapper.CacheMisses, cacheSize));

            var cachedResults = Utils.DetectLoopSingleThreaded(
                _wrapper,
                userAgents,
                Utils.GetAllProperties,
                RequiredProperties,
                true);

            times[1] = cachedResults.AverageTime;

            _wrapper.Dispose();
            return times;
        }

        protected System.TimeSpan[] MultiThreadedPass(int cacheSize)
        {
            var times = new System.TimeSpan[2];

            var userAgents = UserAgentGenerator.GetEnumerable(cacheSize);
            _wrapper = GetFreshWrapper(cacheSize);

            var emptyResults = Utils.DetectLoopMultiThreaded(
                _wrapper,
                userAgents,
                Utils.GetAllProperties,
                RequiredProperties,
                true);

            times[0] = emptyResults.AverageTime;

            Assert.AreEqual(_wrapper.CacheMisses, cacheSize * 2,
                String.Format("Cache misses ({0}) was not equal to cache size" +
                " ({1}) when the cache was empty.",
                _wrapper.CacheMisses, cacheSize));

            var cachedResults = Utils.DetectLoopMultiThreaded(
                _wrapper,
                userAgents,
                Utils.GetAllProperties,
                RequiredProperties,
                true);

            times[1] = cachedResults.AverageTime;
            
            _wrapper.Dispose();
            return times;
        }

        protected void CachePerformanceSingleThreaded(int cacheSize, int iterations)
        {
            var uncachedTimeTotal = new System.TimeSpan(0);
            var cachedTimeTotal = new System.TimeSpan(0);

            for (var i = 0; i < iterations; i++)
            {
                var times = SingleThreadedPass(cacheSize);
                uncachedTimeTotal += times[0];
                cachedTimeTotal += times[1];
            }

            var uncachedTimeAverage =
                new System.TimeSpan(uncachedTimeTotal.Ticks / iterations);

            var cachedTimeAverage =
                new System.TimeSpan(cachedTimeTotal.Ticks / iterations);


            AssertCacheSpeed(cachedTimeAverage, uncachedTimeAverage);

        }

        protected void CachePerformanceMultiThreaded(int cacheSize, int iterations)
        {
            var uncachedTimeTotal = new System.TimeSpan(0);
            var cachedTimeTotal = new System.TimeSpan(0);

            for (var i = 0; i < iterations; i++)
            {
                var times = MultiThreadedPass(cacheSize);
                uncachedTimeTotal += times[0];
                cachedTimeTotal += times[1];
            }

            var uncachedTimeAverage =
                new System.TimeSpan(uncachedTimeTotal.Ticks / iterations);

            var cachedTimeAverage =
                new System.TimeSpan(cachedTimeTotal.Ticks / iterations);


            AssertCacheSpeed(cachedTimeAverage, uncachedTimeAverage);

        }

        private static double tolerance = 1.1;

        private void AssertCacheSpeed(System.TimeSpan cachedTime,
            System.TimeSpan uncachedTime)
        {
            Console.WriteLine("Average time when adding to cache: " +
                uncachedTime.TotalMilliseconds);
            Console.WriteLine("Average time when fetching from cache: " +
                cachedTime.TotalMilliseconds);

            Assert.IsTrue(cachedTime.TotalMilliseconds < uncachedTime.TotalMilliseconds*tolerance,
                String.Format("Average time of '{0:0.000}' ms when fetching " +
                "from the cache exceded time of '{1:0.000}' ms when adding " +
                "to the cache.",
                cachedTime.TotalMilliseconds,
                uncachedTime.TotalMilliseconds));
        }
    }
}