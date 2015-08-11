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
 * Please review the README.txt file for instructions to build this
 * code using SWIG. This code is dependent upon the 51Degees.h which
 * is now contained with the main C library
 */
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
fiftyoneDegreesDataSet* dataSet;
fiftyoneDegreesResultsetCache* cache;
fiftyoneDegreesWorksetPool* pool ; 


%}

%include "../../../src/pattern/51Degrees.h"
%include exception.i

/*
 * Exceptions returned by the C code are handled here.
 */
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
%newobject createJSON;
%inline %{

 /*
  * This method is exposed to the PERL and is used to free all pools, cache
  * and dataset.
  */
void freeDataset() {
      
//Check pool size and then free it
//    fiftyoneDegreesWorksetPool *lpool = (fiftyoneDegreesWorksetPool*)pool;
      if (pool != NULL) {
          fiftyoneDegreesWorksetPoolFree(pool);
          pool = NULL;
      }
      //Need to check cache and then free it
      if (cache != NULL){
         fiftyoneDegreesResultsetCacheFree(cache);
         cache = NULL;
      }
      //dataset can be freed
      if (dataSet != NULL) {
          fiftyoneDegreesDataSetFree(dataSet);
          dataSet = NULL;
      }
  }

/* Initialise the dataset using the datafile and properties required. This 
 * method also initialises the cache and pool
 *  */
  
  void dataSetProvider(char* fileName, char* propertyString, int cacheSize, int poolSize) {
 
	dataSet = (fiftyoneDegreesDataSet*)malloc(sizeof(fiftyoneDegreesDataSet));
	initStatus = fiftyoneDegreesInitWithPropertyString((char*)fileName, dataSet, propertyString);
	if (initStatus != DATA_SET_INIT_STATUS_SUCCESS)
	{
		free(dataSet);
		dataSet = NULL;
	}
        
        cache = fiftyoneDegreesResultsetCacheCreate(dataSet, cacheSize);
        pool = fiftyoneDegreesWorksetPoolCreate(dataSet, cache, poolSize);
        

  //      return (long)pool;
        
  }


/* Methods used for matching. */

   char* getMatch(char* userAgent) {
   //     fiftyoneDegreesWorksetPool *lpool = (fiftyoneDegreesWorksetPool*)pool;
        fiftyoneDegreesWorkset *ws = fiftyoneDegreesWorksetPoolGet(pool);
        fiftyoneDegreesMatch(ws, userAgent); 
        char *output = (char *) malloc(50000);
        output = fiftyoneDegreesJSONCreate(ws);
        int32_t jsout = fiftyoneDegreesProcessDeviceJSON(ws, output); 
        fiftyoneDegreesWorksetPoolRelease(pool, ws);
        return output;

  }
  

void freeMatch(char* output){
        fiftyoneDegreesJSONFree(output);
  }


%}



