using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Getting_Started;

namespace Examples_Tests
{
    [TestClass]
    public class GettingStarted
    {
        [TestMethod]
        public void Examples_Getting_Started()
        {
            PrivateType ExampleClass = new PrivateType(typeof(Program));
            ExampleClass.InvokeStatic("run", new object[] { Constants.LITE_PATTERN_V32 });
        }
    }
}
