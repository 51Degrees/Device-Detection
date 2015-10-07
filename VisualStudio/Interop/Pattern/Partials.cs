using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace FiftyOne.Mobile.Detection.Provider.Interop.Pattern
{
    public partial class Match : IMatchResult
    {
        IList<string> IMatchResult.getValues(string propertyName)
        {
            return (IList<string>)getValues(propertyName);
        }

        IList<string> IMatchResult.getValues(int propertyIndex)
        {
            return (IList<string>)getValues(propertyIndex);
        }

        public string this[string propertyName]
        {
            get 
            { 
                using(var values = getValues(propertyName))
                {
                    return String.Join("|", values); 
                }
            }
        }

        public string DeviceId
        {
            get { return getDeviceId(); }
        }

        public int Rank
        {
            get { return getRank(); }
        }

        public int Difference
        {
            get { return getDifference(); }
        }

        public int Method
        {
            get { return getMethod(); }
        }

        public string UserAgent
        {
            get
            {
                var userAgent = getUserAgent();
                return String.IsNullOrEmpty(userAgent) ? null : userAgent;
            }
        }
    }
}
