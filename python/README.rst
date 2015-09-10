Overview
========

51Degrees.mobi Mobile Detector for Python (>= 2.6) allows fetching properties of mobile devices based on user agent strings. Several detection methods are available. Choose your preferred method and start supporting mobile devices in your website. The solution can be combined with emerging methodologies such as responsive design or progressive enhancement to produce great results quickly.

A middleware and a context processor are also available for Django users. Enable the 51Degrees.mobi Mobile Detector middleware and access properties of client devices directly through the ``device`` property in the ``Request`` instance.

Quickstart
==========

Complete the following 3 steps in your Python environment to start getting information about requesting mobile devices.

Install
-------

51Degrees.mobi Mobile Detector for Python is composed by several packages available in the Python Package Index (PyPI) repository: the core package (``51degrees-mobile-detector``) and two packages containing specific detection methods (``51degrees-mobile-detector-lite-pattern-wrapper`` and ``51degrees-mobile-detector-trie-wrapper``). An additional premium package (``51degrees-mobile-detector-premium-pattern-wrapper``) is available for download in the 51Degrees website for users owning a valid license key.

All detection packages are very efficient Python wrappers of the pattern-based and trie-based `C detection solutions <http://51degrees.mobi/Support/Documentation/C.aspx>`_. Beware these are C-based implementations that may be not available in your platform.

Check you have a C compiler and the Python development headers installed in your system. Then, simply install the package with your preferred detection method (note several methods may coexist in the same system). The core package will be automatically installed as a dependency. For example::

    $ pip install 51degrees-mobile-detector-trie-wrapper

You may be interested interested in installing the package in a separate Python virtual environment. If so, remember to create and activate the virtual environment before you execute the ``pip`` command.

Next you can find some platform-specific tips:

  - **Linux**. For example, for Ubuntu and Python 2.x, you can install the Python development headers and the virtualenvs support using ``apt``::

        $ sudo apt-get install python-dev python-virtualenv
        $ mkdir -p ~/.virtualenvs
        $ virtualenv ~/.virtualenvs/some-project-name
        $ source ~/.virtualenvs/some-project-name/bin/activate
        $ pip install 51degrees-mobile-detector-lite-pattern-wrapper

  - **OSX**: Recommended approach for OSX users is using `MacPorts <http://www.macports.org>`_. Once installed you can easily prepare your environment using ``port``::

        $ sudo port install py27-pip py27-virtualenv
        $ mkdir -p ~/.virtualenvs
        $ virtualenv-2.7 ~/.virtualenvs/some-project-name
        $ source ~/.virtualenvs/some-project-name/bin/activate
        $ pip-2.7 install 51degrees-mobile-detector-lite-pattern-wrapper

Configure
---------

Before start matching user agents, you need to configure the solution. You can easily generate a sample settings file running the following command::

    $ 51degrees-mobile-detector settings > $HOME/51degrees-mobile-detector.settings.py

Edit the generated settings and set your preferences. Ensure you set ``DETECTION_METHOD`` and ``TRIE_WRAPPER_DATABASE`` (if using the trie-based detection method).

Finally, link your settings file from the ``FIFTYONE_DEGREES_MOBILE_DETECTOR_SETTINGS`` environment variable::

    $ export FIFTYONE_DEGREES_MOBILE_DETECTOR_SETTINGS=$HOME/51degrees-mobile-detector.settings.py

Django users can skip this step and embed settings in their Django settings files. Check out the Django section for extra information.

Match
-----

You are now ready to start matching user agent strings! Check everything have been correctly set up using the command line matching utility::

    $ 51degrees-mobile-detector match "Mozilla/5.0 (iPad; CPU OS 5_1 like Mac OS X) AppleWebKit/534.46 (KHTML, like Gecko) Mobile/9B176"

