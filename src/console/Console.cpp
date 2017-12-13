/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright 2017 51Degrees Mobile Experts Limited, 5 Charlotte Close,
 * Caversham, Reading, Berkshire, United Kingdom RG4 7BY
 *
 * This Source Code Form is the subject of the following patents and patent
 * applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
 * Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
 * European Patent No. 2871816;
 * European Patent Application No. 17184134.9;
 * United States Patent Nos. 9,332,086 and 9,350,823; and
 * United States Patent Application No. 15/686,066.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.
 *
 * If a copy of the MPL was not distributed with this file, You can obtain
 * one at http://mozilla.org/MPL/2.0/.
 *
 * This Source Code Form is "Incompatible With Secondary Licenses", as
 * defined by the Mozilla Public License, v. 2.0.
 ********************************************************************** */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../pattern/Provider.hpp"

using namespace std;

#ifdef HTTP_HEADERS_PREFIXED
#define FIRST_HEADER "HTTP_USER_AGENT"
#define SECOND_HEADER "DEVICE_STOCK_UA"
#else
#define FIRST_HEADER "User-Agent"
#define SECOND_HEADER "Device-Stock-UA"
#endif

const char *PROPERTY_LIST = "Id,IsTablet,IsMobile,IsCrawler,DeviceType";

const char *TARGET_USER_AGENTS[] = {
    // Internet explorer
    "Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko",
    // Internet explorer (again to test the cache)
    "Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko",
    // A set top box running Android, not a mobile device.
    "Mozilla/5.0 (Linux; U; Android 4.1.1; nl-nl; Rikomagic MK802IIIS Build/JRO03H) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Safari/534.30",
    // Galaxy Note from Samsung.
    "Mozilla/5.0 (Linux; U; Android 4.1.2; de-de; GT-N8020 Build/JZO54K) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Safari/534.30",
    // A possible future version of the Galaxy Note to show how numeric fallback handling works.
    "Mozilla/5.0 (Linux; U; Android 4.1.2; de-de; GT-N8420 Build/JZO54K) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Safari/534.30",
    // Crawler
    "Mozilla/5.0 (compatible; AhrefsBot/3.1; +http://ahrefs.com/robot/)",
    // Modern version of Chrome
    "Mozilla/5.0 (Windows NT 6.3; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/43.0.2357.81 Safari/537.36",
    // Internet explorer (again to test the cache)
    "Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko",
	// Non Mozilla prefixed
	"Dalvik/2.1.0 (Linux; U; Android 5.0.1; HTC One_M8 Build/LRX22C)",
	// Unusual
	"DOSarrest Monitor/1.3 (Linux)"
};
int TARGET_USER_AGENTS_LENGTH = 9;

#define RANDOM_INDEX(r) rand() % r

void display(vector<string> *properties, Match *match) {
    cout << "Id: " << match->getDeviceId() << "\r\n";
    cout << "Method: " << match->getMethod() << "\r\n";
    cout << "Difference: " << match->getDifference() << "\r\n";
    cout << "Rank: " << match->getRank() << "\r\n";
    for (vector<string>::iterator it = properties->begin();
        it != properties->end();
        ++it) {
        string value = match->getValue(it - properties->begin());
        cout << "(" << it - properties->begin() << ") " << *it << ": " << value << "\r\n";
    }
    delete match;
}

void display(map<string, vector<string> > *result) {
    int index = 0;
    for (map<string, vector<string> >::iterator pit = result->begin();
        pit != result->end();
        ++pit) {
        cout << "(" << index << ") " << pit->first << ": ";
        for (size_t vit = 0; vit < pit->second.size(); vit++) {
            cout << pit->second[vit];
            if (vit < pit->second.size() - 1) {
                cout << ",";
            }
        }
        cout << "\r\n";
        index++;
    }
}

int main(int argc, char* argv[])
{
	if (argc > 1) {
		Provider *provider = new Provider(string(argv[1]), string(PROPERTY_LIST));
        vector<string> properties = provider->getAvailableProperties();

		cout << "\r\n*** Data Set Information ***\r\n";
		cout << "Name: " << provider->getDataSetName() << "\r\n";
		cout << "Format: " << provider->getDataSetFormat() << "\r\n";
		cout << "Published: " << provider->getDataSetPublishedDate() << "\r\n";
		cout << "Next Update: " << provider->getDataSetNextUpdateDate() << "\r\n";
        cout << "Signatures: " << provider->getDataSetSignatureCount() << "\r\n";
        cout << "Device Combinations: " << provider->getDataSetDeviceCombinations() << "\r\n";

        cout << "\r\n*** Available Headers ***\r\n";
        vector<string> httpHeaders = provider->getHttpHeaders();
        for (size_t i = 0; i < httpHeaders.size(); i++) {
            cout << httpHeaders[i] << "\r\n";
        }

        cout << "\r\n*** Available Properties ***\r\n";
        for (size_t i = 0; i < properties.size(); i++) {
            cout << properties[i] << "\r\n";
        }

		for (int i = 0; i < TARGET_USER_AGENTS_LENGTH;  i++) {
			string firstHttpHeader(TARGET_USER_AGENTS[i]);
			string secondHttpHeader(TARGET_USER_AGENTS[RANDOM_INDEX(TARGET_USER_AGENTS_LENGTH)]);

			cout << "\r\n*** Input Headers ***\r\n";
			cout << FIRST_HEADER << ": " << firstHttpHeader << "\r\n";
			cout << SECOND_HEADER << ": " << secondHttpHeader << "\r\n";

			map<string,string> httpHeaders;
			httpHeaders.insert(pair<string,string>(
                FIRST_HEADER,
                string(TARGET_USER_AGENTS[i])));
			httpHeaders.insert(pair<string,string>(
                SECOND_HEADER, secondHttpHeader));

			cout << "\r\n*** HTTP Headers Array - Map ***\r\n";
			display(&provider->getMatchMap(httpHeaders));

			cout << "\r\n*** HTTP Headers Array - Match ***\r\n";
			display(&properties, provider->getMatch(httpHeaders));

			cout << "\r\n*** HTTP Headers Array - JSON ***\r\n";
			cout << provider->getMatchJson(httpHeaders) << "\r\n";

			cout << "\r\n*** User-Agent - Map ***\r\n";
			display(&provider->getMatchMap(string(TARGET_USER_AGENTS[i])));

			cout << "\r\n*** User-Agent - Match ***\r\n";
			display(&properties, provider->getMatch(string(TARGET_USER_AGENTS[i])));

			cout << "\r\n*** User-Agent - JSON ***\r\n";
			cout << provider->getMatchJson(string(TARGET_USER_AGENTS[i])) << "\r\n";
		}
		delete provider;
	}

    // Wait for a character to be pressed.
	cin.ignore();

    return 0;
}
