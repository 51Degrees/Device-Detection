#!/usr/bin/env python

'''
:copyright: (c) 2015 by 51Degrees.com, see README.md for more details.
:license: MPL2, see LICENSE.txt for more details.
'''
# // Snippet Start
from BaseHTTPServer import BaseHTTPRequestHandler
import CGIHTTPServer
import cgitb; cgitb.enable()  ## This line enables CGI error reporting
import urlparse
import os
import string
from FiftyOneDegrees import fiftyone_degrees_mobile_detector_v3_trie_wrapper

####################################################
# Set html links globally
####################################################

dataOptions = ('<a class="button" target="_blank" href="https://51degrees.com/'
'compare-data-options?utm_source=github&utm_medium=repository&utm_content='
'server_trie_compare&utm_campaign=python-open-source" title="Compare '
'Premium and Enterprise Data Options">Compare Data Options</a>')
methodHyperLinkUA = ('<a class="button" target="_blank" href="https://'
'51degrees.com/support/documentation/trie?utm_source=github&utm_medium='
'repository&utm_content=example_trie_ua&utm_campaign=python-open-source" '
'title="How Trie Device Detection Works">About Metrics</a>')
methodHyperLinkHeaders = ('<a class="button" target="_blank" href="https://'
'51degrees.com/support/documentation/trie?utm_source=github&utm_medium='
'repository&utm_content=example_trie_headers&utm_campaign=python-open-'
'source" title="How Trie Device Detection Works">About Metrics</a>')
propertiesHyperLinkUA = ('<a class="button" target="_blank" href="https://'
'51degrees.com/resources/property-dictionary?utm_source=github&utm_medium='
'repository&utm_content=server_trie_properties_ua&utm_campaign=python-'
'open-source" title="Review All Properties">All Properties</a>')
propertiesHyperLinkHeaders = ('<a class="button" target="_blank" href="https:'
'//51degrees.com/resources/property-dictionary?utm_source=github&utm_medium='
'repository&utm_content=server_trie_properties_headers&utm_campaign=python'
'-open-source" title="Review All Properties">All Properties</a>')
propertyNotFound = ('<a target="_blank" href="https://51degrees.com/compare-'
'data-options?utm_source=github&utm_medium=repository&utm_content=server_'
'trie_compare&utm_campaign=python-open-source">Switch Data Set</a>')


#######################################################
#------------------------------------------------------
# Handler function
# Called on each request
#------------------------------------------------------
#######################################################

class GetHandler(CGIHTTPServer.CGIHTTPRequestHandler):

    def do_GET(self):

        self.send_response(200, 'OK')
        self.send_header('Content-type', 'text/html')
        self.end_headers()

###################################################
# Fetch http headers from browser request
###################################################

        headers = self.headers

####################################################
# If usage sharing is enabled, share with 51Degrees
# (core must be installed for this to work).
####################################################

	if shareUsage == 'True':
		client_ip = headers.get('client_ip')
		from fiftyone_degrees.mobile_detector import usage
		usage.UsageSharer.instance().record(client_ip, http_headers)

####################################################
# Begin html formatting
####################################################

        self.wfile.write('<!doctype html>')
        self.wfile.write('<html>')
        self.wfile.write('<link rel="stylesheet" type="text/css" href="'
		'https://51degrees.com/Demos/examples.css" class="inline">')
        self.wfile.write('<body>')
        self.wfile.write('<div class="content">')

        self.wfile.write('<p><img src="https://51degrees.com/DesktopModules/'
		'FiftyOne/Distributor/Logo.ashx?utm_source=github&utm_medium='
		'repository&utm_content=server_trie&utm_campaign=python-'
		'open-source"></p>')
        self.wfile.write('<h1>Python Trie - Device Detection '
		'Server Example</h1>')

#####################################################
# Output data set information
#####################################################

        self.wfile.write('<table>')
        self.wfile.write('<tr><th colspan="3">Data Set Information</th></tr>')
        self.wfile.write('<tr><td>Name</td><td>%s</td><td rowspan="6">%s</td>'
		'</tr>' % (provider.getDataSetName(),  dataOptions))
        self.wfile.write('<tr><td>Format</td><td>%s</td></tr>'\
		% "Not available for Trie")
        self.wfile.write('<tr><td>Published Date</td><td>%s</td></tr>'\
		% "Not available for Trie")
        self.wfile.write('<tr><td>Next Update Date</td><td>%s</td></tr>'\
		% "Not available for Trie")
        self.wfile.write('<tr><td>Signature Count</td><td>%s</td></tr>'\
		% "Not available for Trie")
        self.wfile.write('<tr><td>Device Combinations</td><td>%s</td></tr>'\
		% "Not available for Trie")

        self.wfile.write('</table>')

