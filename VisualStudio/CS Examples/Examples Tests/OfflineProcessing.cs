using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Offline_Processing;

namespace Examples_Tests
{
    [TestClass]
    public class OfflineProcessing
    {
        [TestMethod]
        public void Examples_Offline_Processing()
        {
            PrivateType ExampleClass = new PrivateType(typeof(Program));
            ExampleClass.InvokeStatic("run", new object[] { Constants.LITE_PATTERN_V32, Constants.GOOD_USERAGENTS_FILE });
        }
    }
}
