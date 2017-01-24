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

#include <string>
#include <vector>
#include <map>
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
	vector<string> getValues(string &propertyName);
	vector<string> getValues(int requiredPropertyIndex);

	string getValue(const char *propertyName);
	string getValue(string &propertyName);
	string getValue(int requiredPropertyIndex);

	string getDeviceId();
	int getRank();
	int getDifference();
	int getMethod();
	string getUserAgent();
    
    // Manual dispose method for node.
    void close();

protected:

private:
	fiftyoneDegreesDataSet *dataSet;
	fiftyoneDegreesDeviceOffsets *offsets;
	string userAgent;
};

#endif // FIFTYONEDEGREESMATCH_HPP
