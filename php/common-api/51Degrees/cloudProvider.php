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
  * Cloud version of the Provider object
*/
class FiftyOneDegreesProviderCloud
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
        
        if (!isset($settings["FiftyOneLicence"])) {
            
            $error = "No licence key set";
            
            $logger->log("fatal", $error);
            
            trigger_error($error, E_USER_ERROR);

        }
    
        $this->logger = $logger;
                
        $this->settings = $settings;

    }
    
    /**
        * Internal cloud cache checking function
        *
        * @param string the type of cache (from the API path e.g "match")
        * @param array query parameters to check it is a cached and not a new query
        * @return object|null
    **/
    private function checkCache($type, $details)
    {
                                
        if (isset($this->settings["FiftyOneUseSession"]) && $this->settings["FiftyOneUseSession"] && isset($_SESSION[$type]) && $_SESSION[$type]["query"] === json_encode($details)) {
            
            return $_SESSION[$type]["match"];
            
        } else {
            
            return false;
            
        }
        
    }
    
     /**
        * Internal cloud cache storing function
        *
        * @param string the type of cache (from the API path e.g "match")
        * @param array query parameters to check it is a cached and not a new query
        * @param array api response to store in the cache
        * @return void
    **/
    private function storeCache($type, $details, $match)
    {
                
        if (isset($this->settings["FiftyOneUseSession"]) && $this->settings["FiftyOneUseSession"]) {
            
            $_SESSION[$type] = array(
                "query" => json_encode($details),
                "type" => $type,
                "match" => $match
            );
                 
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
        * Internal function to call the API
        *
        * @param string API path
        * @param array Query parameters
        * @param boolean Whether to store in cache
        * @return array|null JSON response as PHP array or null if failed
    **/
    private function cloudCall($path, $parameters = array(), $cache = false)
    {
        
        if ($cache && $this->checkCache($path, $parameters)) {
            
            return $this->checkCache($path, $parameters);
            
        } else {
            
            $basePath = "";
                        
            if(extension_loaded('openssl')){
                
                $basePath = "https://cloud.51degrees.com/api/v1/";
                
            } else {
                                
                $basePath = "http://cloud.51degrees.com/api/v1/";
                
            }
            
            $fullPath = $basePath . $this->settings["FiftyOneLicence"] . "/" . $path . "?" . http_build_query($parameters);
            
            $contents = array();
                
            $contents = @file_get_contents($fullPath);
                
            $matches = array();
            preg_match('#HTTP/\d+\.\d+ (\d+)#', $http_response_header[0], $matches);
        
            $code = $matches[1];
            
            if ($code !== "200") {
                
                if ($code[0] === "5") {
                    
                    $this->log("fatal", "server error");
                    
                } elseif ($code === "403") {
                    
                    $this->log("fatal", "licence key not valid");
                    
                } elseif ($code === "404") {
                    
                    $this->log("fatal", "There was an error processing your request. You have either requested the incorrect URL, i.e. you requested the device with profile id -1. Or the entity (Device Profile, Values, Properties etc...) with selected parameters was not found. Please refer to the documentation for more information.");
                    
                } elseif ($code === "429") {
                    
                    $this->log("fatal", "Too many requests");
                    
                } else {
                    
                    $this->log("Bad request");
                    
                }
                
                return null;
                
            } else {
                
                $contents = json_decode($contents,true);
               
                if ($contents && $cache) {

                    $this->storeCache($path, $parameters, $contents);

                }

                return $contents;
                
            }
            
        }
        
    }
    
    /**
        * Internal function to reformat a date to the required output format
        *
        * @param string datestring
        * @return string
    **/
    private function dateFormat($dateString)
    {
                        
        return date("Y-m-d", strtotime($dateString));
        
    }
    
    /**
        * Get list of available properties
        *
        * @return string[] list of properties
    **/
    public function getAvailableProperties()
    {
        
        $properties = $this->cloudCall("properties");
        
        if (!$properties) {
            
            return null;
            
        }
        
        $propertyArray = array();
        
        foreach ($properties as $property) {
            
            $propertyArray[] = $property["Name"];
            
        }
        
        return $propertyArray;
        
    }
    
    /**
        * Get name of dataset
        *
        * @return string
    **/
    public function getDataSetName()
    {
        
        return $this->cloudCall("datasetname");
        
    }
    
    /**
        * Get format of dataset
        *
        * @return string
    **/
    public function getDataSetFormat()
    {
        
        return "Cloud";
        
    }
    
    /**
        * Get published date of dataset
        *
        * @return string
    **/
    public function getDataSetPublishedDate()
    {
        
        $publishedDate = $this->cloudCall("publisheddate");
    
        if (!$publishedDate) {
            
            return null;
            
        }
        
        return $this->dateFormat($publishedDate);

    }
    
    /**
        * Get next update date of dataset
        *
        * @return string
    **/
    public function getDataSetNextUpdateDate()
    {
        
        $updateDate = $this->cloudCall("nextupdatedate");
        
        if (!$updateDate) {
            
            return null;
            
        }
        
        return $this->dateFormat($updateDate);
        
    }
    
    /**
        * Get signature count (not supported in cloud returns -1)
        *
        * @return number
    **/
    public function getDataSetSignatureCount()
    {
        
        return -1;
        
    }
    
    /**
        * Get device combinations (not supported in cloud returns -1)
        *
        * @return number
    **/
    public function getDataSetDeviceCombinations()
    {
        
        return -1;
        
    }
    
    /**
        * Internal function for extracting User-Agent from string or array of headers
        *
        * @param string|array (array should be getallheaders() style format)
        * @return string A user agent string
    **/
    private function getUserAgent($userAgentOrHeaders)
    {
        
        $userAgent = "";
        
        if (is_string($userAgentOrHeaders)) {
            
            $userAgent = $userAgentOrHeaders;
            
        } else {
            
            $userAgent = $userAgentOrHeaders["User-Agent"];
            
        }
        
        return $userAgent;
        
    }
    
    /**
        * Get a match object from user agent string or array of headers
        *
        * @param string|array (array should be getallheaders() style format)
        * @return FiftyOneDegreesMatchCloud
    **/
    public function getMatch($userAgentOrHeaders = "")
    {
        
        $parameters = array();

        $parameters["User-Agent"] = $this->getUserAgent($userAgentOrHeaders);
        
        if (isset($this->settings["FiftyOneProperties"])) {
                            
            $parameters["values"] = join("+", $this->settings["FiftyOneProperties"]);
                
        }
                                 
        return new FiftyOneDegreesMatchCloud($this->cloudCall("match", $parameters, true), $this->logger);
        
    }
    
    /**
        * Get a JSON match object from user agent string or array of headers
        *
        * @param string|array (array should be getallheaders() style format)
        * @return string
    **/
    public function getMatchJson($userAgentOrHeaders = "")
    {
        
        $parameters = array();

        $parameters["User-Agent"] = $this->getUserAgent($userAgentOrHeaders);
        
        if (isset($this->settings["FiftyOneProperties"])) {
                            
            $parameters["values"] = join("+", $this->settings["FiftyOneProperties"]);
                
        }
                         
        return json_encode($this->cloudCall("match", $parameters, true));
        
    }
    
    /**
        * Get a match object from a dash seperated list of profile ids.
        *
        * @param string
        * @return FiftyOneDegreesMatchCloud
    **/
    public function getMatchForDeviceId($chain)
    {
        
        $idArray = explode("-", $chain);
        
        $match = array();
        
        $match["Values"] = array();
                
        foreach ($idArray as $id) {
            
            $profileValues = $this->cloudCall("profilevalues", array("profileid"=>$id));
            
            if ($profileValues) {
                
                foreach ($profileValues as $property) {
                
                    if (!isset($match["Values"][$property["PropertyName"]])) {

                        $match["Values"][$property["PropertyName"]] = array();

                    }

                    $match["Values"][$property["PropertyName"]][] = $property["Name"];
                
                }
                
            }
                        
        }
        
        if (!count($match["Values"])) {
            
            $match = null;
            
        } else {
         
            $match["ProfileIds"] = $idArray;
            $match["Difference"] = -1;
            $match["MatchMethod"] = null;

        }
                        
        return new FiftyOneDegreesMatchCloud($match, $this->logger);
        
    }
}