Alternatively, open a Python console, load the module and start matching::

    >>> from fiftyone_degrees import mobile_detector

    >>> device = mobile_detector.match("Mozilla/5.0 (iPad; CPU OS 5_1 like Mac OS X) AppleWebKit/534.46 (KHTML, like Gecko) Mobile/9B176")

    >>> device.Id
    '15767-18117-17596-18092'

    >>> device.ScreenPixelsWidth
    '768'

    >>> device.method
    'trie-wrapper'

    >>> device.properties
    {'PostMessage': 'True', 'IndexedDB': 'False', 'Canvas': 'True', 'GeoLocation': 'True', 'FileSaver': 'False', 'CssUI': 'True', 'CssTransforms': 'True', 'DataSet': 'False', 'WebWorkers': 'False', 'Json': 'True', 'ScreenPixelsHeight': '1024', 'CssImages': 'False', 'Masking': 'True', 'Progress': 'True', 'Html-Media-Capture': 'False', 'CssFont': 'False', 'CssTransitions': 'True', 'Track': 'False', 'Selector': 'True', 'LayoutEngine': 'Webkit', 'Html5': 'False', 'CssFlexbox': 'False', 'TouchEvents': 'True', 'Viewport': 'True', 'DeviceOrientation': 'True', 'Xhr2': 'False', 'Fullscreen': 'False', 'CssText': 'True', 'Svg': 'False', 'FormData': 'True', 'Prompts': 'True', 'CssBackground': 'True', 'Iframe': 'False', 'FileWriter': 'False', 'CssCanvas': 'False', 'AnimationTiming': 'False', 'CssColor': 'True', 'IsMobile': 'True', 'History': 'False', 'DataUrl': 'True', 'CssPosition': 'True', 'FileReader': 'False', 'CssBorderImage': 'False', 'BlobBuilder': 'False', 'CssMinMax': 'True', 'CssMediaQueries': 'True', 'Video': 'True', 'CssOverflow': 'True', 'ScreenPixelsWidth': '768', 'Id': '15767-18117-17596-18092', 'CssColumn': 'Unknown'}

API
===

The API of the 51Degrees.mobi Mobile Detector for Python is composed by a single matching method::

    match(user_agent, client_ip=None, http_headers=None, method=None)

This method will always return a ``Device`` instance encapsulating the properties of the detected device. ``user_agent`` is the only mandatory argument. Other arguments are:

- ``client_ip`` is a string with the client IP address. If provided and the ``USAGE_SHARER_ENABLED`` option is enabled in your settings, it'll will be submitted to 51Degrees.mobi in order to improve performance and accuracy of further device detections.

- ``http_headers`` is a dictionary containing all HTTP headers. If provided and the ``USAGE_SHARER_ENABLED`` option is enabled in your settings, it'll will be submitted (removing confidential data such as cookies) to 51Degrees.mobi in order to improve performance and accuracy of further device detections.

- ``method`` is a string with the desired device detection method (``lite-pattern-wrapper``, ``premium-pattern-wrapper`` or ``trie-wrapper``). If not specified, value of ``DETECTION_METHOD`` in your settings  will be used as the default detection method.

Configuration
=============

The 51Degrees.mobi Mobile Detector for Python module will try to find the settings file checking the contents of the ``FIFTYONE_DEGREES_MOBILE_DETECTOR_SETTINGS`` environment variable.

Usually you'll define the environment variable during the bootstrap of the project where the mobile detector is being used::

    os.environ['FIFTYONE_DEGREES_MOBILE_DETECTOR_SETTINGS'] = '...'

Also note that the value of the environment variable may be the absolute path of a Python module (e.g. ``myproject.fiftyone_degrees.mobile_settings``). That way you can include the settings file inside your own project, which is the most convenient option for most cases.

If using Django, you can embed these settings into the Django settings file. Check out the Django section for extra information.

Configuration options
---------------------

- **General settings:**

  - ``DETECTION_METHOD`` (defaults to ``'lite-pattern-wrapper'``). Sets the preferred mobile device detection method. Available options are ``lite-pattern-wrapper`` (requires 51degrees-mobile-detector-lite-pattern-wrapper package), ``trie-wrapper`` (requires 51degrees-mobile-detector-trie-wrapper package) and ``premium-pattern-wrapper`` (requires 51degrees-mobile-detector-premium-pattern-wrapper package).

  - ``PROPERTIES`` (defaults to ``()``). List of case-sensitive property names to be fetched on every device detection. Leave empty to fetch all available properties.

  - ``LICENSE``. Your 51Degrees license key. This is required if you want to set up the automatic 51degrees-mobile-detector-premium-pattern-wrapper package updates.

   List of case-sensitive property names to be fetched on every device detection. Leave empty to fetch all available properties.

