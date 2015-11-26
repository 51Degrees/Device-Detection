using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using FiftyOne.Example.Illustration.CSharp.GettingStarted;

namespace Examples_Tests
{
    [TestClass]
    public class GettingStarted
    {
        [TestMethod]
        public void Examples_Getting_Started()
        {
            Program.Run(Constants.LITE_PATTERN_V32);
        }
    }
}
