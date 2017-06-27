using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using FiftyOne.Example.Illustration.CSharp.MatchMetrics;

namespace Examples_Tests
{
    [TestClass]
    public class MatchMetrics
    {
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Lite")]
        public void LiteExamples_Match_Metrics()
        {
            Program.Run(Constants.LITE_PATTERN_V32);
        }
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Premium")]
        public void PremiumExamples_Match_Metrics()
        {
            Program.Run(Constants.PREMIUM_PATTERN_V32);
        }
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Enterprise")]
        public void EnterpriseExamples_Match_Metrics()
        {
            Program.Run(Constants.ENTERPRISE_PATTERN_V32);
        }
    }
}
