/* *********************************************************************
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.
 *
 * If a copy of the MPL was not distributed with this file, You can obtain
 * one at http://mozilla.org/MPL/2.0/.
 *
 * This Source Code Form is "Incompatible With Secondary Licenses", as
 * defined by the Mozilla Public License, v. 2.0.
 * ********************************************************************* */

using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Threading;
using System.Collections.Concurrent;

namespace FiftyOne.Mobile.Detection.Provider.Interop
{
    /// <summary>
    /// Class used to wrap functions exposed by the pattern matching C
    /// DLL.
    /// </summary>
    public class PatternWrapper : IDisposable
    {
        #region DLL Imports
        
        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl, 
            CharSet = CharSet.Ansi)]
        private static extern IntPtr CreateDataSet(String fileName, String properties);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl)]
        private static extern void Destroy(IntPtr dataSet);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr CreateWorkSet(IntPtr dataSet);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl)]
        private static extern void FreeWorkSet(IntPtr ws);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern int GetPropertiesCSV(IntPtr ws, String userAgent, StringBuilder result, Int32 resultLength);

        #endregion

        #region WorkSet Class

        internal class WorkSet : IDisposable
        {
            /// <summary>
            /// The initial capacity of the buffer used to store the resulting
            /// properties.
            /// </summary>
            private const int DEFAULT_CAPACITY = 8096;

            /// <summary>
            /// The maximum size of the buffer. Anything more than this is too much.
            /// </summary>
            private const int MAX_CAPACITY = DEFAULT_CAPACITY * 10;

            /// <summary>
            /// The workset memory used in the DLL.
            /// </summary>
            private IntPtr _workSet = IntPtr.Zero;

            /// <summary>
            /// The response buffer used to store the char array containing
            /// properties and values from the DLL.
            /// </summary>
            private StringBuilder _result = new StringBuilder(DEFAULT_CAPACITY);

            /// <summary>
            /// Indicates if the workset is free. Must be set to false
            /// when it is locked for processing.
            /// </summary>
            internal bool AcquiredLock = false;

            /// <summary>
            /// Constructs an instance of workset reserving the necessary memory
            /// within the provider DLL.
            /// </summary>
            internal WorkSet(IntPtr dataSet)
            {
                _workSet = CreateWorkSet(dataSet);
            }

            /// <summary>
            /// Returns the properties for the userAgent as a CSV string.
            /// </summary>
            /// <param name="userAgent">The user agent of the device being searched for.</param>
            /// <returns>A comma seperated list of properties.</returns>
            internal string GetPropertiesAsCSV(string userAgent)
            {
                int length = 0;
                do
                {
                    // Call the DLL to get a CSV string of properties.
                    length = GetPropertiesCSV(
                        _workSet,
                        userAgent,
                        _result,
                        _result.Capacity);

                    // If the StringBuilder is too small then increase the size
                    // and try again.
                    if (length < 0)
                    {
                        _result.Capacity += DEFAULT_CAPACITY;
                        // If the capacity is now so large something has gone
                        // wrong exit the loop.
                        if (_result.Capacity > MAX_CAPACITY)
                            break;
                    }
                }
                while (length < 0);

                // If a valid string was created then return.
                if (length > 0)
                    return _result.ToString();
                return null;
            }

            /// <summary>
            /// Returns a list of key value pairs for the properties.
            /// </summary>
            /// <param name="userAgent">The user agent of the device being searched for.</param>
            /// <returns>The properties and device values for the device matching the useragent provided.</returns>
            internal SortedList<string, List<string>> GetProperties(string userAgent)
            {
                return Utils.GetProperties(GetPropertiesAsCSV(userAgent), 0, 1);
            }

            /// <summary>
            /// Frees the working memory reserved by the DLL which is not handled by
            /// the CLR garbage collector.
            /// </summary>
            public void Dispose()
            {
                FreeWorkSet(_workSet);
            }
        }

        #endregion

        #region Properties

        /// <summary>
        /// The dataset to be used by the provider.
        /// </summary>
        private readonly IntPtr _dataSet = IntPtr.Zero;

        /// <summary>
        /// The workset being used by the provider.
        /// </summary>
        private readonly ConcurrentQueue<WorkSet> _workSets = new ConcurrentQueue<WorkSet>();
        
        #endregion

        #region Constructor and Destructor

        /// <summary>
        /// Construct the wrapper creating a workset for each CPU available.
        /// </summary>
        /// <param name="fileName">Path to the data set file.</param>
        /// <param name="properties">Array of properties to include in the results.</param>
        public PatternWrapper(string fileName, string[] properties) 
            : this(fileName, String.Join(",", properties)) {}

        /// <summary>
        /// Construct the wrapper creating a workset for each CPU available.
        /// </summary>
        /// <param name="fileName">Path to the data set file.</param>
        /// <param name="properties">Comma seperated list of properties to include in the results.
        /// </param>
        public PatternWrapper(string fileName, string properties)
        {
            _dataSet = CreateDataSet(fileName, properties);
            if (_dataSet == IntPtr.Zero)
                throw new Exception(String.Format(
                    "Pattern initialisation failed with file '{0}'.",
                    fileName));
        }
        
        #endregion

        #region Private Methods

        /// <summary>
        /// Gets a work set ready to be used for detection.
        /// </summary>
        /// <returns></returns>
        private WorkSet GetWorkSet()
        {
            WorkSet workSet;
            if (_workSets.TryDequeue(out workSet) == false)
            {
                workSet = new WorkSet(_dataSet);
            }
            return workSet;
        }

        /// <summary>
        /// Frees the workset for another thread to use.
        /// </summary>
        /// <param name="workSet"></param>
        private void ReleaseWorkSet(WorkSet workSet)
        {
            _workSets.Enqueue(workSet);
        }

        #endregion

        #region Public Methods

        /// <summary>
        /// Returns the properties for the userAgent as a CSV format string.
        /// </summary>
        /// <param name="userAgent"></param>
        /// <returns></returns>
        public string GetPropertiesAsCSV(string userAgent)
        {
            var workSet = GetWorkSet();
            var result = workSet.GetPropertiesAsCSV(userAgent);
            ReleaseWorkSet(workSet);
            return result;
        }

        /// <summary>
        /// Returns a list of properties and values for the userAgent provided.
        /// </summary>
        /// <param name="userAgent">The useragent to search for.</param>
        /// <returns>A list of properties.</returns>
        public SortedList<string, List<string>> GetProperties(string userAgent)
        {
            var workSet = GetWorkSet();
            var result = workSet.GetProperties(userAgent);
            ReleaseWorkSet(workSet);
            return result;
        }

        /// <summary>
        /// Frees all the workset resource created in the DLL.
        /// </summary>
        public void Dispose()
        {
            WorkSet workSet;
            while(_workSets.TryDequeue(out workSet))
            {
                workSet.Dispose();
            }
            Destroy(_dataSet);
        }

        #endregion
    }
}