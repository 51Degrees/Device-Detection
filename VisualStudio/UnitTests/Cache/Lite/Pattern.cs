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
using System.Collections.Generic;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace FiftyOne.UnitTests.Cache.Lite
{
    [TestClass]
    public class Pattern : PatternBase
    {
        private static int _iterations = 100;

        private static readonly string[] REQUIRED_PROPERTIES = new string[] { "IsMobile" };

        protected override string DataFile
        {
            get { return Constants.LITE_PATTERN_V32; }
        }

        protected override IEnumerable<string> RequiredProperties
        {
            get
            {
                return REQUIRED_PROPERTIES;
            }
        }

        [TestMethod]
        [TestCategory("Lite"), TestCategory("Pattern"), TestCategory("Cache")]
        public void LiteV32Pattern_Cache_LargeCacheSingle()
        {
            CachePerformanceSingleThreaded(20000, _iterations);
        }

        [TestMethod]
        [TestCategory("Lite"), TestCategory("Pattern"), TestCategory("Cache")]
        public void LiteV32Pattern_Cache_MediumCacheSingle()
        {
            CachePerformanceSingleThreaded(1000, _iterations);
        }

        [TestMethod]
        [TestCategory("Lite"), TestCategory("Pattern"), TestCategory("Cache")]
        public void LiteV32Pattern_Cache_SmallCacheSingle()
        {
            CachePerformanceSingleThreaded(100, _iterations);
        }

        [TestMethod]
        [TestCategory("Lite"), TestCategory("Pattern"), TestCategory("Cache")]
        public void LiteV32Pattern_Cache_LargeCacheMulti()
        {
            CachePerformanceMultiThreaded(20000, _iterations);
        }

        [TestMethod]
        [TestCategory("Lite"), TestCategory("Pattern"), TestCategory("Cache")]
        public void LiteV32Pattern_Cache_MediumCacheMulti()
        {
            CachePerformanceMultiThreaded(1000, _iterations);
        }

        [TestMethod]
        [TestCategory("Lite"), TestCategory("Pattern"), TestCategory("Cache")]
        public void LiteV32Pattern_Cache_SmallCacheMulti()
        {
            CachePerformanceMultiThreaded(100, _iterations);
        }
    }
}
