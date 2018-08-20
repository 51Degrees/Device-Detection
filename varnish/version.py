#!/usr/bin/python

import sys

default = "040100"

if len(sys.argv) > 1:
	versionOut = "";
	versionIn = sys.argv[1].split('.')
	for i in range(3):
		if len(versionIn) > i:
			if len(versionIn[i]) < 2:
				versionOut += "0" + versionIn[i]
			else:
				versionOut += versionIn[i]
		else:
			versionOut += "00"
	print versionOut
else:
	print default