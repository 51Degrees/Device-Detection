using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace FiftyOne.UnitTests.Update
{
    public abstract class Base : FiftyOne.UnitTests.Base
    {
        bool _useReloadFromMemory = false;

        private class State
        {
            internal bool Processing = true;
            internal int Reloads = 0;
        }

        protected void UpdateTest()
        {
            UpdateTest(false);
        }

        protected void UpdateTest(bool UseReloadFromMemory)
        {
            if (UseReloadFromMemory)
            {
                _useReloadFromMemory = true;
            }
            var userAgents = UserAgentGenerator.GetBadUserAgents(10000).ToArray();
            var a = UpdateTest(userAgents);
            Console.WriteLine("a: {0}", a);
            var b = UpdateTest(userAgents);
            Console.WriteLine("b: {0}", b);
            Assert.IsTrue(a == b,
                "Two tests with the same User-Agents repeatably reloading the " +
                "data file resulted in different results.");
        }

        private int UpdateTest(IList<string> userAgents)
        {
            var checkSum = 0;
            var state = new State();
            var updateThread = new Thread(Run);
            try
            {
                updateThread.Start(state);
                Parallel.ForEach(userAgents, ua =>
                {
                    using (var match = _wrapper.Match(ua))
                    {
                        var hashcode = 0;
                        foreach (var property in _wrapper.AvailableProperties)
                        {
                            foreach (var value in match.getValues(property))
                            {
                                hashcode ^= value.GetHashCode();
                            }
                        }
                        lock (this)
                        {
                            checkSum ^= hashcode;
                        }
                    }
                });
            }
            finally
            {
                state.Processing = false;
                updateThread.Join();
                Console.WriteLine("'{0}' Reloads", state.Reloads);
            }
            return checkSum;
        }

        /// <summary>
        /// A parallel thread that will repeatably reload the data set from 
        /// the original file whilst other threads are performing device 
        /// detection of a known set of User-Agents. The reload operation is
        /// delayed for up to 1 second before being repeated.
        /// </summary>
        /// <param name="state"></param>
        private void Run(object state)
        {
            var random = new Random();
            while (((State)state).Processing)
            {
                if (_useReloadFromMemory)
                {
                    _wrapper.ReloadFromMemory();
                }
                _wrapper.ReloadFromFile();
                ((State)state).Reloads++;
                Thread.Sleep(random.Next(1000));
            }
        }
    }
}
