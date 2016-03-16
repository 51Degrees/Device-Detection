
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