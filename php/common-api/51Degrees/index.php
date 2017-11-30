<?php 
/*
This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
Copyright 2017 51Degrees Mobile Experts Limited, 5 Charlotte Close,
Caversham, Reading, Berkshire, United Kingdom RG4 7BY

This Source Code Form is the subject of the following patents and patent
applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
European Patent No. 2871816;
European Patent Application No. 17184134.9;
United States Patent Nos. 9,332,086 and 9,350,823; and
United States Patent Application No. 15/686,066.

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0.

If a copy of the MPL was not distributed with this file, You can obtain
one at http://mozilla.org/MPL/2.0/.

This Source Code Form is "Incompatible With Secondary Licenses", as
defined by the Mozilla Public License, v. 2.0.
*/

session_start(); 
?>

<h1>51Degrees PHP Common API Test</h1>

<h2>Configuration Options</h2>

<form method="get">

    <label for="FiftyOneProvider">FiftyOneProvider</label>
    <select name="FiftyOneProvider">

        <option <?php echo (isset($_GET["FiftyOneProvider"]) && $_GET["FiftyOneProvider"] === "Cloud") ? " selected " : '' ?> value="Cloud">Cloud</option>
        <option <?php echo (isset($_GET["FiftyOneProvider"]) && $_GET["FiftyOneProvider"] === "Pattern") ? " selected " : '' ?> value="Pattern">Pattern</option>
        <option <?php echo (isset($_GET["FiftyOneProvider"]) && $_GET["FiftyOneProvider"] === "TRIE") ? " selected " : '' ?> value="TRIE">TRIE</option>

    </select>

    <label for="FiftyOneLogLevel">FiftyOneLogLevel</label>
    <select name="FiftyOneLogLevel">

        <option <?php echo (isset($_GET["FiftyOneLogLevel"]) && $_GET["FiftyOneLogLevel"] === "fatal") ? " selected " : '' ?> value="fatal">fatal</option>
        <option <?php echo (isset($_GET["FiftyOneLogLevel"]) && $_GET["FiftyOneLogLevel"] === "warn") ? " selected " : '' ?> value="warn">warn</option>
        <option <?php echo (isset($_GET["FiftyOneLogLevel"]) && $_GET["FiftyOneLogLevel"] === "info") ? " selected " : '' ?> value="info">info</option>
        <option <?php echo (isset($_GET["FiftyOneLogLevel"]) && $_GET["FiftyOneLogLevel"] === "debug") ? " selected " : '' ?> value="debug">debug</option>

    </select>

    <label for="FiftyOneLicence">FiftyOneLicence</label><input name="FiftyOneLicence" <?php echo "value=" . (isset($_GET["FiftyOneLicence"]) ? $_GET["FiftyOneLicence"] : '') ?>> <br />
    <label for="FiftyOneLogFile">FiftyOneLogFile</label><input name="FiftyOneLogFile" <?php echo "value=" . (isset($_GET["FiftyOneLogFile"]) ? $_GET["FiftyOneLogFile"] : '') ?>> <br />

    <label for="FiftyOneUseSession">FiftyOneUseSession</label>
    <select name="FiftyOneUseSession">

        <option <?php echo (isset($_GET["FiftyOneUseSession"]) && $_GET["FiftyOneUseSession"] === "true") ? " selected " : '' ?> value="true">True</option>
        <option <?php echo (isset($_GET["FiftyOneUseSession"]) && $_GET["FiftyOneUseSession"] === "false") ? " selected " : '' ?> value="false">False</option>

    </select>

    <label for="FiftyOneSessionLifetime">FiftyOneSessionLifetime</label><input type="number" name="FiftyOneSessionLifetime" <?php echo "value=" . (isset($_GET["FiftyOneSessionLifetime"]) ? $_GET["FiftyOneSessionLifetime"] : '') ?>> <br />

    <label for="FiftyOneProperties">FiftyOneProperties (comma seperated)</label><textarea name="FiftyOneProperties"><?php echo (isset($_GET["FiftyOneProperties"]) ? $_GET["FiftyOneProperties"] : '') ?></textarea>

    <input type="submit"/>

</form>

<style>
    * {
        word-break: break-word;
        font-family: monospace
    }

    input,
    select,
    textarea,
    label {
        display: block;
        margin-bottom: 10px;
    }

    dt {
        font-weight: bold;
        font-size: 1.2em;
        margin: 0.5em 0 0.5em 0
    }

    dl dl {
        padding-left: 30px;
    }

    dd {
        background-color: lightgrey;
        margin: 0px;
        padding: 10px;
        box-sizing: border-box;
    }

