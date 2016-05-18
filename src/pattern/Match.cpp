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

#include <iostream>
#include "Match.hpp"

/**
 * Creates a new match instance.
 */
Match::Match() {}

/**
 * Releases the workset back into the pool ready for another Match instance
 * to use.
 */
Match::~Match() {
    fiftyoneDegreesWorksetRelease(ws);
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
    const fiftyoneDegreesAsciiString* valueName;
    if (requiredPropertyIndex >= 0 &&
        fiftyoneDegreesSetValues(ws, requiredPropertyIndex) > 0) {
        valueName = fiftyoneDegreesGetString(
        	ws->dataSet,
        	ws->values[0]->nameOffset);
        result.assign(&(valueName->firstByte));
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
    return getValue(fiftyoneDegreesGetRequiredPropertyIndex(
    	ws->dataSet,
    	propertyName));
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
    const fiftyoneDegreesAsciiString* valueName;
    if (requiredPropertyIndex >= 0 &&
        fiftyoneDegreesSetValues(ws, requiredPropertyIndex) > 0) {
        for (int valueIndex = 0; valueIndex < ws->valuesCount; valueIndex++) {
            valueName = fiftyoneDegreesGetString(
            	ws->dataSet,
            	ws->values[valueIndex]->nameOffset);
            result.insert(result.end(), string(&(valueName->firstByte)));
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
vector<string> Match::getValues(const char* propertyName) {
    return getValues(fiftyoneDegreesGetRequiredPropertyIndex(
    	ws->dataSet,
    	propertyName));
}

/**
 * Returns a vector with all values associated with the required property
 * name. If the name is not valid an empty vector is returned.
 * @param propertyName string containing the property name
 * @returns a vector of values for the property
 */
vector<string> Match::getValues(string& propertyName) {
    return getValues(propertyName.c_str());
}

/**
 * Returns relevant parts of the User-Agent which most closely matched the
 * target User-Agent
 * @returns the relevant parts of the User-Agent as a string
 */
string Match::getUserAgent() {
    string result;
    char *buffer = new char[ws->dataSet->header.maxUserAgentLength];
    fiftyoneDegreesGetSignatureAsString(
    	ws,
    	buffer,
    	ws->dataSet->header.maxUserAgentLength);
    result.assign(buffer);
    delete buffer;
    return result;
}

/**
 * Returns the unique device ID if the Id property was included in the required
 * list of properties when the Provider was constructed.
 * @returns the device ID as a string
 */
string Match::getDeviceId() {
    string result;
    int bufferSize = ws->dataSet->header.components.count * 10;
    char *buffer = new char[bufferSize];
    fiftyoneDegreesGetDeviceId(ws, buffer, bufferSize);
    result.assign(buffer);
    delete buffer;
    return result;
}

/**
 * Returns the Rank of the signature found.
 * See 
 * <a href="https://51degrees.com/support/documentation/pattern"
 * target="_parent">https://51degrees.com/support/documentation/pattern</a>
 * for more details.
 * @returns the Rank of the signature found as a string
 */
int Match::getRank() {
    return fiftyoneDegreesGetSignatureRank(ws);
}

/**
 * Returns the difference between the result returned and the target
 * User-Agent.
 * See <a href="https://51degrees.com/support/documentation/pattern"
 * target="_parent">https://51degrees.com/support/documentation/pattern</a>
 * for more details.
 * @returns the difference as an integer
 */
int Match::getDifference() {
    return ws->difference;
}

/**
 * Returns the method used to determine the match result.
 * See 
 * <a href="https://51degrees.com/support/documentation/pattern"
 * target="_parent">https://51degrees.com/support/documentation/pattern</a>
 * for more details.
 * @returns the method used as an integer
 */
int Match::getMethod() {
    return ws->method;
}
