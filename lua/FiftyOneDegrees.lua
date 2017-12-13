--[[
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
--]]
-- Define the wrapped provider to return.
local returnedProvider={}
local fod
local provider
-- Store the config locally.
local config=...

-- Initialise the provider with the supplied config.
if string.match(config.dataFile, "%.dat") then
	fod=require("FiftyOneDegreesPatternV3")
elseif string.match(config.dataFile, "%.trie") then
	fod=require("FiftyOneDegreesTrieV3")
else
	error("Invalid data file extension (should be .dat or .trie)");
end

provider=fod.Provider(config.dataFile, config.properties)

-- Returns the headers from a headers table.
local function getRequestHeaders(headers)
    -- Get the header names and make a new map
    -- to store the values in.
    headerNames=provider:getHttpHeaders()
    matchedHeaders=fod.MapStringString()

    -- Get the important values from the header table.
    for i=0,headerNames:size()-1,1 do
        if (headers[headerNames[i]]) then
            matchedHeaders:set(headerNames[i],headers[headerNames[i]])
        elseif (headers[string.lower(headerNames[i])]) then
            -- Could be in lowercase.
            matchedHeaders:set(headerNames[i],headers[string.lower(headerNames[i])])
        end
    end
    return matchedHeaders
end

-- Wrap the getMatch function to deal with header arrays and request object.
function returnedProvider:getMatch(input)
    -- Input is a User-Agent, so do a match.
    if (type(input) == "string") then
        return provider:getMatch(input)
    end
    
    -- Input is a table, so match with all the headers
    if (type(input) == "table") then
        headers=getRequestHeaders(input)        
        return provider:getMatch(headers)
    end
end

function returnedProvider:getMatchForDeviceId(id)
	return provider:getMatchForDeviceId(id)
end

function returnedProvider:findProfiles(...)
	return provider:findProfiles(...)
end

-- Returns the properties available in the initialised provider.
function returnedProvider:getAvailableProperties()
    -- Get the available properties from the provider.
    propertiesObj=provider:getAvailableProperties()
    -- Convert the properties to a Lua table.
    properties={}
    for i=0,propertiesObj:size()-1,1 do
        properties[i]=propertiesObj[i]
    end
    return properties
end

function returnedProvider:getDataSetName()
	return provider:getDataSetName()
end

-- Return the wrapped provider.
return returnedProvider
