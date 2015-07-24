%module "FiftyOneDegrees::PatternV3"
%{ 
#include "../../../src/pattern/51Degrees.h"
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

%include "../../../src/pattern/51Degrees.h"
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
%newobject getMatch;
%inline %{

  void freeDataset(long dataSet) {
	fiftyoneDegreesDataSetFree((fiftyoneDegreesDataSet*)dataSet);
  }

  long dataSetInitWithPropertyString(char* fileName, char* propertyString) {
	fiftyoneDegreesDataSet *ds = NULL;
	ds = (fiftyoneDegreesDataSet*)malloc(sizeof(fiftyoneDegreesDataSet));
	initStatus = fiftyoneDegreesInitWithPropertyString((char*)fileName, ds, propertyString);
	if (initStatus != DATA_SET_INIT_STATUS_SUCCESS)
	{
		free(ds);
		ds = NULL;
	}
	return (long)ds;
  }

long cacheInitWithDataSet(long dataSet) {
        fiftyoneDegreesResultsetCache *cache = NULL;
        cache = fiftyoneDegreesResultsetCacheCreate((fiftyoneDegreesDataSet*)dataSet, 10);

	return (long)cache;
  }

  void freeCache(long cache) {
	fiftyoneDegreesResultsetCacheFree((fiftyoneDegreesResultsetCache*)cache);
  }


long poolInitWithDataSet(long dataSet, long cache) {
        fiftyoneDegreesWorksetPool *pool = NULL;
 pool = fiftyoneDegreesWorksetPoolCreate((fiftyoneDegreesDataSet*)dataSet, (fiftyoneDegreesResultsetCache*)cache, 50);

	return (long)pool;
  }

  void freePool(long pool) {
	fiftyoneDegreesWorksetPoolFree((fiftyoneDegreesWorksetPool*)pool);
  }

  void freeJSON(char* output){
        fiftyoneDegreesJSONFree(output);
  }

  char* getMatch(long pool, char* userAgent) {
	fiftyoneDegreesWorkset *ws = fiftyoneDegreesWorksetPoolGet((fiftyoneDegreesWorksetPool*)pool);
        
        fiftyoneDegreesMatch(ws, userAgent);  
        char *output = fiftyoneDegreesJSONCreate(ws);                 
        int32_t jsout = fiftyoneDegreesProcessDeviceJSON(ws, output);
       
        return output;
  }
  
%}



