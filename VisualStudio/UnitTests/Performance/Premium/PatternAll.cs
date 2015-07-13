using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace FiftyOne.UnitTests.Performance.Premium
{
    [TestClass]
    public class PatternAll : PatternBase
    {
        protected override int MaxInitializeTime
        {
            get { return 250; }
        }

        protected override string DataFile
        {
            get { return Constants.PREMIUM_PATTERN_V32; }
        }

        [TestMethod]
        public void PremiumV32Pattern_Performance_InitializeTimeAll()
        {
            base.InitializeTime();
        }
        
        [TestMethod]
        public void PremiumV32Pattern_Performance_BadUserAgentsMultiAll()
        {
            base.BadUserAgentsMulti(1);
        }

        [TestMethod]
        public void PremiumV32Pattern_Performance_BadUserAgentsSingleAll()
        {
            base.BadUserAgentsSingle(1);
        }

        [TestMethod]
        public void PremiumV32Pattern_Performance_UniqueUserAgentsMultiAll()
        {
            base.UniqueUserAgentsMulti(1);
        }

        [TestMethod]
        public void PremiumV32Pattern_Performance_UniqueUserAgentsSingleAll()
        {
            base.UniqueUserAgentsSingle(1);
        }

        [TestMethod]
        public void PremiumV32Pattern_Performance_RandomUserAgentsMultiAll()
        {
            base.RandomUserAgentsMulti(1);
        }

        [TestMethod]
        public void PremiumV32Pattern_Performance_RandomUserAgentsSingleAll()
        {
            base.RandomUserAgentsSingle(1);
        }
    }
}
