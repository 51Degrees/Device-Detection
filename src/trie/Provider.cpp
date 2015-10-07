/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright 2015 51Degrees Mobile Experts Limited, 5 Charlotte Close,
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
 ********************************************************************** */

#include "Provider.hpp"

using namespace std;

#define JSON_BUFFER_LENGTH 50000

#ifdef HTTP_HEADERS_PREFIXED
#define GET_HTTP_HEADER_NAME fiftyoneDegreesGetPrefixedUpperHttpHeaderName
#else
#define GET_HTTP_HEADER_NAME fiftyoneDegreesGetHttpHeaderNamePointer
#endif

/**
 * Constructs a new instance of Provider for the file provided. All properties
 * supported by the source data file are available to query in Match instances
 * created by the Provider.
 *
 * Only one Provider can be created per process.
 *
 * @param fileName of the data source
 */
Provider::Provider(const string &fileName) {
	init(fileName);
}

/**
 * Constructs a new instance of Provider for the file provided. The properties
 * provided are available to query in Match instances created by the Provider.
 *
 * Only one Provider can be created per process.
 *
 * @param fileName of the data source
 * @param propertyString contains comma seperated property names to be available
 * to query from associated match results.
 */
Provider::Provider(const string &fileName, const string &propertyString) {
	init(fileName, propertyString);
}

/**
 * Constructs a new instance of Provider for the file provided. The properties
 * provided are available to query in Match instances created by the Provider.
 *
 * Only one Provider can be created per process.
 *
 * @param fileName of the data source
 * @param propertiesArray array of property names to be available to query from
 * the associated match results.
 */
Provider::Provider(const string &fileName, vector<string> &propertiesArray) {
	init(fileName, propertiesArray);
}

/**
 * Frees the memory used by the Provider.
 */
Provider::~Provider()
{
    fiftyoneDegreesDestroy();
}

/**
 * Checks the initialisation status result and throws a C++ exception if the
 * result is anything other than success. The text of the exception can be used
 * by the high level language to provide a message to the user.
 * @param initStatus status enum value
 * @param fileName of the data source
 */
void Provider::initExecption(
		fiftyoneDegreesDataSetInitStatus initStatus,
		const string &fileName) {
	stringstream message;
	switch(initStatus) {
		case DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY:
			throw runtime_error("Insufficient memory allocated.");
			break;
		case DATA_SET_INIT_STATUS_CORRUPT_DATA:
			throw runtime_error("The data was not in the correct format. Check "
				"the data file is uncompressed.");
			break;
		case DATA_SET_INIT_STATUS_INCORRECT_VERSION:
			throw runtime_error("The data is an unsupported version. Check you "
				"have the latest data and API.");
			break;
		case DATA_SET_INIT_STATUS_FILE_NOT_FOUND:
			message << "The data file '" << fileName << "' could not be found. "
				"Check the file path and that the program has sufficient read "
				"permissions.";
			throw invalid_argument(message.str());
			break;
		default:
		case DATA_SET_INIT_STATUS_NOT_SET:
			throw runtime_error("Could not create data set from file.");
			break;
	}
}

/**
 * Initialises the Provider.
 * @param fileName of the data source
 */
void Provider::init(const string &fileName) {
	initComplete(fiftyoneDegreesInitWithPropertyString(
        fileName.c_str(),
        NULL),
        fileName);
}

/**
 * Initialises the Provider.
 * @param fileName of the data source
 * @param propertyString contains comma seperated property names to be available
 * to query from associated match results.
 */
void Provider::init(const string &fileName, const string &propertyString) {
	initComplete(fiftyoneDegreesInitWithPropertyString(
        fileName.c_str(),
        propertyString.c_str()),
        fileName);
}

/**
 * Initialises the Provider.
 * @param fileName of the data source
 * @param propertiesArray array of property names to be available to query from
 * the associated match results.
 */
void Provider::init(const string &fileName, vector<string> &propertiesArray) {
	fiftyoneDegreesDataSetInitStatus initStatus = DATA_SET_INIT_STATUS_NOT_SET;
	const char **properties = new const char*[propertiesArray.size()];
	if (properties != NULL) {
        for (unsigned int index = 0; index < propertiesArray.size(); index++) {
            properties[index] = propertiesArray[index].c_str();
        }
        initStatus = fiftyoneDegreesInitWithPropertyArray(
            fileName.c_str(),
            properties,
            (int)propertiesArray.size());
        delete properties;
	}
    initComplete(initStatus, fileName);
}

/**
 * Completes the process of initialising the provider by either freeing memory
 * if the data set could be create, or initialising dependent data structures.
 * @param initStatus the status returned from the call to create the data set
 * @param fileName of the data source
 */
void Provider::initComplete(
		fiftyoneDegreesDataSetInitStatus initStatus,
		const string &fileName) {
	if (initStatus != DATA_SET_INIT_STATUS_SUCCESS)	{
		fiftyoneDegreesDestroy();
		initExecption(initStatus, fileName);
	}
	else {
		initAvailableProperites();
		initHttpHeaders();
	}
}

