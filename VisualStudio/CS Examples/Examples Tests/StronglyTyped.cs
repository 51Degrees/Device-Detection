using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using FiftyOne.Example.Illustration.CSharp.StronglyTyped;

namespace Examples_Tests
{
    [TestClass]
    public class StronglyTyped
    {
        [TestMethod]
        public void Examples_Strongly_Typed_Lite()
        {
            Program.Run(Constants.LITE_PATTERN_V32);
        }
        [TestMethod]
        public void Examples_Strongly_Typed_Premium()
        {
            Program.Run(Constants.PREMIUM_PATTERN_V32);
        }
        [TestMethod]
        public void Examples_Strongly_Typed_Enterprise()
        {
            Program.Run(Constants.ENTERPRISE_PATTERN_V32);
        }
    }
}
