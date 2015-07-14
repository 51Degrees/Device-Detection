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
using System.Text;
using System.Runtime.InteropServices;
using System.Threading;
using System.IO;
using System.Collections.Specialized;
using System.Diagnostics;

namespace FiftyOne.Mobile.Detection.Provider.Interop
{
    /// <summary>
    /// Class used to wrap functions exposed by the pattern matching C
    /// DLL.
    /// </summary>
    public class PatternWrapper : IWrapper
    {
        #region Classes

        internal class Provider : IDisposable
        {
            internal IntPtr DataSetPointer;

            internal IntPtr CachePointer;

            internal IntPtr PoolPointer;

            internal readonly AutoResetEvent AllWorksetsReleased = new AutoResetEvent(true);

            internal int AllocatedWorksets = 0;

            internal readonly SortedList<string, int> PropertyIndexes = new SortedList<string, int>();

            /// <summary>
            /// Construct the data set from the file provided.
            /// </summary>
            /// <param name="fileName">Path to the data set file.</param>
            /// <param name="properties">Array of properties to include in the results.</param>
            public Provider(string fileName, string[] properties) 
                : this(fileName, String.Join(",", properties)) {}

            /// <summary>
            /// Construct the wrapper creating a workset for each CPU available.
            /// </summary>
            /// <param name="fileName">Path to the data set file.</param>
            /// <param name="properties">Comma seperated list of properties to include in the results.
            /// <param name="size">The size of the cache to be used with the wrapper</param>
            /// </param>
            public Provider(string fileName, string properties, int size = 0)
            {
                var file = new FileInfo(fileName);
                if (file.Exists == false)
                {
                    throw new ArgumentException(String.Format(
                        "File '{0}' does not exist", 
                        fileName), 
                        "fileName");
                }
                DataSetPointer = InitWithPropertyString(file.FullName, properties);
                if (IntPtr.Zero == DataSetPointer)
                {
                    throw new Exception(String.Format(
                        "Pattern data set initialisation failed with status code '{0}' for file '{1}'.",
                        Marshal.GetLastWin32Error(),
                        file.FullName));
                }
                CachePointer = size > 0 ? ResultsetCacheCreate(DataSetPointer, size) : IntPtr.Zero;
                PoolPointer = WorksetPoolCreate(DataSetPointer, CachePointer, Environment.ProcessorCount * 10);

                // Initialise the list of property names and indexes.
                var propertyIndex = 0;
                var property = new StringBuilder(256);
                while (GetRequiredPropertyName(DataSetPointer, propertyIndex, property, property.Capacity) > 0)
                {
                    PropertyIndexes.Add(property.ToString(), propertyIndex);
                    propertyIndex++;
                }
            }

            ~Provider()
            {
                Dispose(false);
            }

            public void Dispose()
            {
                Dispose(true);
                GC.SuppressFinalize(this);
            }

            /// <summary>
            /// Disposes of the dataset checking that all worksets have been
            /// released before doing so.
            /// </summary>
            /// <param name="disposing"></param>
            protected virtual void Dispose(bool disposing)
            {
                AllWorksetsReleased.WaitOne();
                if (PoolPointer != IntPtr.Zero)
                {
                    Debug.WriteLine("Freeing Pool");
                    WorksetPoolFree(PoolPointer);
                    PoolPointer = IntPtr.Zero;
                }
                if (CachePointer != IntPtr.Zero)
                {
                    Debug.WriteLine("Freeing Cache");
                    ResultsetCacheFree(CachePointer);
                    CachePointer = IntPtr.Zero;
                }
                if (DataSetPointer != IntPtr.Zero) 
                {
                    Debug.WriteLine("Freeing DataSet");
                    DataSetFree(DataSetPointer);
                    DataSetPointer = IntPtr.Zero; 
                }
            }

            internal int GetPropertyIndex(string propertyName)
            {
                int index;
                if (PropertyIndexes.TryGetValue(propertyName, out index) == false)
                {
                    index = -1;
                }
                return index;
            }
        }

