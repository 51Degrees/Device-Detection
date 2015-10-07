/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright © 2015 51Degrees Mobile Experts Limited, 5 Charlotte Close,
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
 * This Source Code Form is “Incompatible With Secondary Licenses”, as
 * defined by the Mozilla Public License, v. 2.0.
 ********************************************************************** */
#ifndef PHP_EXTENSION_H
#define PHP_V3_EXTENSION_H 1

#include "trie/51Degrees.h"

#ifdef ZTS
#include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(fiftyone_degrees)
    fiftyoneDegreesDeviceOffsets* deviceOffsets;
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
PHP_FUNCTION(fiftyone_info); /* Returns an array of data set information */
PHP_FUNCTION(fiftyone_match); /* No parameters - uses current request headers */
PHP_FUNCTION(fiftyone_match_with_useragent); /* Provide a single User-Agent string */
PHP_FUNCTION(fiftyone_match_with_headers); /* Provide a headers string */
PHP_FUNCTION(fiftyone_degrees_get_properties); /* Array of properties */
PHP_FUNCTION(fiftyone_get_http_headers); /* Array of all HTTP headers relevant for device detection */

extern zend_module_entry fiftyone_degrees_detector_module_entry;
#define phpext_my_extension_ptr &fiftyone_degrees_detector_module_entry

#endif
