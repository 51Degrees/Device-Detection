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
inputFile = '../../data/20000 User Agents.csv'
outputFile = 'offlineProcessingOutput.csv'

-- Carries out match for first 20 User-Agents and prints results to
-- output file.
function output_offline_processing()
	fin = io.input(inputFile)
	fout = io.output(outputFile)
	io.write('User-Agent')
	for name in string.gmatch(config.properties,"[^,]+") do
		io.write(string.format('|%s', name))
	end
	io.write('\n')
	i = 0
	while i < 20 do
		userAgent = io.read("*line")
		io.write(userAgent)
		match = provider:getMatch(userAgent)
		for name in string.gmatch(config.properties, "[^,]+") do
			value = match:getValue(name)
			io.write(string.format('|%s', value))
		end
		io.write('\n')
		i = i + 1
	end
	io.close()
end

-- Initialise the provider
config={
    dataFile="../../data/51Degrees-LiteV3.2.dat",
    properties="IsMobile,PlatformName,BrowserName"}
provider=loadfile("../FiftyOneDegrees.lua")(config)

print('Starting Offline Processing')
output_offline_processing()
print(string.format('Output Written to %s', outputFile))
