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
#include <iostream>

#ifdef HTTP_HEADERS_PREFIXED
#define GET_HTTP_HEADER_NAME(d,i) \
							fiftyoneDegreesGetPrefixedUpperHttpHeaderName(d, i)
#else
#define GET_HTTP_HEADER_NAME(d,i) 	i >= 0 && i < d->httpHeadersCount ? \
									(d->httpHeaders + i)->headerName : NULL
#endif

using namespace std;

/**
 * Constructs a new instance of Provider for the file provided. All properties
 * supported by the source data file are available to query in Match instances
 * created by the Provider.
 * @param fileName of the data source
 */
Provider::Provider(const string &fileName) {
	init(fileName,
		FIFTYONEDEGREESPROVIDER_CACHE_SIZE,
		FIFTYONEDEGREESPROVIDER_POOL_SIZE);
}

/**
 * Constructs a new instance of Provider for the file provided. The properties
 * provided are available to query in Match instances created by the Provider.
 * @param fileName of the data source
 * @param propertyString contains comma seperated property names to be available
 * to query from associated match results.
 */
Provider::Provider(const string &fileName, const string &propertyString) {
	init(fileName,
		propertyString,
		FIFTYONEDEGREESPROVIDER_CACHE_SIZE,
		FIFTYONEDEGREESPROVIDER_POOL_SIZE);
}

/**
 * Constructs a new instance of Provider for the file provided. The properties
 * provided are available to query in Match instances created by the Provider.
 * @param fileName of the data source
 * @param propertiesArray array of property names to be available to query from
 * the associated match results.
 */
Provider::Provider(const string &fileName, vector<string> &propertiesArray) {
	init(fileName,
		propertiesArray,
		FIFTYONEDEGREESPROVIDER_CACHE_SIZE,
		FIFTYONEDEGREESPROVIDER_POOL_SIZE);
}

/**
 * Constructs a new instance of Provider for the file provided. The properties
 * provided are available to query in Match instances created by the Provider.
 * @param fileName of the data source
 * @param propertyString contains comma seperated property names to be available
 * to query from associated match results.
 * @param cacheSize the number of prior User-Agent detections to cache
 * @param poolSize the maximum number of worksets to create for the pool
 */
Provider::Provider(
		const string &fileName,
		const string &propertyString,
		int cacheSize,
		int poolSize) {
	init(fileName, propertyString, cacheSize, poolSize);
}

/**
 * Constructs a new instance of Provider for the file provided. The properties
 * provided are available to query in Match instances created by the Provider.
 * @param fileName of the data source
 * @param propertyString contains comma seperated property names to be available
 * to query from associated match results.
 * @param cacheSize the number of prior User-Agent detections to cache
 * @param poolSize the maximum number of worksets to create for the pool
 */
Provider::Provider(
		const string &fileName,
		vector<string> &propertiesArray,
		int cacheSize,
		int poolSize) {
	init(fileName, propertiesArray, cacheSize, poolSize);
}

/**
 * Constructs a new instance of Provider for the file provided. All properties
 * supported by the source data file are available to query in Match instances
 * created by the Provider.
 * @param fileName of the data source
 * @param cacheSize the number of prior User-Agent detections to cache
 * @param poolSize the maximum number of worksets to create for the pool
 */
Provider::Provider(const string &fileName, int cacheSize, int poolSize) {
	init(fileName, cacheSize, poolSize);
}

/**
 * Frees the memory used by the Provider including the cache, pool and dataset.
 */
Provider::~Provider() {
	fiftyoneDegreesProviderFree(&provider);
}

/**
 * Initialises the Provider.
 * This method should not be called as it is part of the internal logic.
 * @param fileName of the data source
 * @param cacheSize the number of prior User-Agent detections to cache
 * @param poolSize the maximum number of worksets to create for the pool
 */
void Provider::init(const string &fileName, int cacheSize, int poolSize) {
	fiftyoneDegreesDataSetInitStatus status = 
		fiftyoneDegreesInitProviderWithPropertyString(
			fileName.c_str(),
			&provider,
			NULL,
			poolSize,
			cacheSize);
	initComplete(status, fileName);
}

/**
 * Initialises the Provider.
 * This method should not be called as it is part of the internal logic.
 * @param fileName of the data source
 * @param propertyString contains comma seperated property names to be available
 * to query from associated match results.
 * @param cacheSize the number of prior User-Agent detections to cache
 * @param poolSize the maximum number of worksets to create for the pool
 */
void Provider::init(
		const string &fileName,
		const string &propertyString,
		int cacheSize,
		int poolSize) {
	fiftyoneDegreesDataSetInitStatus status = 
		fiftyoneDegreesInitProviderWithPropertyString(
			fileName.c_str(), 
			&provider,
			propertyString.c_str(),
			poolSize,
			cacheSize);
	initComplete(status, fileName);
}

