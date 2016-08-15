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

namespace FiftyOne.UnitTests.Cache.Premium
{
    [TestClass]
    public class Pattern : PatternBase
    {
        private static readonly string[] REQUIRED_PROPERTIES = new string[] { "IsMobile" };

        protected override string DataFile
        {
            get { return Constants.PREMIUM_PATTERN_V32; }
        }

        protected override IEnumerable<string> RequiredProperties
        {
            get
            {
                return REQUIRED_PROPERTIES;
            }
        }

        [TestMethod]
        [TestCategory("Premium"), TestCategory("Pattern"), TestCategory("Cache")]
        public void PremiumV32Pattern_Cache_LargeCacheSingle()
        {
            CachePerformanceSingleThreaded(_cacheSizesSingle[2], _iterations);
        }

        [TestMethod]
        [TestCategory("Premium"), TestCategory("Pattern"), TestCategory("Cache")]
        public void PremiumV32Pattern_Cache_MediumCacheSingle()
        {
            CachePerformanceSingleThreaded(_cacheSizesSingle[2], _iterations);
        }

        [TestMethod]
        [TestCategory("Premium"), TestCategory("Pattern"), TestCategory("Cache")]
        public void PremiumV32Pattern_Cache_SmallCacheSingle()
        {
            CachePerformanceSingleThreaded(_cacheSizesSingle[0], _iterations);
        }

        [TestMethod]
        [TestCategory("Premium"), TestCategory("Pattern"), TestCategory("Cache")]
        public void PremiumV32Pattern_Cache_LargeCacheMulti()
        {
            CachePerformanceMultiThreaded(_cacheSizesMulti[2], _iterations);
        }

        [TestMethod]
        [TestCategory("Premium"), TestCategory("Pattern"), TestCategory("Cache")]
        public void PremiumV32Pattern_Cache_MediumCacheMulti()
        {
            CachePerformanceMultiThreaded(_cacheSizesMulti[2], _iterations);
        }

        [TestMethod]
        [TestCategory("Premium"), TestCategory("Pattern"), TestCategory("Cache")]
        public void PremiumV32Pattern_Cache_SmallCacheMulti()
        {
            CachePerformanceMultiThreaded(_cacheSizesMulti[0], _iterations);
        }
    }
}
