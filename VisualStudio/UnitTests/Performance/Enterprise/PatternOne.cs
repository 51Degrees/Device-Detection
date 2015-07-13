using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace FiftyOne.UnitTests.Performance.Enterprise
{
    [TestClass]
    public class PatternOne : PatternBase
    {
        protected override int MaxInitializeTime
        {
            get { return 250; }
        }

        protected override string RequiredProperties
        {
            get
            {
                return "IsMobile";
            }
        }

        protected override string DataFile
        {
            get { return Constants.ENTERPRISE_PATTERN_V32; }
        }

        [TestMethod]
        public void EnterpriseV32Pattern_Performance_InitializeTimeOne()
        {
            base.InitializeTime();
        }
        
        [TestMethod]
        public void EnterpriseV32Pattern_Performance_BadUserAgentsMultiOne()
        {
            base.BadUserAgentsMulti(1);
        }

        [TestMethod]
        public void EnterpriseV32Pattern_Performance_BadUserAgentsSingleOne()
        {
            base.BadUserAgentsSingle(1);
        }

        [TestMethod]
        public void EnterpriseV32Pattern_Performance_UniqueUserAgentsMultiOne()
        {
            base.UniqueUserAgentsMulti(1);
        }

        [TestMethod]
        public void EnterpriseV32Pattern_Performance_UniqueUserAgentsSingleOne()
        {
            base.UniqueUserAgentsSingle(1);
        }

        [TestMethod]
        public void EnterpriseV32Pattern_Performance_RandomUserAgentsMultiOne()
        {
            base.RandomUserAgentsMulti(1);
        }

        [TestMethod]
        public void EnterpriseV32Pattern_Performance_RandomUserAgentsSingleOne()
        {
            base.RandomUserAgentsSingle(1);
        }
    }
}
