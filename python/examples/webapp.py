#!/usr/bin/env python
'''
<tutorial>
This has a main function which initializes the provider and
sets a server running on localhost 8080.<br>
Then in the GetHandler class, the do_GET function
runs a match for the devices http headers and carries out
2 different procedures depending on whether the device
is a mobile device or not. This can be made even more useful
with the use of properties such as ScreenInchesDiagonal
and SupportedSensorTypes.
</tutorial>
'''
# // Snippet Start
from BaseHTTPServer import BaseHTTPRequestHandler
import CGIHTTPServer
import cgitb; cgitb.enable()  ## This line enables CGI error reporting
import urlparse
import string
from FiftyOneDegrees import fiftyone_degrees_mobile_detector_v3_wrapper

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
# If usage shaing is enabled, share with 51Degrees
# (core must be installed for this to work).
####################################################

	if shareUsage == 'True':
		client_ip = headers.get('client_ip')
		from fiftyone_degrees.mobile_detector import usage
		usage.UsageSharer.instance().record(client_ip, http_headers)

########################################################
# Set http headers from browser
########################################################

        httpHeaders = fiftyone_degrees_mobile_detector_v3_wrapper
		.MapStringString()

        for header in importantHeaders:
            value = headers.get(header)
	    if value:
                httpHeaders.__setitem__(header, value)
########################################################
# Perform match with multiple http headers
########################################################

        match = provider.getMatch(httpHeaders)

		if (match.getValue('IsMobile') == 'True'):
			self.wfile.write('Load Mobile Page')
		else:
			self.wfile.write('Load Desktop Page')

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
# Set the match perameters
#######################################################

	properties = 'BrowserName,BrowserVendor,BrowserVersion,DeviceType,
	HardwareVendor,IsTablet,IsMobile,IsCrawler,ScreenInchesDiagonal,
	ScreenPixelsWidth'
	dataFile = os.path.expanduser('~/51Degrees/51Degrees-LiteV3.2.dat')
	cacheSize = 1000
	poolSize =  20
	shareUsage = 'False' # Needs Device Detection Python
                             # Core installed for usage sharing

#######################################################
# Initialize the provider
#######################################################

	provider = fiftyone_degrees_mobile_detector_v3_wrapper.Provider(
            dataFile,
            properties,
            cacheSize,
            poolSize)

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
	print 'Starting server at %s:%d,  use <Ctrl-C> to stop' 
	% (server_address, server_port)
	signal.signal(signal.SIGINT, ctrlc)
	server.serve_forever()
# // Snippet End