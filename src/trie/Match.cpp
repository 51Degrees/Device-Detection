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

#include "Match.hpp"

/**
 * Creates a new match instance for the offsets provided.
 * @param offsets for the devices the match relates to
 */
Match::Match(fiftyoneDegreesDeviceOffsets *offsets) {
	this->offsets = offsets;
}

/**
 * Releases the memory used to store the device offsets.
 */
Match::~Match() {
	fiftyoneDegreesFreeDeviceOffsets(offsets);
}

/**
 * Returns a string representation of the value associated with the required
 * property index. If the index is not valid an empty string is returned. If
 * the property relates to a list with more than one value then values are
 * seperated by | characters.
 * @param requiredPropertyIndex of the property required
 * @returns a string representation of the value for the property
 */
string Match::getValue(int requiredPropertyIndex) {
	string result;
	if (requiredPropertyIndex >= 0 &&
			requiredPropertyIndex <
			fiftyoneDegreesGetRequiredPropertiesCount()) {
		const char *value = fiftyoneDegreesGetValuePtrFromOffsets(
			offsets,
			requiredPropertyIndex);
		if (value != NULL) {
			result.assign(value);
		}
	}
	return result;
}

/**
 * Returns a string representation of the value associated with the required
 * property name. If the property name is not valid an empty string is
 * returned.If the property relates to a list with more than one value then
 * values are seperated by | characters.
 * @param propertyName pointer to a string containing the property name
 * @returns a string representation of the value for the property
 */
string Match::getValue(const char* propertyName) {
	return getValue(fiftyoneDegreesGetRequiredPropertyIndex(propertyName));
}

/**
 * Returns a string representation of the value associated with the required
 * property name. If the property name is not valid an empty string is
 * returned.If the property relates to a list with more than one value then
 * values are seperated by | characters.
 * @param propertyName string containing the property name
 * @returns a string representation of the value for the property
 */
string Match::getValue(string &propertyName) {
	return getValue(propertyName.c_str());
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
		requiredPropertyIndex < fiftyoneDegreesGetRequiredPropertiesCount()) {
		char *start = (char*)fiftyoneDegreesGetValuePtrFromOffsets(
			offsets,
			requiredPropertyIndex);
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
	return getValues(fiftyoneDegreesGetRequiredPropertyIndex(propertyName));
}

/**
 * Returns a vector with all values associated with the required property
 * name. If the name is not valid an empty vector is returned.
 * @param propertyName string containing the property name
 * @returns a vector of values for the property
 */
vector<string> Match::getValues(string &propertyName) {
	return getValues(propertyName.c_str());
}

/**
 * Returns relevant parts of the User-Agent which most closely matched the
 * target User-Agent.
 * @returns string set to the HTTP header value matched
 */
string Match::getUserAgent() {
	return string(offsets->firstOffset->userAgent);
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
	for (int offsetIndex = 0; offsetIndex < offsets->size; offsetIndex++) {
		difference += offsets->firstOffset[offsetIndex].difference;
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
