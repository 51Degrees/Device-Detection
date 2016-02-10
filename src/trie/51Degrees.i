/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright 2014 51Degrees Mobile Experts Limited, 5 Charlotte Close,
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

/*
 * Please review the README.md file for instructions to build this
 * code using SWIG.
 */

%module "FiftyOneDegreesTrieV3"
%{
	#include "Provider.hpp"
	#include "Match.hpp"

	#ifdef SWIGPHP
	Provider *provider;

	PHP_INI_BEGIN()
	PHP_INI_ENTRY("FiftyOneDegreesTrieV3.data_file", "/usr/lib/php5/51Degrees.trie", PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("FiftyOneDegreesTrieV3.property_list", NULL, PHP_INI_ALL, NULL)
	PHP_INI_END()
	#endif

%}

%include exception.i
%include std_string.i
%include std_pair.i
%include std_vector.i
%include std_map.i

/*
 * Exceptions returned by the C++ code are handled here.
 */
%exception {
	try {
		$action;
	}
	catch(runtime_error& e) {
		SWIG_exception(SWIG_RuntimeError, e.what());
	}
	catch(invalid_argument& e) {
		SWIG_exception(SWIG_ValueError, e.what());
	}
}

%template(MapStringString) std::map<std::string,std::string>;
%template(VectorString) std::vector<std::string>;

/*
 * Instances of Match are created from the Provider's get Match methods.
 * They do not need a constructor and the %newobject directive tells target
 * language to be responsible for memory cleanup.
 */
%nodefaultctor Match;
%newobject Provider::getMatch;

/*
 * Allow partial C# classes
 */
%typemap(csclassmodifiers) SWIGTYPE "public partial class"

#ifdef SWIGPHP
/*
 * PHP global variable for the Provider
 */
%immutable provider;
Provider *provider;

%minit {

	REGISTER_INI_ENTRIES();
	char *filePath = INI_STR("FiftyOneDegreesTrieV3.data_file");
	char *propertyList = INI_STR("FiftyOneDegreesTrieV3.property_list");

	provider = new Provider(filePath, propertyList);
}

%mshutdown {

	delete provider;
}
#endif

class Match {

    public:

	virtual ~Match();

    std::vector<std::string> getValues(const char *propertyName);
    std::vector<std::string> getValues(std::string &propertyName);
    std::vector<std::string> getValues(int propertyIndex);

    std::string getValue(const char *propertyName);
    std::string getValue(std::string &propertyName);
    std::string getValue(int propertyIndex);

    std::string getDeviceId();
    int getRank();
    int getDifference();
    int getMethod();
    std::string getUserAgent();
};

class Provider {

	public:

	Provider(const std::string &fileName);
	Provider(const std::string &fileName, const std::string &propertyString);
    Provider(const std::string &fileName, std::vector<std::string> &propertiesArray);
	virtual ~Provider();

    std::vector<std::string> getHttpHeaders();
    std::vector<std::string> getAvailableProperties();
    std::string getDataSetName();
    std::string getDataSetFormat();
    std::string getDataSetPublishedDate();
    std::string getDataSetNextUpdateDate();
    int getDataSetSignatureCount();
    int getDataSetDeviceCombinations();

    Match* getMatch(const std::string &userAgent);
    Match* getMatch(const std::map<std::string, std::string> &headers);

    std::string getMatchJson(const std::string &userAgent);
    std::string getMatchJson(const std::map<std::string, std::string> &headers);
};
