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

using FiftyOne.Mobile.Detection.Provider.Interop;
using System;
using System.Collections.Generic;

namespace FiftyOne.UnitTests
{
    /// <summary>
    /// Base class for all unit tests.
    /// </summary>
    public abstract class Base : IDisposable
    {
        /// <summary>
        /// The wrapper to use for the tests.
        /// </summary>
        protected IWrapper _wrapper;

        /// <summary>
        /// Method used to create the wrapper when the constructor is called.
        /// </summary>
        /// <returns></returns>
        protected abstract IWrapper CreateWrapper();

        /// <summary>
        /// Location of the data file used to create the wrapper.
        /// </summary>
        protected abstract string DataFile { get; }

        /// <summary>
        /// Enumeration of required properties from the test.
        /// </summary>
        protected virtual IEnumerable<string> RequiredProperties { get { return _wrapper.AvailableProperties; } }
        
        /// <summary>
        /// Disposes of the wrapper ensuring unmanaged resources are released.
        /// </summary>
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        ~Base()
        {
            Dispose(false);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (_wrapper != null)
            {
                _wrapper.Dispose();
                _wrapper = null;
            }
        }
    }
}
