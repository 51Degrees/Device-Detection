#ifndef PHP_EXTENSION_H
#define PHP_V3_EXTENSION_H 1

#include "pattern/51Degrees.h"

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(fiftyone_degrees)
    fiftyoneDegreesWorkset *ws;
ZEND_END_MODULE_GLOBALS(fiftyone_degrees)

#ifdef ZTS
#define FIFTYONE_G(v) TSRMG(fiftyone_degrees_globals_id, zend_fiftyone_degrees_globals *, v)
#else
#define FIFTYONE_G(v) (fiftyone_degrees_globals.v)
#endif

#define PHP_EXTENSION_VERSION "3.2"
#define PHP_EXTENSION_EXTNAME "FiftyOne_Degrees_Detector"

/* Prototypes of Init and Shutdown functions for module and request. */
PHP_MINIT_FUNCTION(fiftyone_degrees_detector_init);
PHP_MSHUTDOWN_FUNCTION(fiftyone_degrees_detector_shutdown);

PHP_RINIT_FUNCTION(fiftyone_degrees_request_init);
PHP_RSHUTDOWN_FUNCTION(fiftyone_degrees_request_shutdown);

/* Prototype of 51Degrees functions exposed to PHP. */
PHP_FUNCTION(fiftyone_degrees_test_function);
PHP_FUNCTION(fiftyone_degrees_get_properties);
PHP_FUNCTION(fiftyone_match);
PHP_FUNCTION(fiftyone_info);
PHP_FUNCTION(fiftyone_http_headers);
PHP_FUNCTION(fiftyone_match_with_headers);

extern zend_module_entry fiftyone_degrees_detector_module_entry;
#define phpext_my_extension_ptr &fiftyone_degrees_detector_module_entry

/* Function prototypes */
void matchHttpHeaders(char *httpHeaders, int httpHeadersLength, zval *returnArray);
void matchUserAgent(char *userAgent, int userAgentLength, zval *returnArray);
void buildArray(zval *returnArray);

#endif
