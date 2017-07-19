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

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <sstream>
#include "Match.hpp"
#include "Profiles.hpp"

#ifndef FIFTYONEDEGREESPROVIDER_HPP
#define FIFTYONEDEGREESPROVIDER_HPP

using namespace std;

#define FIFTYONEDEGREESPROVIDER_CACHE_SIZE 10000
#define FIFTYONEDEGREESPROVIDER_POOL_SIZE 20

/**
 * Encapsulates the functionality provided by the C DataSet, Pool and Cache
 * exposing services via a class model. The Provider is resonsible for loading
 * data into memory, freeing memory when destroyed, managing the pool and cache
 * and creating Match instances from User-Agent and HTTP header data. A single
 * process can have multiple Providers and each is thread safe.
 */
class Provider {

	public:
		Provider(const string &fileName);
		Provider(const string &fileName, const string &propertyString);
		Provider(const string &fileName, const string &propertyString,
			int cacheSize, int poolSize);
		Provider(const string &fileName, vector<string> &propertiesArray);
		Provider(const string &fileName, vector<string> &propertiesArray,
			int cacheSize, int poolSize);
		Provider(const string &fileName, int cacheSize, int poolSize);

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
        Match* getMatch(const map<string, string> &headers);

        map<string, vector<string> >& getMatchMap(const char *userAgent);
		map<string, vector<string> >& getMatchMap(const string &userAgent);
		map<string, vector<string> >& getMatchMap(
			const map<string, string> &headers);

        string getMatchJson(const char *userAgent);
        string getMatchJson(const string &userAgent);
        string getMatchJson(const map<string, string> &headers);

		Match* getMatchForDeviceId(const char *deviceId);
		Match* getMatchForDeviceId(const string &deviceId);

		Profiles* findProfiles(const string &propertyName, const string &valueName);
		Profiles* findProfiles(const char *propertyName, const char *valueName);
		Profiles* findProfiles(const string &propertyName, const string &valueName, Profiles* profiles);
		Profiles* findProfiles(const char *propertyName, const char *valueName, Profiles* profiles);

		void reloadFromFile();
	protected:

	private:
		vector<string> httpHeaders;
		vector<string> availableProperties;

		void init(const string &fileName, const string &propertyString,
			int cacheSize, int poolSize);
		void init(const string &fileName, vector<string> &propertyString,
			int cacheSize, int poolSize);
		void init(const string &fileName, int cacheSize, int poolSize);
		void initHttpHeaders();
		void initAvailableProperites();
		void initExecption(fiftyoneDegreesDataSetInitStatus initStatus,
			const string &fileName);
		void initMatch(Match *match);
		void initComplete(fiftyoneDegreesDataSetInitStatus initStatus,
			const string &fileName);
		void buildArray(fiftyoneDegreesWorkset *ws,
			map<string, vector<string> > *result);
		void matchForHttpHeaders(fiftyoneDegreesWorkset *ws,
			const map<string, string> *headers);

		fiftyoneDegreesProvider provider;
};

#endif // FIFTYONEDEGREESPROVIDER_HPP