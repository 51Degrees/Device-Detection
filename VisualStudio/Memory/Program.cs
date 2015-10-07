using FiftyOne.Mobile.Detection.Provider.Interop;
using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleApplication1
{
    class Program
    {
        public class Source : IEnumerable<NameValueCollection>
        {
            private IWrapper _provider;
            
            public Source(IWrapper provider)
            {
                _provider = provider;
            }

            public IEnumerator<NameValueCollection> GetEnumerator()
            {
                return new Iterator(_provider);
            }

            System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
            {
                return new Iterator(_provider);
            }
        }

        public class Iterator : IEnumerator<NameValueCollection>, IDisposable
        {
            private Random _rand = new Random();

            private NameValueCollection _headers;

            private TextReader _reader;

            private IWrapper _provider;

            public Iterator(IWrapper provider)
            {
                _provider = provider;
                _reader = File.OpenText("../../../../../data/20000 User Agents.csv");
            }

            public NameValueCollection Current
            {
                get { return _headers; }
            }

            public void Dispose()
            {
                _reader.Dispose();
            }

            public bool MoveNext()
            {
                var nextHeaders = new NameValueCollection();
                var index = 0;
                var count = _rand.Next(_provider.HttpHeaders.Count);
                var line = _reader.ReadLine();
                while (line != null &&
                        index < count) 
                {
                    nextHeaders.Add(_provider.HttpHeaders[index], line.Trim());
                    index++;
                }
                _headers = nextHeaders;
                return line != null;
            }

            public void Reset()
            {
                _reader.Dispose();
                _reader = File.OpenText("../../../../../data/20000 User Agents.csv");
            }

            object System.Collections.IEnumerator.Current
            {
                get { return Current; }
            }
        }

        static void Main(string[] args)
        {
            using (var pattern = new PatternWrapper("../../../../../data/51Degrees-LiteV3.2.dat"))
            {
                Process(pattern);
            }
            using (var trie = new TrieWrapper("../../../../../data/51Degrees-LiteV3.2.trie"))
            {
                Process(trie);
            }
        }

        private static void Process(IWrapper provider)
        {
            Parallel.ForEach(new Source(provider), headers =>
            {
                using (var match = provider.Match(headers))
                {
                    Console.WriteLine(match["IsMobile"]);
                }
            });
        }
    }
}
