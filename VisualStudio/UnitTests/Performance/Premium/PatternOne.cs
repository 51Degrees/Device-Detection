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

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Collections.Generic;

namespace FiftyOne.UnitTests.Performance.Premium
{
    [TestClass]
    public class PatternOne : PatternBase
    {
        private static readonly string[] REQUIRED_PROPERTIES = new string[] { "IsMobile" };

        protected override int MaxInitializeTime
        {
            get { return 800; }
        }

        protected override IEnumerable<string> RequiredProperties
        {
            get
            {
                return REQUIRED_PROPERTIES;
            }
        }

        protected override string DataFile
        {
            get { return Constants.PREMIUM_PATTERN_V32; }
        }

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
        [TestCategory("Premium"), TestCategory("Pattern"), TestCategory("Performance"), TestCategory("Initialize")]
        public void PremiumV32Pattern_Performance_InitializeTimeOne()
        {
            InitializeTime();
        }
        
        [TestMethod]
        [TestCategory("Premium"), TestCategory("Pattern"), TestCategory("Performance"), TestCategory("Multi"), TestCategory("Bad"), TestCategory("One")]
        public void PremiumV32Pattern_Performance_BadUserAgentsMultiOne()
        {
            BadUserAgentsMulti(1);
        }

        [TestMethod]
        [TestCategory("Premium"), TestCategory("Pattern"), TestCategory("Performance"), TestCategory("Single"), TestCategory("Bad"), TestCategory("One")]
        public void PremiumV32Pattern_Performance_BadUserAgentsSingleOne()
        {
            BadUserAgentsSingle(3);
        }

        [TestMethod]
        [TestCategory("Premium"), TestCategory("Pattern"), TestCategory("Performance"), TestCategory("Multi"), TestCategory("Unique"), TestCategory("One")]
        public void PremiumV32Pattern_Performance_UniqueUserAgentsMultiOne()
        {
            UniqueUserAgentsMulti(1);
        }

        [TestMethod]
        [TestCategory("Premium"), TestCategory("Pattern"), TestCategory("Performance"), TestCategory("Single"), TestCategory("Unique"), TestCategory("One")]
        public void PremiumV32Pattern_Performance_UniqueUserAgentsSingleOne()
        {
            UniqueUserAgentsSingle(1);
        }

        [TestMethod]
        [TestCategory("Premium"), TestCategory("Pattern"), TestCategory("Performance"), TestCategory("Multi"), TestCategory("Random"), TestCategory("One")]
        public void PremiumV32Pattern_Performance_RandomUserAgentsMultiOne()
        {
            RandomUserAgentsMulti(1);
        }

        [TestMethod]
        [TestCategory("Premium"), TestCategory("Pattern"), TestCategory("Performance"), TestCategory("Single"), TestCategory("Random"), TestCategory("One")]
        public void PremiumV32Pattern_Performance_RandomUserAgentsSingleOne()
        {
            RandomUserAgentsSingle(1);
        }
    }
}
