/*
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright © 2017 51Degrees Mobile Experts Limited, 5 Charlotte Close,
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
 * This Source Code Form is "Incompatible With Secondary Licenses", as
 * defined by the Mozilla Public License, v. 2.0.
 */

package Tutorials;

import FiftyOneDegreesTrieV3.FiftyOneDegreesTrieV3;
import FiftyOneDegreesTrieV3.Match;
import FiftyOneDegreesTrieV3.Provider;
import FiftyOneDegreesTrieV3.VectorString;

import java.io.*;
import java.net.URL;

/**
 * <tutorial>
 * Example of using 51Degrees Pattern Detection to process a file containing
 * User-Agent header values and output a CSV file containing the same header
 * values with various properties detected by 51Degrees.
 * <p>
 * The example illustrates:
 * <ol>
 *  <li>
 *  Loading the 51Degrees Hash Trie library
 *  <pre class="prettyprint lang-java">
 *  <code>
 *      res = FiftyOneDegreesTrieV3.class.getResource("/FiftyOneDegreesTrieV3.so");
 *      System.load(res.getPath());
 *  </code>
 *  </pre>
 *  </li>
 *  <li>How to instantiate 51Degrees detection provider
 *  <pre class="prettyprint lang-java">
 *  <code>
 *      provider = new Provider("path/to/51Degrees/data/file.trie");
 *  </code>
 *  </pre>
 *  </li>
 *  <li>Matching a User-Agent header value
 *  <ol>
 *      <li>By creating a match and using it repeatedly (for efficiency)
 *      <pre class="prettyprint lang-java">
 *      <code>
 *          Match match;
 *      </code>
 *      </pre>
 *      <pre class="prettyprint lang-java">
 *      <code>
 *          match = provider.getMatch(userAgentString);
 *      </code>
 *      </pre>
 *      </li>
 *      <li>By having the provider create a new Match for each detection
 *      <pre class="prettyprint lang-java">
 *      <code>
 *          Match match = provider.getMatch(userAgentString);
 *      </code>
 *      </pre>
 *      </li>
 *  </ol>
 *  <li>Getting the values for some properties of the matched User-Agent header
 *  <pre class="prettyprint lang-java">
 *  <code>
 *      VectorString isMobile = match.getValues("IsMobile");
 *  </code>
 *  </pre>
 *  </p>
 * <p>
 * The <a href="https://51degrees.com/resources/property-dictionary">
 * 51 Degrees Property Dictionary</a> contains a description of each of the
 * properties and the editions in which they are available.
 * </p>
 * <p>
 * main assumes it is being run with a working directory at root of
 * project or of this module.
 * </p>
 * </tutorial>
 */
public class OfflineProcessingExample implements Closeable {
    // Snippet Start
    // output file in current working directory
    public String outputFilePath = "batch-processing-example-results.csv";
    // pattern detection matching provider
    private final Provider provider;

    private final URL res;

    /**
     * Loads the 51Degrees Hash Trie library and initialises the device
     * detection Provider with the Lite data file. For more data see:
     * <a href="https://51degrees.com/compare-data-options">compare data options
     * </a>
     *
     * @throws IOException can be thrown if there is a problem reading from the
     * provided data file.
     */
    public OfflineProcessingExample() throws IOException {
        // Load the C/C++ native library. Uncomment dll line for windows and so line in linux.
        res = FiftyOneDegreesTrieV3.class.getResource("/FiftyOneDegreesTrieV3.dll");
        // res = FiftyOneDegreesTrieV3.class.getResource("/FiftyOneDegreesTrieV3.so");
        System.load(res.getPath());

        // Create a new provider.
        provider = new Provider("../../data/51Degrees-LiteV3.4.trie");
    }

    /**
     * Reads a CSV file containing User-Agents and adds the IsMobile,
     * PlatformName and PlatformVersion information for the first 20 lines.
     * For a full list of properties and the files they are available in please
     * see: <a href="https://51degrees.com/resources/property-dictionary">
     * Property Dictionary</a>
     *
     * @param inputFileName the CSV file to read from.
     * @param outputFilename where to save the file with extra entries.
     * @throws IOException if there was a problem reading from the data file.
     */
    public void processCsv(String inputFileName, String outputFilename) 
            throws IOException {
        BufferedReader bufferedReader = 
                new BufferedReader(new FileReader(inputFileName));
        try {
            FileWriter fileWriter = new FileWriter(outputFilename);
            try {
                // it's more efficient over the long haul to create a match 
                // once and reuse it in multiple matches
                Match match;
                // there are 20k lines in supplied file, we'll just do a couple 
                // of them!
                for (int i = 0; i < 20; i++) {

                    // read next line
                    String userAgentString = bufferedReader.readLine();

                    // ask the provider to match the UA using match we created
                    match = provider.getMatch(userAgentString);

                    // get some property values from the match
                    VectorString isMobile = match.getValues("IsMobile");
                    VectorString platformName = match.getValues("PlatformName");
                    VectorString platformVersion = match.getValues("PlatformVersion");


                    // write result to file
                    fileWriter.append("\"")
                            .append(userAgentString)
                            .append("\", ")
                            .append(getValueForDisplay(isMobile))
                            .append(", ")
                            .append(getValueForDisplay(platformName))
                            .append(", ")
                            .append(getValueForDisplay(platformVersion))
                            .append('\n')
                            .flush();
                }
            } finally {
                fileWriter.close();

            }
        } finally {
            bufferedReader.close();
        }
    }

    /**
     * Match values may be null. A helper method to get something displayable
     * @param values a Values to render
     * @return a non-null String
     */
    protected String getValueForDisplay(VectorString values) {
        return values == null ? "N/A": values.get(0);
    } 

    public void close() throws IOException {
        provider.delete();
    }

    /**
     * Instantiates this class and starts
     * {@link #processCsv(java.lang.String, java.lang.String)} with default
     * parameters.
     *
     * @param args command line arguments.
     * @throws IOException if there was a problem accessing the data file.
     */
    public static void main(String[] args) throws IOException {
        System.out.println("Starting Offline Processing Example");
        OfflineProcessingExample offlineProcessingExample = 
                new OfflineProcessingExample();
        try {
            offlineProcessingExample.processCsv("../../data/20000 User Agents.csv",
                    offlineProcessingExample.outputFilePath);
            System.out.println("Output written to " + 
                    offlineProcessingExample.outputFilePath);
        } finally {
            offlineProcessingExample.close();
        }
    }
    // Snippet End
}