#####################################################
# Perform match with single User-Agent
#####################################################

        userAgent = headers.get('user-agent')
        match = provider.getMatch(userAgent)

#######################################################
# Output properties from match
#######################################################

	self.outputProperties(match, propertiesHyperLinkUA, methodHyperLinkUA)

########################################################
# Set http headers from browser
########################################################

        httpHeaders = fiftyone_degrees_mobile_detector_v3_trie_wrapper\
			.MapStringString()

        self.wfile.write('<table>')
        self.wfile.write('<tr><th colspan="2">Match with HTTP Headers'
		'</th></tr>')
        self.wfile.write('<tr><th colspan="2">Relevant HTTP Headers</th></tr>')
        for header in importantHeaders:
            value = headers.get(header)
	    if value:
                self.wfile.write('<tr><td>%s</td><td>%s</td></tr>'\
				% (header, value))
                httpHeaders.__setitem__(header, value)
	    else:
	        self.wfile.write('<tr><td>%s</td><td>%s</td></tr>'\
			% (header, '<i>header not set</i>'))
        self.wfile.write('</table>')

########################################################
# Perform match with multiple http headers
########################################################

        match = provider.getMatch(httpHeaders)

########################################################
# Output properties
########################################################

	self.outputProperties(match,
	propertiesHyperLinkHeaders,
	methodHyperLinkHeaders)

#######################################################
# End html formatting
#######################################################

        self.wfile.write('</div>')
        self.wfile.write('</body>')
        self.wfile.write('</html>')

#########################################################
# Function which outputs match properties in table
#########################################################

    def outputProperties(self, match, propertiesHyperLink, methodHyperLink):
        self.wfile.write('<table>')
        self.wfile.write('<tr><th colspan="2">Device Properties</th><td '
		'rowspan="%d">%s</td></tr>' % (len(properties.split(',')) + 1,
		propertiesHyperLink))
        for property in properties.split(','):
            value = ' '.join(match.getValues(property))
	    if value:
                self.wfile.write('<tr><td><a target="_blank" href="https:'
				'//51degrees.com/resources/property-dictionary#%s" title="'
				'Read About %s">%s</a></td><td>%s</td></tr>'\
				% (property, property, property, value))
	    else:
	        self.wfile.write('<tr><td><a target="_blank" href="'
			'https://51degrees.com/resources/property-dictionary#%s" '
			'title="Read About %s">%s</a></td><td>%s</td></tr>'\
			% (property, property, property, propertyNotFound))
        self.wfile.write('</table>')

#######################################################
#------------------------------------------------------
# Function to exit cleanly
#------------------------------------------------------
#######################################################

def ctrlc(sig, frame):
	print '\nExiting...'
	server.server_close()
	exit(0)

#######################################################
#------------------------------------------------------
# Main function
# Called when starting server
#------------------------------------------------------
#######################################################

if __name__ == '__main__':
	from BaseHTTPServer import HTTPServer
	import signal

#######################################################
# Set the match parameters
#######################################################

	properties = ('BrowserName,BrowserVendor,BrowserVersion,DeviceType,'
	'HardwareVendor,IsTablet,IsMobile,IsCrawler,ScreenInchesDiagonal,'
	'ScreenPixelsWidth')
	dataFile = os.path.expanduser('~/51Degrees/51Degrees-LiteV3.4.trie')
	shareUsage = 'False' # Needs Device Detection Python
                             # Core installed for usage sharing

#######################################################
# Initialize the provider
#######################################################

	provider = fiftyone_degrees_mobile_detector_v3_trie_wrapper.Provider(
            dataFile,
            properties)

#######################################################
# Fetch important headers from workset
#######################################################

	importantHeaders = provider.getHttpHeaders()

#######################################################
# Setup server
#######################################################

	server_address = 'localhost'
	server_port = 8080
	server = HTTPServer((server_address, server_port), GetHandler)
	print 'Starting server at %s:%d,  use <Ctrl-C> to stop'\
	% (server_address, server_port)
	signal.signal(signal.SIGINT, ctrlc)
	server.serve_forever()

# // Snippet End
