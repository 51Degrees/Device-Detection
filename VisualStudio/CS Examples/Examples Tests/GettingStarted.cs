using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using FiftyOne.Example.Illustration.CSharp.GettingStarted;

namespace Examples_Tests
{
    [TestClass]
    public class GettingStarted
    {
        [TestMethod]
        public void LiteExamples_Getting_Started()
        {
            Program.Run(Constants.LITE_PATTERN_V32);
        }
        [TestMethod]
        public void PremiumExamples_Getting_Started()
        {
            Program.Run(Constants.PREMIUM_PATTERN_V32);
        }
        [TestMethod]
        public void EnterpriseExamples_Getting_Started()
        {
            Program.Run(Constants.ENTERPRISE_PATTERN_V32);
        }
    }
}
