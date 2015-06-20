using FiftyOne.Mobile.Detection.Provider.Interop;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Console_Interop
{
    class Program
    {

        static void Main(string[] args)
        {
            // Initialise the pattern provider with a list of 4 properties.
            using (var pattern = new PatternWrapper(
                new FileInfo("..\\..\\..\\..\\data\\51Degrees-Lite.dat").FullName,
                    new[] { "Id", "IsTablet", "IsMobile", "ScreenPixelsWidth", "ScreenPixelsHeight" }))
            {

                // Initialise the trie provider with a data file and a list of 4 properties.
                using (var trie = new TrieWrapper(
                    new FileInfo("..\\..\\..\\..\\data\\51Degrees-Lite.trie").FullName,
                    new[] { "Id", "IsTablet", "IsMobile", "ScreenPixelsWidth", "ScreenPixelsHeight" }))
                {

                    // IMPORTANT: For a full list of properties see: http://51degrees.mobi/products/devicedata/propertydictionary

                    using (var reader = new FileInfo(args.Length > 0 ? args[0] : "..\\..\\..\\..\\data\\20000 User Agents.csv").OpenText())
                    {
                        var start = DateTime.UtcNow;
                        Console.WriteLine("Started -> {0}", start);
                        var line = reader.ReadLine();
                        while (line != null)
                        {
                            var patternResults = pattern.GetProperties(line.Trim());
                            var trieResults = trie.GetProperties(line.Trim());
                            Output(patternResults);
                            Output(trieResults);
                            line = reader.ReadLine();
                        }
                        Console.WriteLine("Elapsed Time -> {0} seconds", (DateTime.UtcNow - start).TotalSeconds);
                    }
                }
            }
            Console.ReadKey();
        }

        private static void Output(SortedList<string, List<string>> results)
        {
#if DEBUG
            using (var writer = new StreamWriter(Console.OpenStandardOutput()))
#else
            using (var writer = new StreamWriter(Console.OpenStandardOutput()))
            //using(var writer = new StreamWriter(Stream.Null))
#endif
            {
                foreach (var item in results)
                {
                    writer.WriteLine("{0} -> {1}",
                        item.Key,
                        String.Join(", ", item.Value));
                }
            }
        }
    }
}