        public class MatchResult : IMatchResult
        {
            private readonly Provider _provider;

            /// <summary>
            /// Pointer to the workset allocated by the unmanaged code.
            /// </summary>
            private IntPtr _workSet;

            /// <summary>
            /// Memory used to retrieve the values.
            /// </summary>
            private readonly StringBuilder Values = new StringBuilder();

            /// <summary>
            /// Constructs a new instance of the match results for the user
            /// agent provided.
            /// </summary>
            /// <param name="provider">Provider configured for device detection</param>
            /// <param name="userAgent">User agent to be detected</param>
            internal MatchResult(Provider provider, string userAgent)
            {
                _provider = provider;
                Interlocked.Increment(ref _provider.AllocatedWorksets);
                _workSet = WorksetPoolGet(provider.PoolPointer);
                if (IntPtr.Zero == _workSet)
                {
                    throw new Exception("Could not get workset from pool.");
                }
                Console.WriteLine(userAgent);
                try
                {
                    MatchFromUserAgent(_workSet, userAgent);
                }
                catch(AccessViolationException ex)
                {
                    Console.WriteLine(userAgent);
                    Console.WriteLine(ex.Message);
                    Console.WriteLine(ex.StackTrace);
                }
            }

            /// <summary>
            /// Constructs a new instance of the match result for the HTTP
            /// headers provided.
            /// </summary>
            /// <param name="provider">Provider configured for device detection</param>
            /// <param name="headers">HTTP headers of the request for detection</param>
            internal MatchResult(Provider provider, NameValueCollection headers)
            {
                _provider = provider;
                _workSet = WorksetPoolGet(provider.PoolPointer);
                var httpHeaders = new StringBuilder();
                for (int i = 0; i < headers.Count; i++)
                {
                    httpHeaders.AppendLine(String.Format("{0} {1}",
                        headers.Keys[i],
                        String.Concat(headers.GetValues(i))));
                }
                try
                {
                    MatchFromHeaders(_workSet, httpHeaders);
                }
                catch (AccessViolationException ex)
                {
                    Console.WriteLine(httpHeaders.ToString());
                    Console.WriteLine(ex.Message);
                    Console.WriteLine(ex.StackTrace);
                }
            }

            /// <summary>
            /// Ensures any unmanaged memory is freed if dispose didn't run
            /// for any reason.
            /// </summary>
            ~MatchResult()
            {
                Dispose(false);
            }

            public void Dispose()
            {
                Dispose(true);
                GC.SuppressFinalize(this);
            }

            /// <summary>
            /// Releases the pointer to the workset back to the pool.
            /// </summary>
            /// <param name="disposing"></param>
            protected virtual void Dispose(bool disposing)
            {
                if (_workSet != IntPtr.Zero &&
                    _provider != null &&
                    _provider.PoolPointer != IntPtr.Zero) 
                {
                    WorksetPoolRelease(_provider.PoolPointer, _workSet);
                    // Reduce the number of worksets that are allocated.
                    Interlocked.Decrement(ref _provider.AllocatedWorksets);
                    // If the allocated worksets are now zero then signal
                    // the provider to release the pool, cache and dataset
                    // if it's being disposed of. Needed to ensure that
                    // all worksets are released before disposing of the
                    // provider.
                    if (_provider.AllocatedWorksets == 0)
                    {
                        _provider.AllWorksetsReleased.Set();
                    }
                }
                _workSet = IntPtr.Zero;
            }

            /// <summary>
            /// Returns the values for the property provided.
            /// </summary>
            /// <param name="propertyName"></param>
            /// <returns></returns>
            public string this[string propertyName]
            {
                get
                {
                    var index = _provider.GetPropertyIndex(propertyName);
                    if (index >= 0)
                    {
                        // Get the number of characters written. If the result is negative
                        // then this indicates that the values string builder needs to be
                        // set to the positive value and the method recalled.
                        var charactersWritten = GetPropertyValues(_workSet, index, Values, Values.Capacity);
                        if (charactersWritten < 0)
                        {
                            Values.Capacity = Math.Abs(charactersWritten);
                            charactersWritten = GetPropertyValues(_workSet, index, Values, Values.Capacity);
                        }
                        return Values.ToString();
                    }
                    return null;
                }
            }
        }

