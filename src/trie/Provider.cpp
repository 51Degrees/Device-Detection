/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright 2017 51Degrees Mobile Experts Limited, 5 Charlotte Close,
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

/**
 * If required, use prefixed HTTP headers. This is required for some
 * applications which format header names differently.
 */
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
    fiftyoneDegreesProviderFree(&provider);
}

/**
 * Checks the initialisation status result and throws a C++ exception if the
 * result is anything other than success. The text of the exception can be used
 * by the high level language to provide a message to the user.
 * @param initStatus status enum value
 * @param fileName of the data source
 */
void Provider::initException(
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
 * Initialises the Provider.
 * @param fileName of the data source
 */
void Provider::init(const string &fileName) {
	initComplete(fiftyoneDegreesInitProviderWithPropertyString(
        fileName.c_str(),
		&provider,
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
	initComplete(fiftyoneDegreesInitProviderWithPropertyString(
        fileName.c_str(),
		&provider,
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
        initStatus = fiftyoneDegreesInitProviderWithPropertyArray(
            fileName.c_str(),
			&provider,
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
		initException(initStatus, fileName);
	}
	else {
		initAvailableproperties();
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
		httpHeaderIndex < fiftyoneDegreesGetHttpHeaderCount(provider.active->dataSet);
		httpHeaderIndex++) {
        httpHeaders.insert(
            httpHeaders.end(),
			string(GET_HTTP_HEADER_NAME(provider.active->dataSet, httpHeaderIndex)));
	}
}

/**
 * Populates the availableProperties vector with all properties available in
 * source data set. This may return fewer properties than the number requested
 * at construction where the data set source file does not contain details
 * of the property name requested.
 */
void Provider::initAvailableproperties() {
	const char *propertyName;
    for (int requiredPropetyIndex = 0;
		requiredPropetyIndex < fiftyoneDegreesGetRequiredPropertiesCount(provider.active->dataSet);
        requiredPropetyIndex++) {
		propertyName = fiftyoneDegreesGetRequiredPropertiesNames(provider.active->dataSet)[
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
	stringstream stream;
	stream << *provider.active->dataSet->version;
	return stream.str();
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
	fiftyoneDegreesDeviceOffsets* offsets = fiftyoneDegreesProviderCreateDeviceOffsets(&provider);
	const char *httpHeaderName = GET_HTTP_HEADER_NAME(provider.active->dataSet, headerIndex);
	while (httpHeaderName != NULL) {
		map<string, string>::const_iterator httpHeaderValue =
			headers->find(string(httpHeaderName));
		if (httpHeaderValue != headers->end()) {
			fiftyoneDegreesSetDeviceOffset(
				provider.active->dataSet,
				httpHeaderValue->second.c_str(),
				headerIndex,
				&offsets->firstOffset[offsets->size]);
            offsets->size++;
		}
		headerIndex++;
		httpHeaderName = GET_HTTP_HEADER_NAME(provider.active->dataSet, headerIndex);
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
		requiredPropertyIndex < fiftyoneDegreesGetRequiredPropertiesCount(provider.active->dataSet);
        requiredPropertyIndex++) {
        const char *value = fiftyoneDegreesGetValuePtrFromOffsets(
			provider.active->dataSet,
        	offsets,
        	requiredPropertyIndex);
        if (value != NULL) {
            propertyName = new string(
				fiftyoneDegreesGetRequiredPropertiesNames(provider.active->dataSet)[
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
		requiredPropertyIndex < fiftyoneDegreesGetRequiredPropertiesCount(provider.active->dataSet);
        requiredPropertyIndex++) {
        const char *value = fiftyoneDegreesGetValue(
			provider.active->dataSet,
        	offset,
        	requiredPropertyIndex);
        if (value != NULL) {
            propertyName = new string(
				fiftyoneDegreesGetRequiredPropertiesNames(provider.active->dataSet)[
            		requiredPropertyIndex]);
            vector<string> *values = &(result->operator[](*propertyName));
            values->insert(values->begin(), string(value));
        }
	}
}

void Provider::initMatch(Match *match) {
	match->dataSet = match->offsets->active->dataSet;
}
/**
 * Completes device detection for the User-Agent provided.
 * @param User-Agent whose results need to be obtained
 * @returns new Match instance configured to provide access to the results
 */
Match* Provider::getMatch(const char* userAgent) {
	fiftyoneDegreesDeviceOffsets* offsets = fiftyoneDegreesProviderCreateDeviceOffsets(&provider);
	offsets->size = 1;
	offsets->firstOffset = new fiftyoneDegreesDeviceOffset[1];
	fiftyoneDegreesSetDeviceOffset(provider.active->dataSet, userAgent, 0, offsets->firstOffset);
    Match *result = new Match(offsets);
	initMatch(result);
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
    Match *result =  new Match(matchForHttpHeaders(&headers));
	initMatch(result);
	return result;
}

/**
 * Completes device detection for the User-Agent provided.
 * @param User-Agent whose results need to be obtained
 * @returns a Map and values array containing all the properties available
 */
map<string, vector<string> >& Provider::getMatchMap(const char *userAgent) {
	map<string, vector<string> > *result = new map<string, vector<string> >();
	buildArray(fiftyoneDegreesGetDeviceOffset(provider.active->dataSet, userAgent), result);
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
		provider.active->dataSet,
        fiftyoneDegreesGetDeviceOffset(provider.active->dataSet, userAgent),
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
		provider.active->dataSet,
        offsets,
        json,
        JSON_BUFFER_LENGTH);
	result.assign(json);
	delete json;
	return result;
}

/**
* Initiates the data set reload process from the same file location that was
* used to create the current dataset. New dataset will be initialised with
* exactly the same set of properties.
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
*/
Provider::Provider(
	const string &fileName,
	const string &propertyString,
	bool validate) {
	stringstream message;
	int64_t difference;

	if (validate == true)
	{
		// Get the difference between the calculated memory needed and the actual
		// memory userd.
		difference = initWithValidate(fileName, propertyString);

		// If the calculated memory is less than the actual memory then throw
		// an error.
		// Note: this will always be a slight overestimate
		if (difference < 0) {
			message << "Predicted memory usage is below the actual usage by "
				<< (0 - difference)
				<< " bytes.";
			throw runtime_error(message.str());
		}
	}
	else {
		// Validate is false so divert to the standard constructor.
		Provider(fileName, propertyString);
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
*/
int64_t Provider::initWithValidate(
	const string &fileName,
	const string &propertyString) {
	int64_t predictedSize;

	// Reset the actual size parameter as it is global and may have been set
	// before.
	_actualSize = 0;

	// Use the getProviderSize function to get the predicted size that the
	// provider will need.
	predictedSize = (int64_t)fiftyoneDegreesGetProviderSizeWithPropertyString(fileName.c_str(), propertyString.c_str());

	// Set the malloc function to use the function that increments _actualSize
	// by the amount being allocated.
	fiftyoneDegreesMalloc = validateMalloc;

	// Use the standard init function to initialise the provider. This is the
	// same function used by the standard constructor, however every memory
	// allocation will be counted using the validateMalloc function.
	init(fileName, propertyString);

	// Revert the malloc function so that future calls do not use the 
	// validateMalloc function.
	fiftyoneDegreesMalloc = malloc;

	// Return the difference between the predicted and actual sizes. A positive
	// number is an overestimate, and a negative number is an underestimate.
	return predictedSize - _actualSize;
}