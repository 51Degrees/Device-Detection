--[[
 This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 Copyright 2017 51Degrees Mobile Experts Limited, 5 Charlotte Close,
 Caversham, Reading, Berkshire, United Kingdom RG4 7BY
 
 This Source Code Form is the subject of the following patents and patent
 applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
 Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
 European Patent No. 2871816;
 European Patent Application No. 17184134.9;
 United States Patent Nos. 9,332,086 and 9,350,823; and
 United States Patent Application No. 15/686,066.
 
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0.
 
 If a copy of the MPL was not distributed with this file, You can obtain
 one at http://mozilla.org/MPL/2.0/.
 
 This Source Code Form is "Incompatible With Secondary Licenses", as
 defined by the Mozilla Public License, v. 2.0.
--]]
mobileUserAgent="Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) AppleWebKit/537.51.2 (KHTML, like Gecko) 'Version/7.0 Mobile/11D167 Safari/9537.53"

desktopUserAgent="Mozilla/5.0 (Windows NT 6.3; WOW64; rv:41.0) Gecko/20100101 Firefox/41.0"

mediaHubUserAgent="Mozilla/5.0 (Linux; Android 4.4.2; X7 Quad Core Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/30.0.0.0 Safari/537.36"

function output_metrics(match)
	print(string.format('   Id: %s', match:getDeviceId()))
	print(string.format('   Match Method: %s', match:getMethod()))
	print(string.format('   Difference: %s', match:getDifference()))
	print(string.format('   Rank: %s', match:getRank()))
end
	
-- Initialise the provider
config={
    dataFile="../../data/51Degrees-LiteV3.2.dat",
    properties="IsMobile"}
provider=loadfile("../FiftyOneDegrees.lua")(config)

print('Starting Getting Started Match Metrics Example.')

-- Carries out a match with a mobile User-Agent.
print(string.format('\nMobile User-Agent: %s', mobileUserAgent))
match = provider:getMatch(mobileUserAgent)
output_metrics(match)

-- Carries out a match with a desktop User-Agent.
print(string.format('\nDesktop User-Agent: %s', desktopUserAgent))
match = provider:getMatch(desktopUserAgent)
output_metrics(match)

-- Carries out a match with a MediaHub User-Agent.
print(string.format('\nMedia Hub User-Agent: %s', mediaHubUserAgent))
match = provider:getMatch(mediaHubUserAgent)
output_metrics(match)