</style>

<?php


if(isset($_GET["FiftyOneProvider"])){

    $time_start = microtime(true);

    $settings = array();

    foreach($_GET as $setting => $value){

        if($value === "true"){

            $value = true;

        }

        if($value === "false"){

            $value = false;

        }

        if(strlen($value) > 0){

            if($setting === "FiftyOneProperties"){

                $value = explode(",",$value);

            }

            $settings[$setting] = $value;

        }

    }

    include("51degrees.php");

    $provider = FiftyOneDegrees\FiftyOneDegreesGetProvider($settings);

    if (!function_exists('getallheaders')) {
        function getallheaders() {
            $headers = array();
            foreach ($_SERVER as $name => $value) {
                if (substr($name, 0, 5) == 'HTTP_') {
                    $headers[str_replace(' ', '-', ucwords(strtolower(str_replace('_', ' ', substr($name, 5)))))] = $value;
                }
            }
            return $headers;
        }
     }

     function matchTest ($match){

            if (isset($match->success) && !$match->success) {

                echo "No match object recieved. Check logs.";

                return false;

            }

            echo "<dl>";

            echo "<dt>getValues (BrowserVersion)</dt>";
            echo "<dd>" . json_encode($match->getValues("BrowserVersion")) . "</dd>";

            echo "<dt>getValue (BrowserVersion)</dt>";
            echo "<dd>" . json_encode($match->getValue("BrowserVersion")) . "</dd>";

            echo "<dt>getValues (Non existent property)</dt>";
            echo "<dd>" . json_encode($match->getValues("Foo")) . "</dd>";

            echo "<dt>getValue (Non existent property)</dt>";
            echo "<dd>" . json_encode($match->getValue("Foo")) . "</dd>";

            echo "<dt>getDeviceId</dt>";
            echo "<dd>" . json_encode($match->getDeviceId()) . "</dd>";

            echo "<dt>getRank</dt>";
            echo "<dd>" . json_encode($match->getRank()) . "</dd>";

            echo "<dt>getDifference</dt>";
            echo "<dd>" . json_encode($match->getDifference()) . "</dd>";

            echo "<dt>getMethod</dt>";
            echo "<dd>" . json_encode($match->getMethod()) . "</dd>";

            echo "</dl>";

        }

    echo "<dl>";

    echo "<dt>getAvailableProperties</dt>";

    echo "<dd>" . json_encode($provider->getAvailableProperties()) . "</dd>";

    echo "<dt>getDataSetName</dt>";

    echo "<dd>" . json_encode($provider->getDataSetName()) . "</dd>";

    echo "<dt>getDataSetFormat</dt>";

    echo "<dd>" . json_encode($provider->getDataSetFormat()) . "</dd>";

    echo "<dt>getDataSetPublishedDate</dt>";

    echo "<dd>" . json_encode($provider->getDataSetPublishedDate()) . "</dd>";

    echo "<dt>getDataSetNextUpdateDate</dt>";

    echo "<dd>" . json_encode($provider->getDataSetNextUpdateDate()) . "</dd>";

    echo "<dt>getDataSetSignatureCount</dt>";

    echo "<dd>" . json_encode($provider->getDataSetSignatureCount()) . "</dd>";

    echo "<dt>getDataSetDeviceCombinations</dt>";

    echo "<dd>" . json_encode($provider->getDataSetDeviceCombinations()) . "</dd>";

    echo "<dt>getMatchJson user agent string</dt>";

    echo "<dd>\"" . $provider->getMatchJSON($_SERVER['HTTP_USER_AGENT']) . "\"</dd>";

    echo "<dt>getMatchJson header array</dt>";

    echo "<dd>\"" . $provider->getMatchJSON(getallheaders()) . "\"</dd>";

    echo "<dt>Match for User Agent string</dt>";

    $match = $provider->getMatch($_SERVER['HTTP_USER_AGENT']);

    matchTest($match);

    echo "<dt>Match for HTTP Headers array</dt>";

    $match = $provider->getMatch(getallheaders());

    matchTest($match);

    if (isset($_GET["FiftyOneProvider"]) && $_GET["FiftyOneProvider"] != "TRIE") {

      echo "<dt>Match for DeviceID</dt>";
      $match = $provider->getMatchForDeviceId("15364-38914-71859-18092");
      matchTest($match);
      echo "</dl>";

    }

    $time_end = microtime(true);

    $execution_time = ($time_end - $time_start);

    echo '<hr/><b>Execution Time:</b> '.$execution_time.' seconds';

}
