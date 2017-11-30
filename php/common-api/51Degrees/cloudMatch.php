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
  * Cloud version of the Match object
*/
class FiftyOneDegreesMatchCloud
{

    private $properties = array();

    /**
        * Match object constructor
        *
        * @param object a Match object
        * @param FiftyOneDegreesLog A logger object
        * @return void
    **/
    public function __construct($matchObject, $logger)
    {

        if (!$matchObject) {

            $this->success = false;

        } else {

            $this->success = true;

            $this->match = $matchObject;
            $this->logger = $logger;

            foreach ($matchObject["Values"] as $name => $value) {

                $this->properties[$name] = $value;

            }

        }

    }

    /**
        * Internal logging function
        *
        * @param string fatal/warn/debug/info
        * @param string The message to be logged
        * @return void
    **/
    private function log($level, $message)
    {

        $this->logger->log($level, $message);

    }

    /**
        * Get array of values for a property
        *
        * @param string property name
        * @return string[] (array of value strings)
    **/
    public function getValues($propertyName)
    {

        if (isset($this->properties[$propertyName])) {

            return $this->properties[$propertyName];

        } else {

            $this->logger->log("info", "Property ".$propertyName." not available");

            return null;

        }

    }

    /**
        * Get single value for a property
        *
        * @param string property name
        * @return string (the first value strings)
    **/
    public function getValue($propertyName)
    {

        if (isset($this->properties[$propertyName])) {

            return $this->properties[$propertyName][0];

        } else {

            $this->logger->log("info", "Property ".$propertyName." not available");

            return null;

        }

    }

    /**
        * Get the profile ids for the device
        *
        * @return string (4 profile ids seperated by a -)
    **/
    public function getDeviceId()
    {

        $profileIds = array();

        if (isset($this->match["ProfileIds"])) {

            foreach ($this->match["ProfileIds"] as $id) {

                 $profileIds[] = $id;

            }

        }

        return join("-", $profileIds);

    }

    /**
        * Get rank (not supported in cloud)
        *
        * @return number -1
    **/
    public function getRank()
    {

        return -1;

    }

    /**
        * Get Difference
        *
        * @return number
    **/
    public function getDifference()
    {

        return $this->match["Difference"];

    }

    /**
        * Get MatchMethod
        *
        * @return string
    **/
    public function getMethod()
    {

        return $this->match["MatchMethod"];

    }
}
