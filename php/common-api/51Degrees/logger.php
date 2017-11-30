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

/**
  * Class for logging messages to files
*/
class FiftyOneDegreesLog
{

    private $levels = array("debug","info","warn","fatal");

    public function __construct($settings)
    {
        if (!isset($settings["FiftyOneLogFile"])) {

            $logPath = getcwd() . "/" . "51degrees.log";

        } else {

            $logPath = $settings["FiftyOneLogFile"];

        }

        $this->logPath = $logPath;

        if (!isset($settings["FiftyOneLogLevel"])) {

            $logLevel = "fatal";

        } else {

            $logLevel = $settings["FiftyOneLogLevel"];

        }

        $this->logLevel = array_search($logLevel, $this->levels);

    }

    public function log($level, $message)
    {

        $logIndex = array_search($level, $this->levels);

        if ($logIndex >= $this->logLevel) {

            $logMessage = json_encode(array(

                "time" => date('m/d/Y h:i:s a', time()),
                "level" => $level,
                "message" => $message,
                "backtrace" => debug_backtrace(DEBUG_BACKTRACE_IGNORE_ARGS,3)

            ));

            file_put_contents($this->logPath, $logMessage.PHP_EOL, FILE_APPEND | LOCK_EX);

        }

    }
}