/**
 * Initialises the Provider.
 * This method should not be called as it is part of the internal logic.
 * @param fileName of the data source
 * @param propertiesArray array of property names to be available to query from
 * the associated match results.
 * @param cacheSize the number of prior User-Agent detections to cache
 * @param poolSize the maximum number of worksets to create for the pool
 */
void Provider::init(
		const string &fileName,
		vector<string> &propertiesArray,
		int cacheSize,
		int poolSize) {
	fiftyoneDegreesDataSetInitStatus status;
	const char **properties = new const char*[propertiesArray.size()];
	if (properties != NULL) {
		for (unsigned int index = 0; index < propertiesArray.size(); index++) {
			properties[index] = propertiesArray[index].c_str();
		}
		status = fiftyoneDegreesInitProviderWithPropertyArray(
			fileName.c_str(),
			&provider,
			properties,
			(int)propertiesArray.size(),
			poolSize,
			cacheSize);
		initComplete(status, fileName);
		delete properties;
	}
}

/**
* Completes the process of initialising the provider by either freeing memory
* if the data set could be create, or initialising dependent data structures.
* This method should not be called as it is part of the internal logic.
* @param initStatus the status returned from the call to create the data set
* @param fileName of the data source
* @param cacheSize the number of prior User-Agent detections to cache
* @param poolSize the maximum number of worksets to create for the pool
*/
void Provider::initComplete(
	fiftyoneDegreesDataSetInitStatus initStatus,
	const string &fileName) {
	if (initStatus != DATA_SET_INIT_STATUS_SUCCESS)	{
		initExecption(initStatus, fileName);
	}
	else {
		initAvailableProperites();
		initHttpHeaders();
	}
}

/**
* Checks the initialisation status result and throws a C++ exception if the
* result is anything other than success. The text of the exception can be used
* by the high level language to provide a message to the user.
* This method should not be called as it is part of the internal logic.
* @param initStatus status enum value
* @param fileName of the data source
*/
void Provider::initExecption(
	fiftyoneDegreesDataSetInitStatus initStatus,
	const string &fileName) {
	stringstream message;
	switch (initStatus) {
	case DATA_SET_INIT_STATUS_SUCCESS:
		break;
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
	case DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS:
		break;
	case DATA_SET_INIT_STATUS_NULL_POINTER:
		break;
	default:
	case DATA_SET_INIT_STATUS_NOT_SET:
		throw runtime_error("Could not create data set from file.");
		break;
	}
}

/**
 * Populates the httpHeaders vectors for the Provider. The HTTP_HEADERS_PREFIXED
 * definition can be used to force the Provider to use HTTP_ prefixed upper
 * case header name keys. Perl and PHP are examples of such languages where this
 * directive is required.
 * This method should not be called as it is part of the internal logic.
 */
void Provider::initHttpHeaders() {
	for (int httpHeaderIndex = 0;
		httpHeaderIndex < provider.activePool->dataSet->httpHeadersCount;
		httpHeaderIndex++) {
		httpHeaders.insert(
			httpHeaders.end(),
			string(GET_HTTP_HEADER_NAME(provider.activePool->dataSet, httpHeaderIndex)));
	}
}

/**
 * Populates the availableProperties vector with all properties available in
 * source data set. This may return fewer properties than the number requested
 * at construction where the data set source file does not contain details
 * of the property name requested.
 * This method should not be called as it is part of the internal logic.
 */
void Provider::initAvailableProperites() {
	const fiftyoneDegreesAsciiString *propertyName;
	for (int requiredPropetyIndex = 0;
		requiredPropetyIndex < provider.activePool->dataSet->requiredPropertyCount;
			requiredPropetyIndex++) {
		propertyName = fiftyoneDegreesGetString(
			provider.activePool->dataSet,
			provider.activePool->dataSet->requiredProperties[requiredPropetyIndex]->nameOffset);
		availableProperties.insert(
			availableProperties.end(),
			string(&propertyName->firstByte));
	}
}

/**
 * Initialises a new match instance for the provider ready to perform device
 * detection.
 * This method should not be called as it is part of the internal logic.
 */
