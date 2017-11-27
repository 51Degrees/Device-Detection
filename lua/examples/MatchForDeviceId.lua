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

-- Initialise the provider
config={
    dataFile="../../data/51Degrees-LiteV3.2.dat",
    properties="IsMobile"}
provider=loadfile("../FiftyOneDegrees.lua")(config)

print('Starting Match With Device Id Example.')

-- Fetches the device id for a mobile User-Agent.
match = provider:getMatch(mobileUserAgent)
mobileDeviceId = match:getDeviceId()

-- Fetches the device id for a desktop User-Agent.
match = provider:getMatch(desktopUserAgent)
desktopDeviceId = match:getDeviceId()

-- Fetches the device id for a MediaHub User-Agent.
match = provider:getMatch(mediaHubUserAgent)
mediaHubDeviceId = match:getDeviceId()

-- Carries out a match with a mobile device id.
print(string.format('\nMobile Device Id: %s', mobileDeviceId))
match = provider:getMatchForDeviceId(mobileDeviceId)
print(string.format('   IsMobile: %s', match:getValue('IsMobile')))

-- Carries out a match with a desktop device id.
print(string.format('\nDesktop Device Id: %s', desktopDeviceId))
match = provider:getMatchForDeviceId(desktopDeviceId)
print(string.format('   IsMobile: %s', match:getValue('IsMobile')))

-- Carries out a match with a MediaHub device id.
print(string.format('\nMediaHub Device Id: %s', mediaHubDeviceId))
match = provider:getMatchForDeviceId(mediaHubDeviceId)
print(string.format('   IsMobile: %s', match:getValue('IsMobile')))
