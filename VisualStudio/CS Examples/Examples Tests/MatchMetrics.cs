using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using FiftyOne.Example.Illustration.CSharp.MatchMetrics;

namespace Examples_Tests
{
    [TestClass]
    public class MatchMetrics
    {
        [TestMethod]
        public void Examples_Match_Metrics_Lite()
        {
            Program.Run(Constants.LITE_PATTERN_V32);
        }
        [TestMethod]
        public void Examples_Match_Metrics_Premium()
        {
            Program.Run(Constants.PREMIUM_PATTERN_V32);
        }
        [TestMethod]
        public void Examples_Match_Metrics_Enterprise()
        {
            Program.Run(Constants.ENTERPRISE_PATTERN_V32);
        }
    }
}
