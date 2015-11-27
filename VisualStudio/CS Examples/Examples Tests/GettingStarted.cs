using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using FiftyOne.Example.Illustration.CSharp.GettingStarted;

namespace Examples_Tests
{
    [TestClass]
    public class GettingStarted
    {
        [TestMethod]
        public void Examples_Getting_Started_Lite()
        {
            Program.Run(Constants.LITE_PATTERN_V32);
        }
        [TestMethod]
        public void Examples_Getting_Started_Premium()
        {
            Program.Run(Constants.PREMIUM_PATTERN_V32);
        }
        [TestMethod]
        public void Examples_Getting_Started_Enterprise()
        {
            Program.Run(Constants.ENTERPRISE_PATTERN_V32);
        }
    }
}
