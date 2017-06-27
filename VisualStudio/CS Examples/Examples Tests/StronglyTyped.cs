using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using FiftyOne.Example.Illustration.CSharp.StronglyTyped;

namespace Examples_Tests
{
    [TestClass]
    public class StronglyTyped
    {
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Lite")]
        public void LiteExamples_Strongly_Typed()
        {
            Program.Run(Constants.LITE_PATTERN_V32);
        }
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Premium")]
        public void PremiumExamples_Strongly_Typed()
        {
            Program.Run(Constants.PREMIUM_PATTERN_V32);
        }
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Enterprise")]
        public void EnterpriseExamples_Strongly_Typed()
        {
            Program.Run(Constants.ENTERPRISE_PATTERN_V32);
        }
    }
}