void Provider::initMatch(Match *match) {
	match->ws = fiftyoneDegreesProviderWorksetGet(&this->provider);
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
 * @returns the name of the data set used contained in the source file.
 */
string Provider::getDataSetName() {
	string result;
	result.assign(&fiftyoneDegreesGetString(
		provider.activePool->dataSet,
		provider.activePool->dataSet->header.nameOffset)->firstByte);
	return result;
}

/**
 * @returns the version format of the data file and API.
 */
string Provider::getDataSetFormat() {
	string result;
	result.assign(&fiftyoneDegreesGetString(
		provider.activePool->dataSet,
		provider.activePool->dataSet->header.formatOffset)->firstByte);
	return result;
}

/**
 * @returns the date that 51Degrees published the data file.
 */
string Provider::getDataSetPublishedDate() {
	stringstream stream;
	stream << provider.activePool->dataSet->header.published.year << "-"
		   << (int)provider.activePool->dataSet->header.published.month << "-"
		   << (int)provider.activePool->dataSet->header.published.day;
	return stream.str();
}

/**
 * @returns the date that 51Degrees will publish an updated data file.
 */
string Provider::getDataSetNextUpdateDate() {
	stringstream stream;
	stream << provider.activePool->dataSet->header.nextUpdate.year << "-"
		   << (int)provider.activePool->dataSet->header.nextUpdate.month << "-"
		   << (int)provider.activePool->dataSet->header.nextUpdate.day;
	return stream.str();
}

/**
 * @returns the number of Signatures the data set contains.
 */
int Provider::getDataSetSignatureCount() {
	return provider.activePool->dataSet->header.signatures.count;
}

/**
 * @returns the number of device combinations the data set contains.
 */
int Provider::getDataSetDeviceCombinations() {
	return provider.activePool->dataSet->header.deviceCombinations;
}

/**
 * Using the HTTP header name and values pairs sets the detected device
 * information in the workset provided.
 * @param ws instance of a workset to use for the match
 * @param headers name and value pairs to use for device detection
 */
void Provider::matchForHttpHeaders(
		fiftyoneDegreesWorkset *ws,
		const map<string, string> *headers) {
	int dataSetHeaderIndex = 0;
	const char *httpHeaderName = GET_HTTP_HEADER_NAME(
		provider.activePool->dataSet,
		dataSetHeaderIndex);
	ws->importantHeadersCount = 0;
	while (httpHeaderName != NULL) {
		map<string, string>::const_iterator httpHeaderValue =
			headers->find(string(httpHeaderName));
		if (httpHeaderValue != headers->end()) {
			ws->importantHeaders[ws->importantHeadersCount].header =
				ws->dataSet->httpHeaders + dataSetHeaderIndex;
			ws->importantHeaders[ws->importantHeadersCount].headerValue =
				httpHeaderValue->second.c_str();
			ws->importantHeaders[ws->importantHeadersCount].headerValueLength =
				(int)httpHeaderValue->second.length();
			ws->importantHeadersCount++;
		}
		dataSetHeaderIndex++;
		httpHeaderName = GET_HTTP_HEADER_NAME(provider.activePool->dataSet, dataSetHeaderIndex);
	}
	fiftyoneDegreesMatchForHttpHeaders(ws);
}

/**
 * Completes the result map with all properties supported by the Provider and
 * their values for the detection results contained in the workset provided.
 * This method should not be called as it is part of the internal logic.
 * @param ws instance of a workset configured with the results of the match
 * @param result to place the properties and values
 */
void Provider::buildArray(
		fiftyoneDegreesWorkset *ws,
		map<string, vector<string> > *result) {
	string *propertyName;
	vector<string> *values;
	int propertyIndex, valueIndex;
	for (propertyIndex = 0;
		propertyIndex < ws->dataSet->requiredPropertyCount;
		propertyIndex++) {
		if (fiftyoneDegreesSetValues(ws, propertyIndex) > 0) {
			propertyName = new string(fiftyoneDegreesGetPropertyName(
				ws->dataSet,
				*(ws->dataSet->requiredProperties + propertyIndex)));
			values = &(result->operator[](*propertyName));
			for (valueIndex = 0; valueIndex < ws->valuesCount; valueIndex++) {
				vector<string>::iterator it = values->end();
				string value = string((const char*)&fiftyoneDegreesGetString(
					ws->dataSet,
					ws->values[valueIndex]->nameOffset)->firstByte);
				values->insert(it, value);
			}
		}
	}
}

/**
 * Completes device detection for the User-Agent provided.
 * @param User-Agent whose results need to be obtained
 * @returns new Match instance configured to provide access to the results
 */
Match* Provider::getMatch(const char* userAgent) {
	Match *result = new Match();
	initMatch(result);
	fiftyoneDegreesMatch(result->ws, userAgent);
	return result;
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
	Match *result = new Match();
	initMatch(result);
	matchForHttpHeaders(result->ws, &headers);
	return result;
}

/**
 * Completes device detection for the User-Agent provided.
 * @param User-Agent whose results need to be obtained
 * @returns a Map and values array containing all the properties available
 */
map<string, vector<string> >& Provider::getMatchMap(const char *userAgent) {
	map<string, vector<string> > *result = new map<string, vector<string> >();
	fiftyoneDegreesWorkset *ws = fiftyoneDegreesProviderWorksetGet(&provider);
	fiftyoneDegreesMatch(ws, userAgent);
	buildArray(ws, result);
	fiftyoneDegreesWorksetRelease(ws);
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
	fiftyoneDegreesWorkset *ws = fiftyoneDegreesProviderWorksetGet(&provider);
	matchForHttpHeaders(ws, &headers);
	buildArray(ws, result);
	fiftyoneDegreesWorksetRelease(ws);
	return *result;
}

/**
 * Returns a JSON string containing all available properties for the User-Agent.
 * @param User-Agent whose results need to be obtained
 * @returns a JSON string with property and value pairs
 */
string Provider::getMatchJson(const char* userAgent) {
	string result;
	fiftyoneDegreesWorkset *ws = fiftyoneDegreesProviderWorksetGet(&provider);
	fiftyoneDegreesMatch(ws, userAgent);
	char *json = fiftyoneDegreesJSONCreate(ws);
	fiftyoneDegreesProcessDeviceJSON(ws, json);
	result.assign(json);
	fiftyoneDegreesJSONFree(json);
	fiftyoneDegreesWorksetRelease(ws);
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
	fiftyoneDegreesWorkset *ws = fiftyoneDegreesProviderWorksetGet(&provider);
	char *json = fiftyoneDegreesJSONCreate(ws);
	matchForHttpHeaders(ws, &headers);
	fiftyoneDegreesProcessDeviceJSON(ws, json);
	result.assign(json);
	fiftyoneDegreesJSONFree(json);
	fiftyoneDegreesWorksetRelease(ws);
	return result;
}

/**
 * Completes device detection for the device id provided.
 * @param device id used for the match
 * @returns new Match instance configured to provide access to the results
 */
Match* Provider::getMatchForDeviceId(const char *deviceId) {
	Match *result = new Match();
	initMatch(result);
	fiftyoneDegreesMatchForDeviceId(result->ws, deviceId);
	return result;
}

/**
 * Completes device detection for the device id provided.
 * @param device id used for the match
 * @returns new Match instance configured to provide access to the results
 */
Match* Provider::getMatchForDeviceId(const string& deviceId) {
	return getMatchForDeviceId(deviceId.c_str());
}

/**
 * Finds all profiles in the data set which contain the property
 * value pair provided.
 * @param propertyName used to search the profiles.
 * @param valueName used to search the profiles.
 * @returns new Profiles instance configured to provide access to the results.
 */
Profiles* Provider::findProfiles(const char *propertyName, const char *valueName) {
	Profiles *profiles = new Profiles();
	profiles->profiles = fiftyoneDegreesFindProfiles(provider.activePool->dataSet, propertyName, valueName);
	return profiles;
}

/**
 * Finds all profiles in the data set which contain the property
 * value pair provided.
 * @param propertyName used to search the profiles.
 * @param valueName used to search the profiles.
 * @returns new Profiles instance configured to provide access to the results.
 */
Profiles* Provider::findProfiles(const string &propertyName, const string &valueName) {
	return findProfiles(propertyName.c_str(), valueName.c_str());
}

/**
 * Finds all profiles within the provided Profiles object which contain
 * the property value pair provided.
 * @param propertyName used to search the profiles.
 * @param valueName used to search the profiles.
 * @param profiles object to perform the search in.
 * @returns new Profiles instance configured to provide access to the results.
 */
Profiles* Provider::findProfiles(const char *propertyName, const char *valueName, Profiles *profiles) {
	Profiles *returnprofiles = new Profiles();
	returnprofiles->profiles = fiftyoneDegreesFindProfilesInProfiles(provider.activePool->dataSet, propertyName, valueName, profiles->profiles);
	return returnprofiles;

}

/**
 * Finds all profiles within the provided Profiles object which contain
 * the property value pair provided.
 * @param propertyName used to search the profiles.
 * @param valueName used to search the profiles.
 * @param profiles object to perform the search in.
 * @returns new Profiles instance configured to provide access to the results.
 */
Profiles* Provider::findProfiles(const string &propertyName, const string &valueName, Profiles *profiles) {
	return findProfiles(propertyName.c_str(), valueName.c_str(), profiles);
}

/**
 * Initiates the data set reload process from the same file location that was 
 * used to create the current dataset. New dataset will be initialised with 
 * exactly the same set of properties, cache size and number of worksets in 
 * the workset pool.
 *
 * Function is not thread safe.
 */
void Provider::reloadFromFile() {
	fiftyoneDegreesProviderReloadFromFile(&provider);
}
