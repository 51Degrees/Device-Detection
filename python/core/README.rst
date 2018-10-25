|51degrees|

Device Detection Python API

51Degrees Mobile Detector is a server side mobile detection solution.

Changelog
====================

- Fixed a bug where an additional compile argument was causing compilation errors with clang.
- Updated the v3-trie-wrapper package to include the Lite Hash Trie data file.

General
========
	
Before you start matching user agents, you may wish to configure the solution to use a different datadase. You can easily generate a sample settings file running the following command
			
	$ 51degrees-mobile-detector settings > ~/51degrees-mobile-detector.settings.py

The core ``51degrees-mobile-detector`` is included as a dependency when installing either the ``51degrees-mobile-detector-v3-wrapper`` or ``51degrees-mobile-detector-v3-wrapper`` packages.

During install a directory which contains your data file will be created in ``~\51Degrees``.

Settings
=========
General Settings
----------------

- ``DETECTION_METHOD`` (defaults to 'v3-wrapper'). Sets the preferred mobile device detection method. Available options are v3-wrapper (requires 51degrees-mobile-detector-v3-wrapper package), v3-trie-wrapper
- ``PROPERTIES`` (defaults to ''). List of case-sensitive property names to be fetched on every device detection. Leave empty to fetch all available properties.
- ``LICENCE`` Your 51Degrees license key for enhanced device data. This is required if you want to set up the automatic 51degrees-mobile-detector-premium-pattern-wrapper package updates.

Trie Detector settings
-----------------------

- ``V3_TRIE_WRAPPER_DATABASE`` Location of the Hash Trie data file.

Pattern Detector settings
--------------------------

- ``V3_WRAPPER_DATABASE`` Location of the Pattern data file.
- ``CACHE_SIZE`` (defaults to 10000). Sets the size of the workset cache.
- ``POOL_SIZE`` (defaults to 20). Sets the size of the workset pool.

Usage Sharer Settings
----------------------

- ``USAGE_SHARER_ENABLED`` (defaults to True). Indicates if usage data should be shared with 51Degrees.com. We recommended leaving this value unchanged to ensure we're improving the performance and accuracy of the solution.
- Adavanced usage sharer settings are detailed in your settings file.

Automatic Updates
------------------
If you want to set up automatic updates, add your license key to your settings and add the following command to your cron

	$ 51degrees-mobile-detector update-premium-pattern-wrapper
	
NOTE: Currently auto updates are only available with our Pattern API.
	
Usage
======
Core
-----

By executing the following a useful help page will be displayed explaining basic usage.

	$ 51degrees-mobile-detector
	
To check everything is set up , try fetching a match with
	
	$ 51degrees-mobile-detector match "Mozilla/5.0 (iPad; CPU OS 5_1 like Mac OS X) AppleWebKit/534.46 (KHTML, like Gecko) Mobile/9B176"
	
Examples
=========

Additional examples can be found on our GitHub_ repository.

User Support
============

If you have any issues please get in touch with our Support_ or open an issue on our GitHub_ repository.

.. |51degrees| image:: https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx?utm_source=github&utm_medium=repository&utm_content=readme_pattern&utm_campaign=python-open-source
	:target: https://51degrees.com

.. _GitHub: https://github.com/51Degrees/Device-Detection/tree/master/python

.. _Support: support@51degrees.com
