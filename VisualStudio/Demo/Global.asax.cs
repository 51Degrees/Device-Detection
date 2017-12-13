using FiftyOne.Mobile.Detection.Provider.Interop;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Web;
using System.Web.Security;
using System.Web.SessionState;

namespace FiftyOne.Demo.WebSite
{
    public class Global : System.Web.HttpApplication
    {
        /// <summary>
        /// A provider initialised when the application starts for Pattern
        /// device detection.
        /// </summary>
        public static PatternWrapper PatternProvider
        {
            get { return _pattern; }
        }
        private static PatternWrapper _pattern = null;

        /// <summary>
        /// A provider initialised when the application starts for Trie
        /// device detection.
        /// </summary>
        public static TrieWrapper TrieProvider
        {
            get { return _trie; }
        }
        private static TrieWrapper _trie = null;

        /// <summary>
        /// Creates the device detection providers.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void Application_Start(object sender, EventArgs e)
        {
            _pattern = new PatternWrapper(
            Path.Combine(
                AppDomain.CurrentDomain.BaseDirectory,
                "..\\..\\data\\51Degrees-LiteV3.2.dat"));
            _trie = new TrieWrapper(
            Path.Combine(
                AppDomain.CurrentDomain.BaseDirectory,
                "..\\..\\data\\51Degrees-LiteV3.4.trie"));
        }

        /// <summary>
        /// Disposes of the device detection providers.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void Application_End(object sender, EventArgs e)
        {
            if (_pattern != null)
            {
                _pattern.Dispose();
                _pattern = null;
            }
            if (_trie != null)
            {
                _trie.Dispose();
                _trie = null;
            }
        }
    }
}