/**
 * Populates the httpHeaders vectors for the Provider. The HTTP_HEADERS_PREFIXED
 * definition can be used to force the Provider to use HTTP_ prefixed upper
 * case header name keys. Perl and PHP are examples of such languages where this
 * directive is required.
 */
void Provider::initHttpHeaders() {
	for (int httpHeaderIndex = 0;
		httpHeaderIndex < fiftyoneDegreesGetHttpHeaderCount();
		httpHeaderIndex++) {
        httpHeaders.insert(
            httpHeaders.end(),
			string(GET_HTTP_HEADER_NAME(httpHeaderIndex)));
	}
}

/**
 * Populates the availableProperties vector with all properties available in
 * source data set. This may return fewer properties than the number requested
 * at construction where the data set source file does not contain details
 * of the property name requested.
 */
void Provider::initAvailableProperites() {
	const char *propertyName;
    for (int requiredPropetyIndex = 0;
        requiredPropetyIndex < fiftyoneDegreesGetRequiredPropertiesCount();
        requiredPropetyIndex++) {
        propertyName = fiftyoneDegreesGetRequiredPropertiesNames()[
        	requiredPropetyIndex];
        availableProperties.insert(
            availableProperties.end(),
            string(propertyName));
    }
}

/**
 * @returns a list of HTTP headers device detection considers are important.
 */
vector<string> Provider::getHttpHeaders() {
	return httpHeaders;
}

/**
 * @returns a list of the properties Match instances created from the Provider
 * can return.
 */
vector<string> Provider::getAvailableProperties() {
    return availableProperties;
}

/**
 * @returns the name of the data set used contained in the source file. Always
 * returns "Trie".
 */
string Provider::getDataSetName() {
    string result;
    result.assign("Trie");
    return result;
}

/**
 * @returns the version format of the data file and API.
 */
string Provider::getDataSetFormat() {
    string result;
    result.assign("3.2");
    return result;
}

/**
 * TODO - Adds this meta data to the next version of the Trie data file.
 * @returns the date that 51Degrees published the data file.
 */
string Provider::getDataSetPublishedDate() {
    stringstream stream;
    stream << 2015 << "-"
        << 1 << "-"
        << 1;
    return stream.str();
}

/**
 * @returns the date that 51Degrees will publish an updated data file.
 */
string Provider::getDataSetNextUpdateDate() {
    stringstream stream;
    stream << 2015 << "-"
        << 1 << "-"
        << 1;
    return stream.str();
}

/**
 * @returns the number of Signatures the data set contains.
 */
int Provider::getDataSetSignatureCount() {
    return 0;
}

/**
 * @returns the number of device combinations the data set contains.
 */
int Provider::getDataSetDeviceCombinations() {
    return 0;
}

/**
 * Using the HTTP header name and values pairs returns a DeviceOffsets
 * structure with a HTTP header index and device offset for each relevant
 * header..
 * @param headers name and value pairs to use for device detection
 * @returns a fresh DeviceOffsets structure set for the headers provided
 */
fiftyoneDegreesDeviceOffsets* Provider::matchForHttpHeaders(
		const map<string, string> *headers) {
	int headerIndex = 0;
	fiftyoneDegreesDeviceOffsets* offsets = fiftyoneDegreesCreateDeviceOffsets();
	const char *httpHeaderName = GET_HTTP_HEADER_NAME(headerIndex);
	while (httpHeaderName != NULL) {
		map<string, string>::const_iterator httpHeaderValue =
			headers->find(string(httpHeaderName));
		if (httpHeaderValue != headers->end()) {
			fiftyoneDegreesSetDeviceOffset(
				httpHeaderValue->second.c_str(),
				headerIndex,
				&offsets->firstOffset[offsets->size]);
            offsets->size++;
		}
		headerIndex++;
		httpHeaderName = GET_HTTP_HEADER_NAME(headerIndex);
	}
	return offsets;
}

/**
 * Completes the result map with all properties supported by the Provider and
 * their values for the detection results contained in the device offsets
 * provided.
 * @param offsets for HTTP headers and devices
 * @param result to place the properties and values
 */
void Provider::buildArray(
		fiftyoneDegreesDeviceOffsets *offsets,
		map<string, vector<string> > *result) {
	int requiredPropertyIndex;
	string *propertyName;
	for (requiredPropertyIndex = 0;
        requiredPropertyIndex < fiftyoneDegreesGetRequiredPropertiesCount();
        requiredPropertyIndex++) {
        const char *value = fiftyoneDegreesGetValuePtrFromOffsets(
        	offsets,
        	requiredPropertyIndex);
        if (value != NULL) {
            propertyName = new string(
            	fiftyoneDegreesGetRequiredPropertiesNames()[
            		requiredPropertyIndex]);
            vector<string> *values = &(result->operator[](*propertyName));
            values->insert(values->begin(), string(value));
        }
	}
}

