/*

This Source Code Form is copyright of Yorkshire, Inc.
Copyright © 2014 Yorkshire, Inc,
Guiyang, Guizhou, China

This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
Copyright © 2014 51Degrees Mobile Experts Limited, 5 Charlotte Close,
Caversham, Reading, Berkshire, United Kingdom RG4 7BY

This Source Code Form is the subject of the following patent
applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
European Patent Application No. 13192291.6; and
United States Patent Application Nos. 14/085,223 and 14/085,301.

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0.

If a copy of the MPL was not distributed with this file, You can obtain
one at http://mozilla.org/MPL/2.0/.

This Source Code Form is “Incompatible With Secondary Licenses”, as
defined by the Mozilla Public License, v. 2.0.

*/

#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <node.h>
#include <v8.h>
#include <nan.h>
#include "../../../src/trie/51Degrees.h"

#define BUFFER_LENGTH 50000

#ifdef _MSC_VER
#define _INTPTR 0
#endif

#ifdef _MSC_FULL_VER
#define snprintf _snprintf
#endif

using namespace v8;
using namespace node;

//
// TrieParser: the `ObjectWrap` for pattern
//
class TrieParser : public ObjectWrap {
public:

  // @constructor
  // @filename: the database of file, must be *.trie
  // @required_properties: specify properties that would be returned
  TrieParser(char * filename, char * required_properties);

  // @destructor
  ~TrieParser();

  // land this class to node.js runtime
  static void Init(Handle<Object> target);

  // TrieParser.prototype.constructor
  static NAN_METHOD(New);

  // TrieParser.prototype.parse
  static NAN_METHOD(Parse);

private:
  int init_result;
};

#endif
