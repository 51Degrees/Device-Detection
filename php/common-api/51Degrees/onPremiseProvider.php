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
  * On Premise version of the Provider object
*/
class FiftyOneDegreesProviderOnPremise
{
    /**
        * Provider object constructor
        *
        * @param object a settings object (see FiftyOneDegreesGetProvider)
        * @param FiftyOneDegreesLog A logger object
        * @return void
    **/
    public function __construct($settings, $logger)
    {
        $this->settings = $settings;
        $this->logger = $logger;

        // Load in On Premise files

        if ($settings["FiftyOneProvider"] === "Pattern") {

          $provider = new \Provider(ini_get("FiftyOneDegreesPatternV3.data_file"));

        } else if ($settings["FiftyOneProvider"] === "TRIE") {

          $provider = new \Provider(ini_get("FiftyOneDegreesTrieV3.data_file"));

        }

        $this->provider = $provider;

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
        * Get list of available properties
        *
        * @return string[] list of properties
    **/
    public function getAvailableProperties()
    {

        return vectorToArray($this->provider->getAvailableProperties());

    }

    /**
        * Get name of dataset
        *
        * @return string
    **/
    public function getDataSetName()
    {

        return $this->provider->getDataSetName();

    }

    /**
        * Get format of dataset
        *
        * @return string
    **/
    public function getDataSetFormat()
    {

        return $this->provider->getDataSetFormat();

    }

    /**
        * Get published date of dataset
        *
        * @return string
    **/
    public function getDataSetPublishedDate()
    {

        return $this->provider->getDataSetPublishedDate();


    }

    /**
        * Get next update date of dataset
        *
        * @return string
    **/
    public function getDataSetNextUpdateDate()
    {
        return $this->provider->getDataSetNextUpdateDate();
    }

    /**
        * Get signature count
        *
        * @return number
    **/
    public function getDataSetSignatureCount()
    {
        return $this->provider->getDataSetSignatureCount();
    }

    /**
        * Get device combinations
        *
        * @return number
    **/
    public function getDataSetDeviceCombinations()
    {
        return $this->provider->getDataSetDeviceCombinations();
    }


    /**
        * Internal function for making a SWIG compatible MapStringString from an array of headers
        *
        * @param array (array should be getallheaders() style format)
        * @return object
    **/
    private function headersToMapStringString($headers)
    {

        $importantHeaders = array();

        foreach (vectorToArray($this->provider->getHttpHeaders()) as $header) {

            if (isset($headers[$header])) {

                $importantHeaders[] = $header;

            }

        }

        $headerObject = new \MapStringString();

        foreach ($importantHeaders as $header) {

            $headerObject->set($header, $headers[$header]);

        }

        return $headerObject;

    }

    /**
        * Get a match object from user agent string or array of headers
        *
        * @param string|array (array should be getallheaders() style format)
        * @return FiftyOneDegreesMatchCloud
    **/
    public function getMatch($userAgentOrHeaders = "")
    {

        $matchObject;

        if (is_string($userAgentOrHeaders)) {

            $matchObject = $this->provider->getMatch($userAgentOrHeaders);

        } else {

            $headerObject = $this->headersToMapStringString($userAgentOrHeaders);

            $matchObject = $this->provider->getMatch($headerObject);

        }

        return new FiftyOneDegreesMatchOnPremise($matchObject, $this->logger);

    }

    /**
        * Get a JSON match object from user agent string or array of headers
        *
        * @param string|array (array should be getallheaders() style format)
        * @return string
    **/
    public function getMatchJSON($userAgentOrHeaders)
    {

        $matchObject;

        if (is_string($userAgentOrHeaders)) {

            $matchObject = $this->provider->getMatchJSON($userAgentOrHeaders);

        } else {

            $headerObject = $this->headersToMapStringString($userAgentOrHeaders);

            $matchObject = $this->provider->getMatchJSON($headerObject);

        }

        return $matchObject;

    }

    /**
        * Get a match object from a dash seperated list of profile ids.
        *
        * @param string
        * @return FiftyOneDegreesMatchCloud
    **/
    public function getMatchForDeviceId($chain)
    {

        $matchObject = $this->provider->getMatchForDeviceId($chain);

        return new FiftyOneDegreesMatchOnPremise($matchObject, $this->logger);

    }
}