        #endregion

        #region DLL Imports

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl, 
            CharSet = CharSet.Ansi,
            SetLastError = true)]
        private static extern IntPtr InitWithPropertyString(String fileName, String properties);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl)]
        private static extern void DataSetFree(IntPtr dataSet);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr ResultsetCacheCreate(IntPtr dataSet, int size);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl)]
        private static extern void ResultsetCacheFree(IntPtr cache);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr WorksetPoolCreate(IntPtr dataSet, IntPtr cache, int size);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl)]
        private static extern void WorksetPoolFree(IntPtr pool);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr WorksetPoolGet(IntPtr pool);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl)]
        private static extern void WorksetPoolRelease(IntPtr pool, IntPtr ws);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetRequiredPropertyIndex(IntPtr dataSet, String propertyName);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetRequiredPropertyName(IntPtr dataSet, int requiredPropertyIndex, StringBuilder propertyName, int size);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl,
            CharSet = CharSet.Ansi)]
        private static extern void MatchFromUserAgent(IntPtr ws, String userAgent);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl,
            CharSet = CharSet.Ansi)]
        private static extern void MatchFromHeaders(IntPtr ws, StringBuilder httpHeaders);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetPropertyValues(IntPtr workSet, int requiredPropertyIndex, StringBuilder values, int size);
        
        #endregion

        #region Fields

        /// <summary>
        /// Insance of a class wrapping the unmanaged dataset.
        /// </summary>
        private readonly PatternWrapper.Provider _provider;

        #endregion

        #region Constructor and Destructor

        /// <summary>
        /// Construct the wrapper creating a workset for each CPU available.
        /// </summary>
        /// <param name="fileName">Path to the data set file.</param>
        /// <param name="properties">Collection of properties to include in the results.</param>
        /// <param name="size">The size of the cache to be used with the wrapper</param>
        public PatternWrapper(string fileName, IEnumerable<string> properties, int size = 0) 
            : this(fileName, String.Join(",", properties), size) {}

        /// <summary>
        /// Construct the wrapper creating a workset for each CPU available.
        /// </summary>
        /// <param name="fileName">Path to the data set file.</param>
        /// <param name="properties">Comma seperated list of properties to include in the results.
        /// <param name="size">The size of the cache to be used with the wrapper</param>
        /// </param>
        public PatternWrapper(string fileName, string properties, int size = 0)
        {
            _provider = new Provider(fileName, properties, size);
        }
        
        #endregion

        #region Public Methods

        /// <summary>
        /// A list of properties available from the provider.
        /// </summary>
        public IList<string> AvailableProperties
        {
            get { return _provider.PropertyIndexes.Keys; }
        }

        /// <summary>
        /// Returns a match result for the user agent provided.
        /// </summary>
        /// <param name="userAgent"></param>
        /// <returns></returns>
        public IMatchResult Match(string userAgent)
        {
            return new MatchResult(_provider, userAgent);
        }

        /// <summary>
        /// Returns a match result for the headers provided.
        /// </summary>
        /// <param name="headers"></param>
        /// <returns></returns>
        public IMatchResult Match(NameValueCollection headers)
        {
            return new MatchResult(_provider, headers);
        }

        /// <summary>
        /// Finalize to ensure that all resources have been freed.
        /// </summary>
        ~PatternWrapper()
        {
            Dispose(false);
        }

        /// <summary>
        /// Frees all the unmanaged resources.
        /// </summary>
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// Fress all the unmanaged resources.
        /// </summary>
        /// <param name="disposing"></param>
        protected virtual void Dispose(bool disposing)
        {
            if (_provider != null)
            {
                _provider.Dispose();
            }
        }

        #endregion
    }
}