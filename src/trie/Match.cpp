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

#include "Match.hpp"

/**
 * Creates a new match instance for the offsets provided.
 * @param offsets for the devices the match relates to
 */
Match::Match(fiftyoneDegreesDeviceOffsets *offsets) {
	this->offsets = offsets;
}

/**
 * Releases the memory used to store the device offsets if this was set.
 */
Match::~Match() {
	fiftyoneDegreesProviderFreeDeviceOffsets(offsets);
}

/**
 * Gets a pointer to the value string associated with the required property 
 * index. Checks if multiple offsets are available, or a single integer offset.
 * @param requiredPropertyIndex of the property required
 * @returns pointer to a string representation of the value for the property
 */
const char* Match::getValuePointer(int requiredPropertyIndex) {
	const char *value = NULL;
	if (requiredPropertyIndex >= 0 &&
		requiredPropertyIndex <
		fiftyoneDegreesGetRequiredPropertiesCount(offsets->active->dataSet)) {
		value = fiftyoneDegreesGetValuePtrFromOffsets(
			offsets->active->dataSet,
			offsets,
			requiredPropertyIndex);
	}
	return value;
}

/**
 * Returns a string representation of the value associated with the required
 * property index. If the index is not valid an empty string is returned. If
 * the property relates to a list with more than one value then values are
 * separated by | characters.
 * @param requiredPropertyIndex of the property required
 * @returns a string representation of the value for the property
 */
string Match::getValue(int requiredPropertyIndex) {
	string result;
	const char *value = getValuePointer(requiredPropertyIndex);
	if (value != NULL) {
		result.assign(value);
	}
	return result;
}

/**
 * Returns a string representation of the value associated with the required
 * property name. If the property name is not valid an empty string is
 * returned.If the property relates to a list with more than one value then
 * values are separated by | characters.
 * @param propertyName pointer to a string containing the property name
 * @returns a string representation of the value for the property
 */
string Match::getValue(const char* propertyName) {
	return getValue(
		fiftyoneDegreesGetRequiredPropertyIndex(
			offsets->active->dataSet,
			propertyName));
}

/**
* Returns a string representation of the value associated with the required
* property name. If the property name is not valid an empty string is
* returned.If the property relates to a list with more than one value then
* values are separated by | characters.
* @param propertyName string containing the property name
* @returns a string representation of the value for the property
*/
string Match::getValue(const string &propertyName) {
	return getValue(propertyName.c_str());
}

/**
* Returns a boolean representation of the value associated with the required
* property index. If the property index is not valid then false is returned.
* @param requiredPropertyIndex in the required properties
* @returns a boolean representation of the value for the property
*/
bool Match::getValueAsBool(int requiredPropertyIndex) {
	const char *value = getValuePointer(requiredPropertyIndex);
	return value != NULL && strcmp(value, "True") == 0;
}

/**
* Returns a boolean representation of the value associated with the required
* property name. If the property name is not valid then false is returned.
* @param propertyName string containing the property name
* @returns a boolean representation of the value for the property
*/
bool Match::getValueAsBool(const char* propertyName) {
	return getValueAsBool(
		fiftyoneDegreesGetRequiredPropertyIndex(
			offsets->active->dataSet, 
			propertyName));
}

/**
* Returns a boolean representation of the value associated with the required
* property name. If the property name is not valid then false is returned.
* @param propertyName string containing the property name
* @returns a boolean representation of the value for the property
*/
bool Match::getValueAsBool(const string &propertyName) {
	return getValueAsBool(propertyName.c_str());
}

/**
* Returns an integer representation of the value associated with the required
* property index. If the property index is not valid then 0 is returned.
* Using a property which returns non-numeric characters will result in
* unexpected behaviour.
* @param requiredPropertyIndex in the required properties
* @returns an integer representation of the value for the property
*/
int Match::getValueAsInteger(int requiredPropertyIndex) {
	const char *value = getValuePointer(requiredPropertyIndex);
	if (value != NULL) {
		return atoi(value);
	}
	return 0;
}

/**
* Returns an integer representation of the value associated with the required
* property index. If the property name is not valid then 0 is returned.
* Using a property which returns non-numeric characters will result in
* unexpected behaviour.
* @param propertyName string containing the property name
* @returns an integer representation of the value for the property
*/
int Match::getValueAsInteger(const char* propertyName) {
	return getValueAsInteger(
		fiftyoneDegreesGetRequiredPropertyIndex(
			offsets->active->dataSet, 
			propertyName));
}

