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

/*
 * Please review the README.md file for instructions to build this
 * code using SWIG.
 */

%module "FiftyOneDegreesPatternV3"
%{
	#include "Provider.hpp"
	#include "Match.hpp"
	#include "Profiles.hpp"

	#ifdef SWIGPHP
	Provider *provider;

	PHP_INI_BEGIN()
	PHP_INI_ENTRY("FiftyOneDegreesPatternV3.data_file", "/usr/lib/php5/51Degrees.dat", PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("FiftyOneDegreesPatternV3.pool_size", "10", PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("FiftyOneDegreesPatternV3.cache_size", "10000", PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("FiftyOneDegreesPatternV3.property_list", "", PHP_INI_ALL, NULL)
	PHP_INI_END()
	#endif
%}

/*
 * For Go API, force the Go Build tool to include sources from directories
 * outside the main package directories, e.g. pattern/trie.
 *
 * If building the Swig wrapper for Go, insert code snippet at the beginning of 
 * the generated go file. This code imports the threading and cityhash sources 
 * which are located outside of the pattern package directory. This allows Go 
 * Build access to the source code. Otherwise, as these paths are not copied to 
 * the temporary 'work' directory used by Go Build, the build will fail even 
 * though they are referenced in the C source.
 *
 * For building on windows, add -lpthread linker flag and include time.h to 
 * support `clock_gettime()` used in threading.
 *
 * References:
 * http://www.swig.org/Doc3.0/SWIGDocumentation.html#Go_adding_additional_code
 * https://golang.org/cmd/cgo/#hdr-Go_references_to_C
 */
#ifdef SWIGGO
%insert(go_begin) %{
/*
#cgo LDFLAGS: -lpthread
#include <time.h>
#include "../threading.c"
#include "../cityhash/city.c"
*/
import "C"
%}
#endif

%include exception.i
%include std_string.i
%include std_pair.i
%include std_vector.i
%include std_map.i

/*
 * For node.js, override the overloader constructor template to
 * carry though the correct error message. The additions are the
 * two "goto fail"'s.
 */
#ifdef BUILDING_NODE_EXTENSION
%fragment ("js_ctor_dispatch_case", "templates")
%{
	if(args.Length() == $jsargcount) {
		errorHandler.err.Clear();
#if (V8_MAJOR_VERSION-0) < 4 && (SWIG_V8_VERSION < 0x031903)
		self = $jswrapper(args, errorHandler);
		if(errorHandler.err.IsEmpty()) {
			SWIGV8_ESCAPE(self);
		} else {
			goto fail;
		}
#else
		$jswrapper(args, errorHandler);
		if(errorHandler.err.IsEmpty()) {
			return;
		} else {
			goto fail;
		}
#endif
	}
%}
#endif

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
%newobject Provider::getMatchForDeviceId;
%nodefaultctor Profiles;
%newobject Provider::findProfiles;

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
	char *filePath = INI_STR("FiftyOneDegreesPatternV3.data_file");
	int poolSize = INI_INT("FiftyOneDegreesPatternV3.pool_size");
	int cacheSize = INI_INT("FiftyOneDegreesPatternV3.cache_size");
	char *propertyList = INI_STR("FiftyOneDegreesPatternV3.property_list");

	provider = new Provider(filePath, propertyList, cacheSize, poolSize);
}

%mshutdown {

	delete provider;
}

%rename(findProfilesInProfiles) findProfiles(const std::string propertyName, const std::string valueName, Profiles *profiles);
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
    
    // Manual dispose method for node.
#ifdef BUILDING_NODE_EXTENSION
    void close();
#endif
};

class Profiles {

public:
	virtual ~Profiles();

	Profiles();
	int getCount();
	int getProfileIndex(int index);
	int getProfileId(int index);

};

class Provider {

	public:

	Provider(const std::string &fileName);
	Provider(const std::string &fileName, const std::string &propertyString);
	Provider(const std::string &fileName, const std::string &propertyString, int cacheSize, int poolSize);
	Provider(const std::string &fileName, std::vector<std::string> &propertiesArray);
	Provider(const std::string &fileName, std::vector<std::string> &propertiesArray, int cacheSize, int poolSize);
	Provider(const std::string &fileName, int cacheSize, int poolSize);
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

	Match* getMatchForDeviceId(const std::string &deviceId);
	Profiles* findProfiles(const std::string propertyName, const std::string valueName);
	Profiles* findProfiles(const std::string propertyName, const std::string valueName, Profiles *profiles);

	void reloadFromFile();
	void reloadFromMemory(const std::string &source, int length);

	int getCacheHits();
	int getCacheMisses();
	int getCacheMaxIterations();

	bool getIsThreadSafe();

	Provider(const std::string &fileName, const std::string &propertyString, int cacheSize, int poolSize, bool validate);
};
