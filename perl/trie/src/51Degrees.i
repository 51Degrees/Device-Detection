%module "FiftyOneDegrees::TrieV3"
%{ 
#include "../../../src/trie/51Degrees.h"
#ifdef __cplusplus
#define EXTERNAL extern "C"
#else
#define EXTERNAL
#endif

fiftyoneDegreesDataSetInitStatus initStatus;
fiftyoneDegreesDataSetInitStatus getInitStatus() {
  return initStatus;
}

%}

%include "../../../src/trie/51Degrees.h"
%include exception.i

%exception dataSetInitWithPropertyString {
    
 	$action; 
 	fiftyoneDegreesDataSetInitStatus initStatus = getInitStatus();
    switch (initStatus) {
      case DATA_SET_INIT_STATUS_SUCCESS: // nothing to do
      break;

      case DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY:
        SWIG_exception(SWIG_MemoryError, "Insufficient memory allocated.");
      break;

      case DATA_SET_INIT_STATUS_CORRUPT_DATA:
        SWIG_exception(SWIG_RuntimeError, "The data was not the correct format. Check it is uncompressed.");
      break;

      case DATA_SET_INIT_STATUS_INCORRECT_VERSION:
        SWIG_exception(SWIG_RuntimeError, "The data is an unsupported version. Check you have the latest data and API.");
      break;

      case DATA_SET_INIT_STATUS_FILE_NOT_FOUND:
        SWIG_exception(SWIG_IOError, "The data file could not be found. Check the file path and that the program has sufficient read permissions.");
      break;
    }
  }

%inline %{

  void destroy(){
      fiftyoneDegreesDestroy();
  } 
  void dataSetInitWithPropertyString(char *fileName, char* properties) {
	initStatus = (fiftyoneDegreesDataSetInitStatus)fiftyoneDegreesInitWithPropertyString(fileName, properties);
	if (initStatus != DATA_SET_INIT_STATUS_SUCCESS) {
	  fiftyoneDegreesDestroy();
	}
  }
  
  char* getMatch(char* userAgent) {
    char output[50000];
    if (strlen(userAgent) > 0) {
      fiftyoneDegreesProcessDeviceJSON(fiftyoneDegreesGetDeviceOffset(userAgent), output, 50000);
      return output;
    }
  }
  
   char* getMatchWithHeaders(char* userHeader) {
    char output[50000];
    fiftyoneDegreesDeviceOffsets *deviceoffset;
    int js_int;
    if (strlen(userHeader) > 0) {
      deviceoffset = fiftyoneDegreesGetDeviceOffsetsWithHeadersString(userHeader, strlen(userHeader));
      js_int = fiftyoneDegreesGetValueFromOffsets(deviceoffset, 0, output, 5000 );
      fiftyoneDegreesProcessDeviceJSON(js_int, output, 50000);
      return output;
    }
  }

%}



