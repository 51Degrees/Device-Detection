using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Strongly_Typed;

namespace Examples_Tests
{
    [TestClass]
    public class StronglyTyped
    {
        [TestMethod]
        public void Examples_Strongly_Typed()
        {
            PrivateType ExampleClass = new PrivateType(typeof(Program));
            ExampleClass.InvokeStatic("run", new object[] { Constants.LITE_PATTERN_V32 });
        }
    }
}
