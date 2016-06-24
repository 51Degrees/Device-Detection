using FiftyOne.Mobile.Detection.Provider.Interop;

namespace FiftyOne.UnitTests.Update
{
    public abstract class PatternBase : FiftyOne.UnitTests.Update.Base
    {
        protected override IWrapper CreateWrapper()
        {
            Utils.CheckFileExists(DataFile);
            return new PatternWrapper(DataFile);
        }
    }
}