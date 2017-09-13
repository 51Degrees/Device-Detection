/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright 2017 51Degrees Mobile Experts Limited, 5 Charlotte Close,
 * Caversham, Reading, Berkshire, United Kingdom RG4 7BY
 *
 * This Source Code Form is the subject of the following patents and patent
 * applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
 * Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
 * European Patent No. 2871816;
 * European Patent Application No. 17184134.9;
 * United States Patent Nos. 9,332,086 and 9,350,823; and
 * United States Patent Application No. 15/686,066.
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
		initException(initStatus, fileName);
	}
	else {
		initAvailableproperties();
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
void Provider::initException(
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
	case DATA_SET_INIT_STATUS_NULL_POINTER:
		throw runtime_error("Null pointer to the existing dataset or memory"
			" location.");
		break;
	case DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS:
		throw runtime_error("Allocated continuous memory containing "
			"51Degrees data file appears to be smaller than expected. Most"
			" likely because the data file was not fully loaded into the "
			"allocated memory.");
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
void Provider::initAvailableproperties() {
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

Match* Provider::getMatchForHttpHeaders(const map<string, string>& headers) {
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

/**
* Initiates the data set reload process from the memory location supplied.
* New dataset will be initialised with exactly the same set of properties.
*
* Function is not thread safe.
* @param source pointer to the dataset in memory.
* @param length of the dataset in memory.
*/
void Provider::reloadFromMemory(const char *source, int length) {
	fiftyoneDegreesProviderReloadFromMemory(&provider, (void*)source, (long)length);
}

/**
* Initiates the data set reload process from the memory location supplied.
* New dataset will be initialised with exactly the same set of properties.
*
* Function is not thread safe.
* @param source pointer to the dataset in memory.
* @param length of the dataset in memory.
*/
void Provider::reloadFromMemory(const string &source, int length) {
	reloadFromMemory(source.c_str(), length);
}

/**
* Returns the number of times the cache fetch has found what it's
* looking for.
* @returns int number of cache hits.
*/
int Provider::getCacheHits() {
	return provider.activePool->cache->hits;
}

/**
* Returns the number of times the cache fetch has not found what it's
* looking for. The cache fetch function is called a second time to insert
* a value that was not found.
* @returns int number of cache misses.
*/
int Provider::getCacheMisses() {
	return provider.activePool->cache->misses;
}

/**
* Return the maximum number of iterations a cache fetch could take to find
* a match. This is the depth of the binary tree.
*/
int Provider::getCacheMaxIterations() {
	return provider.activePool->cache->maxIterations;
}

// The section below is entirely for testing purposes and is not written
// for use in a production environment.

// The actual size allocated for the provider.
static int64_t _actualSize;

/**
* Overrides the malloc function so that the size being allocated can be
* added up.
* @param size to allocate in memory.
* @returns void* pointer to the memory allocated.
*/
static void *validateMalloc(size_t __size) {
	_actualSize += (int64_t)__size;
	return malloc(__size);
}

/**
* Modified constructor for the provider. If validate is true, then the
* calculated value of the memory needed is compared to the actual amount
* of memory allocated for the provider and an error thrown if the value is
* too low.
*
* This function is used when testing the getProviderSize funtion and is NOT
* THREAD SAFE as it accesses a static variable. For this reason it SHOULD
* NOT BE USED IN PRODUCTION.
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
	int poolSize,
	bool validate) {
	stringstream message;
	int64_t difference;

	if (validate == true)
	{
		// Get the difference between the calculated memory needed and the actual
		// memory userd.
		difference = initWithValidate(fileName, propertyString, cacheSize, poolSize);

		// If the calculated memory is less than the actual memory then throw
		// an error.
		// Note: this will always be a slight overestimate as calculating the
		// exact number of unique HTTP headers requires reading in the data
		// file. For this reason the maximum is used.
		if (difference < 0) {
			message << "Predicted memory usage is below the actual usage by "
				<< (0 - difference)
				<< " bytes.";
			throw runtime_error(message.str());
		}
	}
	else {
		// Validate is false so divert to the standard constructor.
		Provider(fileName, propertyString, cacheSize, poolSize);
	}
}

/**
* Modified provider init function. This uses the standard init function,
* but uses a modified malloc function to determine the size. It also uses
* the getProviderSize function to get the estimated size and return the
* difference.
*
* This function is NOT THREAD SAFE so should be used appropriately.
* @param fileName of the data source
* @param propertyString contains comma seperated property names to be available
* to query from associated match results.
* @param cacheSize the number of prior User-Agent detections to cache
* @param poolSize the maximum number of worksets to create for the pool
*/
int64_t Provider::initWithValidate(
	const string &fileName,
	const string &propertyString,
	int cacheSize,
	int poolSize) {
	int64_t predictedSize;

	// Reset the actual size parameter as it is global and may have been set
	// before.
	_actualSize = 0;

	// Use the getProviderSize function to get the predicted size that the
	// provider will need.
	predictedSize = (int64_t)fiftyoneDegreesGetProviderSizeWithPropertyString(fileName.c_str(), propertyString.c_str(), poolSize, cacheSize);

	// Set the malloc function to use the function that increments _actualSize
	// by the amount being allocated.
	fiftyoneDegreesMalloc = validateMalloc;

	// Use the standard init function to initialise the provider. This is the
	// same function used by the standard constructor, however every memory
	// allocation will be counted using the validateMalloc function.
	init(fileName, propertyString, cacheSize, poolSize);

	// Revert the malloc function so that future calls do not use the
	// validateMalloc function.
	fiftyoneDegreesMalloc = malloc;

	// Return the difference between the predicted and actual sizes. A positive
	// number is an overestimate, and a negative number is an underestimate.
	return predictedSize - _actualSize;
}

/**
*@return true if the implementation is thread safe, otherwise false.
*/
bool Provider::getIsThreadSafe() {
	return fiftyoneDegreesGetIsThreadSafe() == 1;
}
