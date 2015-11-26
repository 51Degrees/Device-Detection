using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using FiftyOne.Example.Illustration.CSharp.MatchMetrics;

namespace Examples_Tests
{
    [TestClass]
    public class MatchMetrics
    {
        [TestMethod]
        public void Examples_Match_Metrics()
        {
            Program.Run(Constants.LITE_PATTERN_V32);
        }
    }
}
