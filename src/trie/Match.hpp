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

#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <stdlib.h>
#include "51Degrees.h"

#ifndef FIFTYONEDEGREESMATCH_HPP
#define FIFTYONEDEGREESMATCH_HPP

using namespace std;

/**
 * Encapsulates the results of device detection for given target HTTP headers.
 * The class is constructed using an instance of device offsets which are then
 * referenced to return associated values and metrics. The memory used by the
 * offsets is released when the instance is destroyed.
 *
 * Match instances can only be created by a Provider.
 *
 * The Match class interface is compaitable with the class of the same name
 * by the Trie project.
 */
class Match {
	friend class Provider;

public:
	Match(fiftyoneDegreesDeviceOffsets *offsets);
	virtual ~Match();

	vector<string> getValues(const char *propertyName);
	vector<string> getValues(const string &propertyName);
	vector<string> getValues(int requiredPropertyIndex);

	string getValue(const char *propertyName);
	string getValue(const string &propertyName);
	string getValue(int requiredPropertyIndex);
	
	bool getValueAsBool(const char *propertyName);
	bool getValueAsBool(const string &propertyName);
	bool getValueAsBool(int requiredPropertyIndex);

	int getValueAsInteger(const char *propertyName);
	int getValueAsInteger(const string &propertyName);
	int getValueAsInteger(int requiredPropertyIndex);

	double getValueAsDouble(const char *propertyName);
	double getValueAsDouble(const string &propertyName);
	double getValueAsDouble(int requiredPropertyIndex);

	string getDeviceId();
	int getRank();
	int getDifference();
	int getMethod();
	string getUserAgent();
    
    // Manual dispose method for node.
    void close();

protected:

private:

	const char* getValuePointer(int requiredPropertyIndex);

	fiftyoneDegreesDeviceOffsets *offsets;
	string userAgent;
};

#endif // FIFTYONEDEGREESMATCH_HPP
