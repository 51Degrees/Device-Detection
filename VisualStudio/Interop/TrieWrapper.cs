/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited. 
 * Copyright © 2014 51Degrees Mobile Experts Limited, 5 Charlotte Close,
 * Caversham, Reading, Berkshire, United Kingdom RG4 7BY
 * 
 * This Source Code Form is the subject of the following patent 
 * applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
 * Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY: 
 * European Patent Application No. 13192291.6; and
 * United States Patent Application Nos. 14/085,223 and 14/085,301.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.
 * 
 * If a copy of the MPL was not distributed with this file, You can obtain
 * one at http://mozilla.org/MPL/2.0/.
 * 
 * This Source Code Form is “Incompatible With Secondary Licenses”, as
 * defined by the Mozilla Public License, v. 2.0.
 * ********************************************************************* */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Threading;
using System.IO;
using System.Collections.Specialized;

namespace FiftyOne.Mobile.Detection.Provider.Interop
{
    /// <summary>
    /// Class used to wrap functions exposed by the tree matching C
    /// DLL.
    /// </summary>
    public class TrieWrapper : IWrapper
    {
        #region DLL Imports

        [DllImport("FiftyOne.Mobile.Detection.Provider.Trie.dll", 
            CallingConvention = CallingConvention.Cdecl, 
            CharSet = CharSet.Ansi)]
        private static extern void Init(String fileName, String properties);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Trie.dll", 
            CallingConvention = CallingConvention.Cdecl)]
        private static extern void Destroy();

        [DllImport("FiftyOne.Mobile.Detection.Provider.Trie.dll",
            CallingConvention = CallingConvention.Cdecl,
            CharSet = CharSet.Ansi)]
        private static extern int GetPropertiesCSV(String userAgent, StringBuilder result, Int32 resultLength);

        #endregion

        #region Constants

        /// <summary>
        /// The initial capacity of the buffer used to store the resulting
        /// properties.
        /// </summary>
        private const int DEFAULT_CAPACITY = 8096;

        /// <summary>
        /// The maximum size of the buffer. Anything more than this is too much.
        /// </summary>
        private const int MAX_CAPACITY = DEFAULT_CAPACITY * 10;

        #endregion

        #region Fields

        /// <summary>
        /// The number of instances of the wrapper.
        /// </summary>
        private static int _instanceCount = 0;

        /// <summary>
        /// Used to lock initialise and destroy calls.
        /// </summary>
        private static object _lock = new Object();

        #endregion

        #region Constructor and Destructor

        /// <summary>
        /// Construct the wrapper creating a workset for each CPU available.
        /// </summary>
        /// <param name="fileName">The full path to the file containing device data.</param>
        public TrieWrapper(string fileName) : this(fileName, String.Empty) { }

        /// <summary>
        /// Construct the wrapper creating a workset for each CPU available.
        /// </summary>
        /// <param name="fileName">The full path to the file containing device data.</param>
        /// <param name="properties">Array of properties to include in the results.</param>
        public TrieWrapper(string fileName, string[] properties) : this(fileName, String.Join(",", properties)) {}

        /// <summary>
        /// Construct the wrapper creating a workset for each CPU available.
        /// </summary>
        /// <param name="fileName">The full path to the file containing device data.</param>
        /// <param name="properties">Comma seperated list of properties to include in the results.</param>
        public TrieWrapper(string fileName, string properties)
        {
            lock(_lock)
            {
                if (_instanceCount == 0)
                {
                    var info = new FileInfo(fileName);
                    if (info.Exists == false)
                        throw new ArgumentException(String.Format("File '{0}' can not be found.", info.FullName), "fileName");
                    Init(info.FullName, properties);
                    _instanceCount++;
                }
                else
                {
                    throw new Exception("Only one instance of the TrieWrapper can be initialised.");
                }
            }
        }

        #endregion

        #region Public Methods
        
        public IList<string> AvailableProperties
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns the properties for the userAgent as a CSV string.
        /// </summary>
        /// <param name="userAgent">The user agent of the device being searched for.</param>
        /// <returns>A | seperated list of properties.</returns>
        public StringBuilder GetPropertiesAsCSV(string userAgent)
        {
            int length = 0;
            var result = new StringBuilder(DEFAULT_CAPACITY);
            do
            {
                // Call the DLL to get a CSV string of properties.
                length = GetPropertiesCSV(
                    userAgent,
                    result,
                    result.Capacity);

                // If the StringBuilder is too small then increase the size
                // and try again.
                if (length < 0)
                {
                    result.Capacity += DEFAULT_CAPACITY;
                    // If the capacity is now so large something has gone
                    // wrong exit the loop.
                    if (result.Capacity > MAX_CAPACITY)
                        break;
                }
            }
            while (length < 0);

            return result;
        }

        /// <summary>
        /// Returns a list of properties and values for the userAgent provided.
        /// </summary>
        /// <param name="userAgent">The useragent to search for.</param>
        /// <returns>A list of properties.</returns>
        public IMatchResult Match(string userAgent)
        {
            throw new NotImplementedException();
        }

        public IMatchResult Match(NameValueCollection headers)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Frees all the resource created in the DLL.
        /// </summary>
        public void Dispose()
        {
            lock (_lock)
            {
                if (_instanceCount > 0)
                {
                    Destroy();
                    _instanceCount--;
                }
            }
        }

        #endregion
    }
}