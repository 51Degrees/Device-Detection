/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited. 
 * Copyright 2015 51Degrees Mobile Experts Limited, 5 Charlotte Close,
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
using System.Text;
using System.Runtime.InteropServices;
using System.Threading;
using System.IO;
using System.Collections.Specialized;
using System.Diagnostics;

namespace FiftyOne.Mobile.Detection.Provider.Interop
{
    /// <summary>
    /// Class used to wrap functions exposed by the tree matching C
    /// DLL.
    /// </summary>
    public class TrieWrapper : IWrapper
    {
        #region Fields

        /// <summary>
        /// The number of instances of the wrapper.
        /// </summary>
        private static int _instanceCount = 0;

        /// <summary>
        /// Used to lock initialise and destroy calls.
        /// </summary>
        private static object _lock = new Object();

        /// <summary>
        /// Collection of property names to indexes.
        /// </summary>
        internal readonly SortedList<string, int> PropertyIndexes = new SortedList<string, int>();

        /// <summary>
        /// The name of the file used to create the current 
        /// single underlying provider.
        /// </summary>
        private static string _fileName;

        /// <summary>
        /// Set to true when dispose has run for the wrapper
        /// as sometimes the finaliser still runs even when
        /// requested not to.
        /// </summary>
        private bool _disposed = false;

        /// <summary>
        /// Provider used by the wrapper.
        /// </summary>
        private readonly Trie.Provider _provider;

        #endregion

        #region Constructor and Destructor

        /// <summary>
        /// Construct the wrapper.
        /// </summary>
        /// <param name="fileName">The full path to the file containing device data.</param>
        public TrieWrapper(string fileName) : this(fileName, String.Empty) { }

        /// <summary>
        /// Construct the wrapper.
        /// </summary>
        /// <param name="fileName">The full path to the file containing device data.</param>
        /// <param name="properties">Array of properties to include in the results.</param>
        public TrieWrapper(string fileName, string[] properties) : this(fileName, String.Join(",", properties)) { }

        /// <summary>
        /// Construct the wrapper.
        /// </summary>
        /// <param name="fileName">The full path to the file containing device data.</param>
        /// <param name="properties">Comma seperated list of properties to include in the results.</param>
        public TrieWrapper(string fileName, string properties)
        {
            _provider = new Trie.Provider(fileName, properties);
            _fileName = fileName;
        }
        
        /// <summary>
        /// Construct the wrapper.
        /// Also validates the memory calculation.
        /// </summary>
        /// <param name="fileName">Path to the data set file.</param>
        /// <param name="properties">Comma separated list of properties to include in the results</param>
        /// <param name="validate">Set to true to validate the memory calculation.</param>
        public TrieWrapper(string fileName, string properties, bool validate)
        {
            _provider = new Trie.Provider(fileName, properties, validate);
            _fileName = fileName;
        }

        /// <summary>
        /// Ensure the memory used by trie has been freed.
        /// </summary>
        ~TrieWrapper()
        {
            Disposing(false);
        }

        /// <summary>
        /// When disposed of correctly ensures all memory is freed.
        /// </summary>
        public void Dispose()
        {
            Disposing(true);
            GC.SuppressFinalize(true);
        }

        /// <summary>
        /// If the instance count is zero disposes of the memory.
        /// </summary>
        /// <param name="disposing"></param>
        protected virtual void Disposing(bool disposing)
        {
            lock (_lock)
            {
                if (_disposed == false)
                {
                    if (_instanceCount == 1 &&
                        _fileName != null)
                    {
                        // Clear down any static data and free memory.
                        HttpHeaders.Clear();
                        PropertyIndexes.Clear();
                        _fileName = null;
                        _provider.Dispose();
                        _disposed = true;

                        Debug.WriteLine("Freed Trie Data");
                    }
                    _instanceCount--;
                    _disposed = true;
                }
            }
        }

        #endregion

        #region Public Methods

        /// <summary>
        /// A list of the http headers that the wrapper can use for detection.
        /// </summary>
        public List<string> HttpHeaders
        {
            get
            {
                if (_httpHeaders == null)
                {
                    lock (this)
                    {
                        if (_httpHeaders == null)
                        {
                            _httpHeaders = new List<string>(_provider.getHttpHeaders());
                        }
                    }
                }
                return _httpHeaders;
            }
        }
        private List<string> _httpHeaders;


        /// <summary>
        /// A list of properties available from the provider.
        /// </summary>
        public IList<string> AvailableProperties
        {
            get
            {
                if (_availableProperties == null)
                {
                    lock (this)
                    {
                        if (_availableProperties == null)
                        {
                            _availableProperties = new List<string>(_provider.getAvailableProperties());
                        }
                    }
                }
                return _availableProperties;
            }
        }
        private List<string> _availableProperties;


        /// <summary>
        /// Returns a list of properties and values for the userAgent provided.
        /// </summary>
        /// <param name="userAgent">The useragent to search for.</param>
        /// <returns>A list of properties.</returns>
        public IMatchResult Match(string userAgent)
        {
            return (IMatchResult)_provider.getMatch(userAgent == null ? String.Empty : userAgent);
        }

        public IMatchResult Match(NameValueCollection headers)
        {
            using (var mappedHeaders = new Trie.MapStringString())
            {
                foreach (var header in HttpHeaders)
                {
                    var value = headers[header];
                    if (String.IsNullOrEmpty(value) == false)
                    {
                        mappedHeaders.Add(header, value);
                    }
                }
                return (IMatchResult)_provider.getMatch(mappedHeaders);
            }
        }

        /// <summary>
        /// Returns a match result for the device id provided. The device id
        /// would have been returned from a previous matches DeviceId 
        /// property.
        /// </summary>
        /// <param name="deviceId"></param>
        /// <returns></returns>
        public IMatchResult MatchForDeviceId(string deviceId)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Finds the profiles associated with the property name and value 
        /// provided.
        /// </summary>
        /// <param name="propertyName"></param>
        /// <param name="valueName"></param>
        /// <returns></returns>
        public Pattern.Profiles FindProfiles(string propertyName, string valueName)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Reloads the original file path and associated configuration. Used
        /// to refresh the active data set being used at runtime after the
        /// wrapper has been created.
        /// </summary>
        public void ReloadFromFile()
        {
            _provider.reloadFromFile();
        }

        /// <summary>
        /// Reads the data file at the original file path into memory and
        /// uses the reload from memory function to reload the data set
        /// from that memory location.
        /// </summary>
        public void ReloadFromMemory()
        {
            byte[] bytes = File.ReadAllBytes(_fileName);
            _provider.reloadFromMemory(bytes.ToString(), bytes.GetLength(0));
        }

        /// <summary>
        /// Returns the number of times the cache fetch function has found what
        /// it is looking for.
        /// </summary>
        public int CacheHits
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        /// <summary>
        /// Returns the number of times the cache fetch function has not found
        /// what it is looking for. The cache fetch function is called a second
        /// time to insert a value that was not found.
        /// </summary>
        public int CacheMisses
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        #endregion
    }
}