/**
* Returns an integer representation of the value associated with the required
* property name. If the property name is not valid then 0 is returned.
* Using a property which returns non-numeric characters will result in
* unexpected behaviour.
* @param propertyName string containing the property name
* @returns an integer representation of the value for the property
*/
int Match::getValueAsInteger(const string &propertyName) {
	return getValueAsInteger(propertyName.c_str());
}

/**
* Returns a double representation of the value associated with the required
* property index. If the property index is not valid then 0 is returned.
* Using a property which returns non-numeric characters will result in
* unexpected behaviour.
* @param requiredPropertyIndex in the required properties
* @returns a double representation of the value for the property
*/
double Match::getValueAsDouble(int requiredPropertyIndex) {
	const char *value = getValuePointer(requiredPropertyIndex);
	if (value != NULL) {
		return strtod(value, NULL);
	}
	return 0;
}

/**
* Returns a double representation of the value associated with the required
* property name. If the property name is not valid then 0 is returned.
* Using a property which returns non-numeric characters will result in
* unexpected behaviour.
* @param propertyName string containing the property name
* @returns a double representation of the value for the property
*/
double Match::getValueAsDouble(const char* propertyName) {
	return getValueAsDouble(
		fiftyoneDegreesGetRequiredPropertyIndex(
			offsets->active->dataSet, 
			propertyName));
}

/**
* Returns a double representation of the value associated with the required
* property name. If the property name is not valid then 0 is returned.
* Using a property which returns non-numeric characters will result in
* unexpected behaviour.
* @param propertyName string containing the property name
* @returns a double representation of the value for the property
*/
double Match::getValueAsDouble(const string &propertyName) {
	return getValueAsDouble(propertyName.c_str());
}

/**
 * Returns a vector with all values associated with the required property
 * index. If the index is not valid an empty vector is returned.
 * @param requiredPropertyIndex of the property required
 * @returns a vector of values for the property
 */
vector<string> Match::getValues(int requiredPropertyIndex) {
	vector<string> result;
	if (requiredPropertyIndex >= 0 &&
		requiredPropertyIndex < 
			fiftyoneDegreesGetRequiredPropertiesCount(
				offsets->active->dataSet)) {
		char *start = (char*)getValuePointer(requiredPropertyIndex);
		if (start != NULL) {
			char *current = start, *last = start;
			while (*current != 0) {
				if (*current == '|') {
					result.insert(
						result.end(),
						string(last, current - last));
					last = current + 1;
				}
				current++;
			}
			result.insert(
				result.end(),
				string(last, current - last));
		}
	}
	return result;
}

/**
 * Returns a vector with all values associated with the required property
 * name. If the name is not valid an empty vector is returned.
 * @param propertyName pointer to a string containing the property name
 * @returns a vector of values for the property
 */
vector<string> Match::getValues(const char *propertyName) {
	return getValues(fiftyoneDegreesGetRequiredPropertyIndex(
		offsets->active->dataSet,
		propertyName));
}

/**
 * Returns a vector with all values associated with the required property
 * name. If the name is not valid an empty vector is returned.
 * @param propertyName string containing the property name
 * @returns a vector of values for the property
 */
vector<string> Match::getValues(const string &propertyName) {
	return getValues(propertyName.c_str());
}

/**
 * Returns relevant parts of the User-Agent which most closely matched the
 * target User-Agent.
 * @returns string set to the HTTP header value matched
 */
string Match::getUserAgent() {
	return offsets != NULL ? string(offsets->firstOffset->userAgent) : "";
}

/**
 * Returns the unique device ID if the Id property was included in the required
 * list of properties when the Provider was constructed.
 */
string Match::getDeviceId() {
	return getValue("Id");
}

/**
 * Returns the Rank of the signature found.
 * TODO - add rank data to the Trie data set
 */
int Match::getRank() {
	return 0;
}

/**
 * Returns the difference between the result returned and the target
 * User-Agent
 */
int Match::getDifference() {
	int difference = 0;
	if (offsets != NULL) {
		for (int offsetIndex = 0; offsetIndex < offsets->size; offsetIndex++) {
			difference += offsets->firstOffset[offsetIndex].difference;
		}
	}
	return difference;
}

/**
 * Returns the method used to determine the match result. Always 0 as Trie
 * only has one method available.
 */
int Match::getMethod() {
	return 0;
}

/**
 * Manual dispose method for node.
 * Deprecated: match object is now freed by the garbage collector in
 * all languages, so this does nothing.
 */
void Match::close() {
}
