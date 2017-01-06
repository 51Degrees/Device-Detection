# This is a basic VCL configuration file for varnish.  See the vcl(7)
# man page for details on VCL syntax and semantics.
# 
# Default backend definition.  Set this to point to your content
# server.
# 
vcl 4.0;
import fiftyonedegrees;



backend default {
	.host = "127.0.0.1";
	.port = "80";
}

sub vcl_recv {
	set req.http.X-IsMobile = fiftyonedegrees.match_all("IsMobile");
	set req.http.X-BrowserName = fiftyonedegrees.match_all("BrowserName");
	set req.http.X-PlatformName = fiftyonedegrees.match_all("PlatformName");
	set req.http.X-Difference = fiftyonedegrees.match_all("Difference");
	set req.http.X-Method = fiftyonedegrees.match_all("Method");
	set req.http.X-Rank = fiftyonedegrees.match_all("Rank");
	set req.http.X-DeviceId = fiftyonedegrees.match_all("DeviceId");
}

sub vcl_deliver {
	set resp.http.X-IsMobile = fiftyonedegrees.match_all("IsMobile");
	set resp.http.X-BrowserName = fiftyonedegrees.match_single(req.http.user-agent, "BrowserName");
	set resp.http.X-PlatformName = fiftyonedegrees.match_single(req.http.user-agent, "PlatformName");
	set resp.http.X-Difference = fiftyonedegrees.match_all("Difference");
	set resp.http.X-Method = fiftyonedegrees.match_all("Method");
	set resp.http.X-Rank = fiftyonedegrees.match_all("Rank");
	set resp.http.X-ID = fiftyonedegrees.match_all("DeviceId");
}

sub vcl_init {
	fiftyonedegrees.start("/home/ben/Downloads/51Degrees-EnterpriseV3_2.dat");
}