- **Trie detector (C wrapper) settings:**

  - ``TRIE_WRAPPER_DATABASE``. Location of the database file. If not specified, the trie-based detection method will not be available. Download the latest ``51Degrees.mobi-Lite-*.trie.zip`` file from http://sourceforge.net/projects/fiftyone-c/files/.

- **Usage sharer settings:**

  - ``USAGE_SHARER_ENABLED`` (defaults to ``True``). Indicates if usage data should be shared with 51Degrees.mobi. We recommended leaving this value unchanged to ensure we're improving the performance and accuracy of the solution.

  - ``USAGE_SHARER_MAXIMUM_DETAIL`` (defaults to ``True``). The detail that should be provided relating to new devices. Modification not required for most users.

  - ``USAGE_SHARER_SUBMISSION_URL`` (defaults to ``'http://devices.51degrees.mobi/new.ashx'``). URL to send new device data to. Modification not required for most users.

  - ``USAGE_SHARER_SUBMISSION_TIMEOUT`` (defaults to ``10``). Data submission timeout (seconds).

  - ``USAGE_SHARER_MINIMUM_QUEUE_LENGTH`` (defaults to ``50``). Minimum queue length to lauch data submission.

  - ``USAGE_SHARER_LOCAL_ADDRESSES``. Used to detect local devices. Modification not required for most users.

  - ``USAGE_SHARER_IGNORED_HEADER_FIELD_VALUES``. The content of fields in this list should not be included in the request information sent to 51Degrees. Modification not required for most users.

Automatic updates
=================

All packages can be automatically updated simple adding a ``pip <package name> --upgrade`` command to your cron.

The ``51degrees-mobile-detector-premium-pattern-wrapper`` package it not available through PyPI. If you want to set up automatic updates, add your license key to your settings and add the ``51degrees-mobile-detector update-premium-pattern-wrapper`` command to your cron.

Django
======

51Degrees.mobi Mobile Detector for Python provides a middleware and a context processor designed to simplify as much as possible the integration of the detection solution in Django platforms.

Quickstart
----------

1. Install your preferred mobile detection method (the core package will be installed as a dependency). For example::

    $ pip install 51degrees-mobile-detector-lite-pattern-wrapper

2. Add the 51Degrees.mobi Mobile Detector middleware to your Django settings. Inserting it just after the ``SessionMiddleware`` is a safe place for it::

    MIDDLEWARE_CLASSES = (
        ...
        'django.contrib.sessions.middleware.SessionMiddleware',
        'fiftyone_degrees.mobile_detector.contrib.django.middleware.DetectorMiddleware',
        ...
    )

3. Optionally, add the 51Degrees.mobi Mobile Detector context processor somewhere in the list of context processors in your Django settings::

    TEMPLATE_CONTEXT_PROCESSORS = (
        ...
        'fiftyone_degrees.mobile_detector.contrib.django.context_processors.device',
        ...
    )

4. Configure the solution. When integrating 51Degrees.mobi Mobile Detector for Python in a Django website there is an extra and more convenient option to set your preferences directly in your Django settings. For example::

    FIFTYONE_DEGREES_MOBILE_DETECTOR_SETTINGS = {
        'DETECTION_METHOD': 'lite-pattern-wrapper',
        'PROPERTIES': ('Id', 'IsMobile', 'WebWorkers', 'Html5',),
    }

5. Finally, the middleware provides some extra settings only available for Django users:

  - ``FIFTYONE_DEGREES_MOBILE_DETECTOR_SESSION_CACHE``. If enabled (defaults to ``False``), information about the detected device will be cached in the user's session.

  - ``FIFTYONE_DEGREES_MOBILE_DETECTOR_SESSION_FIELD``. If set (defaults to ``_51degrees_device``) and the session cache is enabled, allows configuring the caching key in the user's session.

Now you are ready to start using 51Degrees.mobi Mobile Detector for Python. All ``Request`` instances will now include a lazily generated ``device`` attribute. Use it directly in your views (``request.device.Id``, ``request.device.IsMobile``, ``request.device.properties``, etc.) and templates (using the ``device`` variable defined in the context, if you are using the 51Degrees.mobi Mobile Detector context processor).
