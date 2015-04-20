#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include "php.h"
#include "src/pattern/51Degrees.h"
 
#define PHP_EXTENSION_VERSION "3.1"
#define PHP_EXTENSION_EXTNAME "FiftyOne_Degrees_Detector"
 
extern zend_module_entry fiftyone_degrees_detector_module_entry;
#define phpext_my_extension_ptr &fiftyone_degrees_detector_module_entry

PHP_FUNCTION(fiftyone_degrees_test_function);
PHP_FUNCTION(fiftyone_degrees_get_properties);

PHP_MINIT_FUNCTION(fiftyone_degrees_detector_init);
PHP_MSHUTDOWN_FUNCTION(fiftyone_degrees_detector_shutdown);
 
// list of custom PHP functions provided by this extension
// set {NULL, NULL, NULL} as the last record to mark the end of list
static zend_function_entry fiftyone_degrees_detector_functions[] = {
  PHP_FE(fiftyone_degrees_test_function, NULL)
  PHP_FE(fiftyone_degrees_get_properties, NULL)
  {NULL, NULL, NULL}
};
 
// the following code creates an entry for the module and registers it with Zend.
zend_module_entry fiftyone_degrees_detector_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
  STANDARD_MODULE_HEADER,
#endif
  PHP_EXTENSION_EXTNAME,
  fiftyone_degrees_detector_functions,
  PHP_MINIT(fiftyone_degrees_detector_init),
  PHP_MSHUTDOWN(fiftyone_degrees_detector_shutdown),
  NULL, // name of the RINIT function or NULL if not applicable
  NULL, // name of the RSHUTDOWN function or NULL if not applicable
  NULL, // name of the MINFO function or NULL if not applicable
#if ZEND_MODULE_API_NO >= 20010901
  PHP_EXTENSION_VERSION,
#endif
  STANDARD_MODULE_PROPERTIES
};
 
ZEND_GET_MODULE(fiftyone_degrees_detector)

PHP_FUNCTION(fiftyone_degrees_test_function)
{
  RETURN_STRING("HELLO WORLD", 1);
}

fiftyoneDegreesDataSet dataSet;
fiftyoneDegreesDataSetInitStatus initStatus;

// get properties implementation
PHP_FUNCTION(fiftyone_degrees_get_properties)
{
  switch (initStatus) {
    case DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY:
      php_error(E_WARNING, "51Degrees data set could not be created as not enough memory could be allocated.");
      return;
    case DATA_SET_INIT_STATUS_CORRUPT_DATA:
      php_error(E_WARNING, "51Degrees data set could not be created as a corrupt data file has been provided. Check it is uncompressed.");
      return;
    case DATA_SET_INIT_STATUS_INCORRECT_VERSION:
      php_error(E_WARNING, "51Degrees data set could not be created as the data file is an unsupported version. Check you have the latest version of the data and api.");
      return;
    case DATA_SET_INIT_STATUS_FILE_NOT_FOUND:
      php_error(E_WARNING, "51Degrees data set could not be created as a data file could not be found.");
      return;
    case DATA_SET_INIT_STATUS_SUCCESS: // do nothing
      break;
  }
  char* useragent;
  int useragent_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", 
  &useragent, &useragent_len) == SUCCESS) {
    fiftyoneDegreesWorkset* ws = NULL;
    
    ws = fiftyoneDegreesCreateWorkset(&dataSet);

    if (ws != NULL) {
      array_init(return_value);
      fiftyoneDegreesMatch(ws, useragent);
      int propertyIndex;
      for(propertyIndex = 0; propertyIndex < ws->dataSet->requiredPropertyCount; propertyIndex++) {
        char* propertyName = fiftyoneDegreesGetPropertyName(ws->dataSet, *(ws->dataSet->requiredProperties + propertyIndex));
        int valueCount = fiftyoneDegreesSetValues(ws, propertyIndex);
        if (valueCount == 1) {
          char *valueString = fiftyoneDegreesGetValueName(ws->dataSet, *ws->values);
          add_assoc_string(return_value, propertyName, valueString, 1);
        }
        else if (valueCount > 1) {
          zval* valueArray;
          ALLOC_INIT_ZVAL(valueArray);
          array_init(valueArray);
          add_assoc_zval(return_value, propertyName, valueArray);
          int valueIndex;
          for(valueIndex = 0; valueIndex < valueCount; valueIndex++) {
            char *valueString = fiftyoneDegreesGetValueName(ws->dataSet, *(ws->values + valueIndex));
            add_next_index_string(valueArray, valueString, 1);
          }
        }
      }

      int32_t sigRank = fiftyoneDegreesGetSignatureRank(ws);
      add_assoc_long(return_value, "SignatureRank", sigRank);
      add_assoc_long(return_value, "Difference", ws->difference);

      char* methodString;
      switch(ws->method){
        case NONE: methodString = "None"; break;
        case EXACT: methodString = "Exact"; break;
        case NUMERIC: methodString = "Numeric"; break;
        case NEAREST: methodString = "Nearest"; break;
        case CLOSEST: methodString = "Closest"; break;
        default: methodString = "Unknown"; break;
      }

      add_assoc_string(return_value, "Method", methodString, 1);
      fiftyoneDegreesFreeWorkset(ws);
    }
    else 
    {
      php_error(E_WARNING, "The workset could not be initialised.");
    }
  }
  else 
  {
    php_error(E_WARNING, "Could not parse arguments.");
  }
  
}

int32_t getSignatureRank(Workset *ws) {
	int32_t rank;
	int32_t signatureIndex = (int32_t)(ws->signature - ws->dataSet->signatures) / ws->dataSet->sizeOfSignature;
	for (rank = 0; rank < ws->dataSet->header.signatures.count; rank++) {
		if (ws->dataSet->rankedSignatureIndexes[rank] == signatureIndex) {
			return rank;
		}
	}
	return INT32_MAX;
}

PHP_INI_BEGIN()
PHP_INI_ENTRY("fiftyone_degrees.data_file", "/usr/lib/php5/51Degrees-Lite.dat", PHP_INI_ALL, NULL)
PHP_INI_END()
 
// function implementation
PHP_MINIT_FUNCTION(fiftyone_degrees_detector_init)
{
  REGISTER_INI_ENTRIES();
  char* dataFilePath = INI_STR("fiftyone_degrees.data_file");
  initStatus = fiftyoneDegreesInitWithPropertyString(dataFilePath, &dataSet, NULL);
  return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(fiftyone_degrees_detector_shutdown)
{
  UNREGISTER_INI_ENTRIES();
  if (initStatus == SUCCESS) {
    fiftyoneDegreesDestroy(&dataSet);
  }
  return SUCCESS;
}
