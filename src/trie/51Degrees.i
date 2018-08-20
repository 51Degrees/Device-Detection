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

%module "FiftyOneDegreesTrieV3"
%{
	#include "Provider.hpp"
	#include "Match.hpp"

	#ifdef SWIGPHP
	Provider *provider;

	PHP_INI_BEGIN()
	PHP_INI_ENTRY("FiftyOneDegreesTrieV3.data_file", "/usr/lib/php5/51Degrees.trie", PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("FiftyOneDegreesTrieV3.property_list", "", PHP_INI_ALL, NULL)
	PHP_INI_END()
	#endif

%}

/*
 * For Go API, force the Go Build tool to include sources from directories
 * outside the main package directories, e.g. pattern/trie.
 *
 * If building the Swig wrapper for Go, insert code snippet at the beginning of 
 * the generated go file. This code imports the threading and cache sources 
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
#include "../cache.c"
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

	std::vector<std::string> getValues(const std::string &propertyName);
    std::vector<std::string> getValues(int propertyIndex);

	std::string getValue(const std::string &propertyName);
    std::string getValue(int propertyIndex);

	bool getValueAsBool(const std::string &propertyName);
	bool getValueAsBool(int requiredPropertyIndex);

	int getValueAsInteger(const std::string &propertyName);
	int getValueAsInteger(int requiredPropertyIndex);

	double getValueAsDouble(const std::string &propertyName);
	double getValueAsDouble(int requiredPropertyIndex);
	
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

#ifdef SWIGJAVA
%apply (char *STRING, size_t LENGTH) { (const char userAgent[], size_t length) }
%newobject Provider::getMatchForByteArray;
#endif

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
#ifdef SWIGJAVA
	Match* getMatchForByteArray(const char userAgent[], size_t length);
#endif
    Match* getMatch(const std::map<std::string, std::string> &headers);

	Match* getMatchWithTolerances(const std::string &userAgent, int drift, int difference);
    Match* getMatchWithTolerances(const std::map<std::string, std::string> &headers, int drift, int difference);

	std::string getMatchJson(const std::string &userAgent);
    std::string getMatchJson(const std::map<std::string, std::string> &headers);

	void setDrift(int drift);
    void setDifference(int difference);

	void reloadFromFile();
	void reloadFromMemory(unsigned char source[], int size);

	bool getIsThreadSafe();

	Provider(const std::string &fileName, const std::string &propertyString, bool validate);
};
