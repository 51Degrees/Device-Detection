/**
 * Copyright (C) 2013 51Degrees.mobi Limited
 *
 * See README.rst and LICENSE.txt for more details.
 */

#include <Python.h>
#include "lib/trie/51Degrees.h"

#define OUTPUT_BUFFER_LENGTH 50000

struct module_state {
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct module_state _state;
#endif

static PyObject *py_init(PyObject *self, PyObject *args)
{
    // Input data filename.
    const char *filePath;

    // Input property names.
    const char *properties;

    // Parse input arguments: data filename (string) + property names (string).
    if (!PyArg_ParseTuple(args, "ss", &filePath, &properties)) {
        return NULL;
    }
	if (strlen(properties) == 0) {
		properties = NULL;
	}
	struct stat   buffer;   
  	if (stat (filePath, &buffer) == 0) {
	    // Init matcher
		fiftyoneDegreesDataSetInitStatus status = fiftyoneDegreesInitWithPropertyString(filePath, properties);
		switch (status) {
			case DATA_SET_INIT_STATUS_SUCCESS:
                Py_RETURN_NONE;
			case DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY:
				PyErr_SetString(PyExc_RuntimeError, "Unable to initialise dataset. There was insufficient memory.");
				return NULL;
			case DATA_SET_INIT_STATUS_CORRUPT_DATA:
				PyErr_SetString(PyExc_RuntimeError, "Unable to initialise dataset. Data file was corrupt. Make sure it is uncompressed.");
				return NULL;
			case DATA_SET_INIT_STATUS_INCORRECT_VERSION:
				PyErr_SetString(PyExc_RuntimeError, "Unable to initialise dataset. The data file was an unsupported version. Make sure you're using the latest data and api.");
				return NULL;
			case DATA_SET_INIT_STATUS_FILE_NOT_FOUND:
				PyErr_SetString(PyExc_RuntimeError, "Unable to initialise dataset.  The data file could not be found.");
				return NULL;
		}
	}
	else {
		PyErr_SetString(PyExc_IOError, "Data file not found.");
		return NULL;
	}
}

static PyObject *py_match(PyObject *self, PyObject *args)
{
    // Input user agent.
    const char *userAgent;

    // Output CSV buffer.
    char output[OUTPUT_BUFFER_LENGTH];

    // Parse input arguments: user agent (string).
    if (!PyArg_ParseTuple(args, "s", &userAgent)) {
        return NULL;
    }

    // Match user agent & fetch properties.
    if (fiftyoneDegreesProcessDeviceCSV(fiftyoneDegreesGetDeviceOffset(userAgent), output, OUTPUT_BUFFER_LENGTH) < 0) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to process device CSV.");
        return NULL;
    } else {
        return  Py_BuildValue("s", output);
    }
}

static PyMethodDef wrapperMethods[] =
{
     {"init", (PyCFunction)py_init, METH_VARARGS, "Initializes wrapper."},
     {"match", (PyCFunction)py_match, METH_VARARGS, "Matches user agent string."},
     {NULL, NULL, 0, NULL}
};

#if PY_MAJOR_VERSION >= 3
static int _fiftyone_degrees_mobile_detector_v3_trie_wrapper_traverse(PyObject *m, visitproc visit, void *arg)
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int _fiftyone_degrees_mobile_detector_v3_trie_wrapper_clear(PyObject *m)
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef wrapperDefinition =
{
        PyModuleDef_HEAD_INIT,
        "_fiftyone_degrees_mobile_detector_v3_trie_wrapper",
        NULL,
        sizeof(struct module_state),
        wrapperMethods,
        NULL,
        _fiftyone_degrees_mobile_detector_v3_trie_wrapper_traverse,
        _fiftyone_degrees_mobile_detector_v3_trie_wrapper_clear,
        NULL
};

#define INITERROR return NULL
#else
#define INITERROR return
#endif

#if PY_MAJOR_VERSION >= 3
PyObject *PyInit__fiftyone_degrees_mobile_detector_v3_trie_wrapper(void)
#else
void init_fiftyone_degrees_mobile_detector_v3_trie_wrapper(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *module = PyModule_Create(&wrapperDefinition);
#else
    PyObject *module = Py_InitModule("_fiftyone_degrees_mobile_detector_v3_trie_wrapper", wrapperMethods);
#endif

    if (module == NULL) {
        INITERROR;
    }
    struct module_state *st = GETSTATE(module);

    st->error = PyErr_NewException("_fiftyone_degrees_mobile_detector_v3_trie_wrapper.Error", NULL, NULL);
    if (st->error == NULL) {
        Py_DECREF(module);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return module;
#endif
}
