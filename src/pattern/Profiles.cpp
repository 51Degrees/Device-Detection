/* *********************************************************************
* This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
* Copyright 2015 51Degrees Mobile Experts Limited, 5 Charlotte Close,
* Caversham, Reading, Berkshire, United Kingdom RG4 7BY
*
* This Source Code Form is the subject of the following patent
* applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
* Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
* European Patent Application No. 13192291.6; and
* United States Patent Application Nos. 14/085,223 and 14/085,301.
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
#include "Profiles.hpp"

/**
* Creates a new profiles instance.
*/
Profiles::Profiles() {}

/**
* Frees the memory used by the profiles structure.
*/
Profiles::~Profiles() {
	fiftyoneDegreesFreeProfilesStruct(profiles);
}

/**
 * Returns the number of profiles in the object.
 * @returns count the number of profiles.
 */
int Profiles::getCount() {
	return profiles->count;
}

/**
 * Returns the profile index of the requested profile. returns -1 if
 * the profile requested is out of range.
 * @param index of the profile to return the profile index of.
 * @returns the index in the data sets profiles structure.
 */
int Profiles::getProfileIndex(int index) {
	if (index < profiles->count) {
		return profiles->indexes[index];
	}
	else {
		return -1;
	}
}

/**
 * Returns the profile id of the requested profile. Returns -1 if the
 * profile requested is out of range.
 * @param index of the profile to return the profile id of.
 * @returns the profiles profile id.
 */
int Profiles::getProfileId(int index) {
	if (index < profiles->count) {
		return profiles->profiles[index]->profileId;
	}
	else {
		return -1;
	}
}