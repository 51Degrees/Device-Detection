#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include "php.h"
#include "src/pattern/51Degrees.h"
 
#define PHP_EXTENSION_VERSION "3.0"
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

DataSet dataSet;

// get properties implementation
PHP_FUNCTION(fiftyone_degrees_get_properties)
{
  char* useragent;
  int useragent_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", 
    &useragent, &useragent_len) == SUCCESS) {
      Workset* ws = NULL;
      
      ws = createWorkset(&dataSet);

      if (ws != NULL) {
        array_init(return_value);
        match(ws, useragent);
        int propertyIndex;
        for(propertyIndex = 0; propertyIndex < ws->dataSet->requiredPropertyCount; propertyIndex++) {
          char* propertyName = getPropertyName(ws->dataSet, *(ws->dataSet->requiredProperties + propertyIndex));
          int valueCount = setValues(ws, propertyIndex);
          if (valueCount == 1) {
            char *valueString = getValueName(ws->dataSet, *ws->values);
            add_assoc_string(return_value, propertyName, valueString, 1);
          }
          else if (valueCount > 1) {
            zval* valueArray;
	    ALLOC_INIT_ZVAL(valueArray);
            array_init(valueArray);
	    add_assoc_zval(return_value, propertyName, valueArray);
            int valueIndex;
            for(valueIndex = 0; valueIndex < valueCount; valueIndex++) {
              char *valueString = getValueName(ws->dataSet, *(ws->values + valueIndex));
              add_next_index_string(valueArray, valueString, 1);
            }
          }
        }
	freeWorkset(ws);
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



PHP_INI_BEGIN()
PHP_INI_ENTRY("fiftyone_degrees.data_file", "/usr/lib/php5/51Degrees-Lite.dat", PHP_INI_ALL, NULL)
PHP_INI_END()
 
// function implementation
PHP_MINIT_FUNCTION(fiftyone_degrees_detector_init)
{
  REGISTER_INI_ENTRIES();
  char* dataFilePath = INI_STR("fiftyone_degrees.data_file");
  initWithPropertyString(dataFilePath, &dataSet, NULL);
  return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(fiftyone_degrees_detector_shutdown)
{
  UNREGISTER_INI_ENTRIES();
  destroy(&dataSet);
  return SUCCESS;
}
