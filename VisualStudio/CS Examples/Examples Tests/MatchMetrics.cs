using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Match_Metrics;

namespace Examples_Tests
{
    [TestClass]
    public class MatchMetrics
    {
        [TestMethod]
        public void Examples_Match_Metrics()
        {
            PrivateType ExampleClass = new PrivateType(typeof(Program));
            ExampleClass.InvokeStatic("run", new object[] { Constants.LITE_PATTERN_V32 });
        }
    }
}