/**
 * Completes the result map with all properties supported by the Provider and
 * their values for the device offset provided.
 * @param offset to the device to use to populate the result map
 * @param result to place the properties and values
 */
void Provider::buildArray(int offset, map<string, vector<string> > *result) {
	int requiredPropertyIndex;
	string *propertyName;
	for (requiredPropertyIndex = 0;
        requiredPropertyIndex < fiftyoneDegreesGetRequiredPropertiesCount();
        requiredPropertyIndex++) {
        const char *value = fiftyoneDegreesGetValue(
        	offset,
        	requiredPropertyIndex);
        if (value != NULL) {
            propertyName = new string(
            	fiftyoneDegreesGetRequiredPropertiesNames()[
            		requiredPropertyIndex]);
            vector<string> *values = &(result->operator[](*propertyName));
            values->insert(values->begin(), string(value));
        }
	}
}

/**
 * Completes device detection for the User-Agent provided.
 * @param User-Agent whose results need to be obtained
 * @returns new Match instance configured to provide access to the results
 */
Match* Provider::getMatch(const char* userAgent) {
	fiftyoneDegreesDeviceOffsets* offsets = new fiftyoneDegreesDeviceOffsets();
	offsets->size = 1;
	offsets->firstOffset = new fiftyoneDegreesDeviceOffset[1];
	fiftyoneDegreesSetDeviceOffset(userAgent, 0, offsets->firstOffset);
    return new Match(offsets);
}

/**
 * Completes device detection for the User-Agent provided.
 * @param User-Agent whose results need to be obtained
 * @returns new Match instance configured to provide access to the results
 */
Match* Provider::getMatch(const string& userAgent) {
    return getMatch(userAgent.c_str());
}

/**
 * Completes device detection for the User-Agent provided.
 * @param headers HTTP headers to use to detect the device and return a match
 * @returns new Match instance configured to provide access to the results
 */
Match* Provider::getMatch(const map<string, string>& headers) {
    return new Match(matchForHttpHeaders(&headers));
}

/**
 * Completes device detection for the User-Agent provided.
 * @param User-Agent whose results need to be obtained
 * @returns a Map and values array containing all the properties available
 */
map<string, vector<string> >& Provider::getMatchMap(const char *userAgent) {
	map<string, vector<string> > *result = new map<string, vector<string> >();
	buildArray(fiftyoneDegreesGetDeviceOffset(userAgent), result);
	return *result;
}

/**
 * Completes device detection for the collection of HTTP headers provided.
 * @param User-Agent whose results need to be obtained
 * @returns a Map and values array containing all the properties available
 */
map<string, vector<string> >& Provider::getMatchMap(const string &userAgent) {
    return getMatchMap(userAgent.c_str());
}

/**
 * Completes device detection for the collection of HTTP headers provided.
 * @param headers HTTP headers to use to detect the device and return a match
 * @returns a Map and values array containing all the properties available
 */
map<string, vector<string> >& Provider::getMatchMap(
		const map<string, string> &headers) {
	map<string, vector<string> > *result = new map<string, vector<string> >();
	fiftyoneDegreesDeviceOffsets *offsets = matchForHttpHeaders(&headers);
	buildArray(offsets, result);
	delete offsets->firstOffset;
	delete offsets;
	return *result;
}

/**
 * Returns a JSON string containing all available properties for the User-Agent.
 * @param User-Agent whose results need to be obtained
 * @returns a JSON string with property and value pairs
 */
string Provider::getMatchJson(const char* userAgent) {
    string result;
    char *json = new char[JSON_BUFFER_LENGTH];
    fiftyoneDegreesProcessDeviceJSON(
        fiftyoneDegreesGetDeviceOffset(userAgent),
        json,
        JSON_BUFFER_LENGTH);
    result.assign(json);
    delete json;
    return result;
}

/**
 * Returns a JSON string containing all available properties for the User-Agent.
 * @param User-Agent whose results need to be obtained
 * @returns a JSON string with property and value pairs
 */
string Provider::getMatchJson(const string& userAgent) {
    return getMatchJson(userAgent.c_str());
}

/**
 * Returns a JSON string containing all available properties for the HTTP header
 * pairs.
 * @param headers HTTP headers to use to detect the device and return JSON
 * @returns a JSON string with property and value pairs
 */
string Provider::getMatchJson(const map<string, string>& headers) {
	string result;
	char *json = new char[JSON_BUFFER_LENGTH];
    fiftyoneDegreesDeviceOffsets *offsets = matchForHttpHeaders(&headers);
    fiftyoneDegreesProcessDeviceOffsetsJSON(
        offsets,
        json,
        JSON_BUFFER_LENGTH);
	result.assign(json);
	delete json;
	return result;
}
