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

namespace FiftyOneDegrees;

if (!isset($_GET["datafile"])) {

  die("No datafile parameter passed in");

} else {

  $datafile = $_GET["datafile"];

}

function download($file_source, $file_target) {

    $md5 = "";

    $rh = fopen($file_source, 'rb');

    foreach($http_response_header as $header){

        if(strpos($header,"Content-MD5: ") === 0){

            $md5 = str_replace("Content-MD5: ", "", $header);

        }

    };

    $wh = fopen($file_target, 'w+b');

    if (!$rh || !$wh) {
        return false;
    }

    while (!feof($rh)) {
        if (fwrite($wh, fread($rh, 4096)) === FALSE) {
            return false;
        }
        echo ' ';
        flush();
    }

    fclose($rh);
    fclose($wh);

    return $md5;
}

function uncompress($srcName, $dstName) {

    $md5 = "";

    $sfp = gzopen($srcName, "rb");
    $fp = fopen($dstName, "w");

    while (!gzeof($sfp)) {
        $string = gzread($sfp, 4096);
        fwrite($fp, $string, strlen($string));
    }
    gzclose($sfp);
    fclose($fp);

}

function autoUpdate($provider, $licence) {

    global $datafile;
    
    if ($provider->getDataSetFormat() === "3.2")
        $type = "BinaryV32";
    else
        $type = $provider->getDataSetFormat();

    $parameters = array(
        "LicenseKeys" => $licence,
        "Type" => $type,
        "Download" => "True",
        "Product" => $provider->getDataSetName()
    );

    $location = ini_get("FiftyOneDegrees".$datafile.".data_file");

    $query = http_build_query($parameters);

    $url = "https://distributor.51degrees.com/api/v2/download" . "?" . $query;

    set_time_limit(0);

    $md5 = download($url, getcwd(). "/tmp.gz");

    if(!empty($md5) && md5_file(getcwd(). "/tmp.gz") === $md5){

        uncompress(getcwd(). "/tmp.gz", $location);

        $provider->reloadFromFile();

    } else {

        // Something went wrong

    }

    unlink(getcwd(). "/tmp.gz");

}

require "FiftyOneDegrees".$datafile.".php";

$provider = new \Provider(ini_get("FiftyOneDegrees".$datafile.".data_file"));

$updateDate = $provider->getDataSetNextUpdateDate();

if (strtotime($updateDate) < time()) {

    $key = get_cfg_var("FiftyOneDegrees".$datafile.".license_key");

    if(isset($key) && $key){

        autoUpdate($provider, $key);

    } else {

        echo "No license key variable in PHP ini";

    }

}
