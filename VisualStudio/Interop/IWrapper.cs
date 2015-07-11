using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;

namespace FiftyOne.Mobile.Detection.Provider.Interop
{
    public interface IWrapper : IDisposable
    {
        /// <summary>
        /// Returns a list of properties and values for the user agent
        /// provided.
        /// </summary>
        /// <param name="userAgent"></param>
        /// <returns></returns>
        SortedList<string, List<string>> GetProperties(string userAgent);

        /// <summary>
        /// Returns a list of properties and values for the HTTP headers
        /// provided.
        /// </summary>
        /// <param name="headers"></param>
        /// <returns></returns>
        SortedList<string, List<string>> GetProperties(NameValueCollection headers);
    }
}
