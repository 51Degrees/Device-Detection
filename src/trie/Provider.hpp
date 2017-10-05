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
#include <vector>
#include <map>
#include <stdexcept>
#include <stdlib.h>
#include <sstream>
#include "Match.hpp"

#ifndef FIFTYONEDEGREESPROVIDER_HPP
#define FIFTYONEDEGREESPROVIDER_HPP

using namespace std;

/**
 * Encapsulates C based device detection functionality. The Provider is
 * responsible for loading and initialising the Trie data file, and creating
 * instances of Match classes based on User-Agents or HTTP Headers.
 *
 * When the Provider is destroyed the memory is freed. As Trie device detection
 * can only be initialised once one provider should be created per process.
 *
 * The Provider can be constructed with a cache to improve performance in
 * situations where requests will be repeated for the same User-Agent. The 
 * cache has a fixed size in memory and does not grow or shrink over time.
 *
 * A pool of worksets can be configured to recycle the memory allocated to a
 * previous detection match. In a multi threaded environment the pool size
 * should optimally be set to the number of concurrent threads the system
 * can support.
 */
class Provider {

	public:
		Provider(const string &fileName);
		Provider(const string &fileName, const string &propertyString);
		Provider(const string &fileName, vector<string> &propertiesArray);

		virtual ~Provider();

		vector<string> getHttpHeaders();
		vector<string> getAvailableProperties();
		string getDataSetName();
		string getDataSetFormat();
		string getDataSetPublishedDate();
		string getDataSetNextUpdateDate();
		int getDataSetSignatureCount();
		int getDataSetDeviceCombinations();

        Match* getMatch(const char *userAgent);
		Match* getMatch(const string &userAgent);
		Match* getMatchForByteArray(const char userAgent[], size_t length);
        Match* getMatch(const map<string, string> &headers);

		Match* getMatchWithTolerances(
			const char *userAgent, 
			int drift, 
			int difference);
		Match* getMatchWithTolerances(
			const char *userAgent, 
			int userAgentLength,
			int drift, 
			int difference);
		Match* getMatchWithTolerances(
			const string &userAgent, 
			int drift, 
			int difference);
		Match* getMatchWithTolerances(
			const map<string, string> &headers, 
			int drift, 
			int difference);

        map<string, vector<string> >& getMatchMap(const char *userAgent);
		map<string, vector<string> >& getMatchMap(const string &userAgent);
		map<string, vector<string> >& getMatchMap(
			const map<string, string> &headers);

        string getMatchJson(const char *userAgent);
        string getMatchJson(const string &userAgent);
        string getMatchJson(const map<string, string> &headers);

		void setDrift(int drift);
		void setDifference(int difference);

		void reloadFromFile();
		void reloadFromMemory(void *source, int length);
		void reloadFromMemory(unsigned char source[], int length);

		bool getIsThreadSafe();

		Provider(const string &fileName, const string &propertyString,
			bool validate);

	protected:

	private:
		vector<string> httpHeaders;
		vector<string> availableProperties;

		void init(const string &fileName, const string &propertyString);
		void init(const string &fileName, vector<string> &propertyString);
		void init(const string &fileName);
		void initHttpHeaders();
		void initAvailableproperties();
		void initException(fiftyoneDegreesDataSetInitStatus initStatus,
			const string &fileName);
		void initComplete(fiftyoneDegreesDataSetInitStatus initStatus,
			const string &fileName);
		void buildArray(fiftyoneDegreesDeviceOffsets *offsets,
			map<string, vector<string> > *result);
		void buildArray(int offset, map<string, vector<string> > *result);
		fiftyoneDegreesDeviceOffsets* matchForHttpHeaders(
			const map<string, string> *headers,
			int drift,
			int difference);

		int64_t initWithValidate(const string &fileName,
			const string &properties);

		fiftyoneDegreesProvider provider;
};

#endif // FIFTYONEDEGREESPROVIDER_HPP
