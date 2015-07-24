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

  void destroyDataset(long dataSet) {
	destroy((DataSet*)dataSet);
  }

  long dataSetInitWithPropertyString(char* fileName, char* propertyString) {
	fiftyoneDegreesDataSet *ds = NULL;
	ds = (fiftyoneDegreesDataSet*)malloc(sizeof(fiftyoneDegreesDataSet));
	initStatus = initWithPropertyString((char*)fileName, ds, propertyString);
	if (initStatus != DATA_SET_INIT_STATUS_SUCCESS)
	{
		free(ds);
		ds = NULL;
	}
	return (long)ds;
  }


  char* getMatch(long dataSet, char* userAgent) {
        fiftyoneDegreesResultsetCache *cache = NULL;
	fiftyoneDegreesWorksetPool *pool = NULL;
	fiftyoneDegreesWorkset *ws = NULL;

        cache = fiftyoneDegreesResultsetCacheCreate((fiftyoneDegreesDataSet*)dataSet, 50);
        if (cache != NULL) {
		pool = fiftyoneDegreesWorksetPoolCreate((fiftyoneDegreesDataSet*)dataSet, cache, 10);
		if (pool != NULL) {
                     ws = fiftyoneDegreesWorksetPoolGet(pool);
                     fiftyoneDegreesMatch(ws, userAgent);
                     char *output = (char *) malloc(50000);
                     processDeviceJSON(ws, output, 50000);
                     fiftyoneDegreesWorksetPoolRelease(pool, ws);
	             fiftyoneDegreesWorksetPoolFree(pool);
                }
	fiftyoneDegreesResultsetCacheFree(cache);
       }

    return output;
  }
  
%}



