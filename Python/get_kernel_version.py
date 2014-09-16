#!/usr/bin/python
#
# Get Linux kernel version from include/linux/version.h

import sys, os, re

def main(argv):
	header = "include/linux/version.h"
	const = ".*LINUX_VERSION_CODE"

	if len(sys.argv) < 2:
		print "Usage: " + sys.argv[0] + " <kernel-path>"
		return

	path = sys.argv[1] + "/" + header;

	if not os.path.isfile(path):
		print "No version header: " + path
		return

	for line in open(path):
		if re.match(const,line):
			tokens = line.split(' ')
			code = int (tokens[2])
			print "Kernel code {}".format(code)
			a = code >> 16 & 0xFF;
			b = code >> 8 & 0xFF;
			c = code & 0xFF;
			print "Kernel version {}.{}.{}".format(a, b, c)
			break
	

if __name__ == "__main__":
	main(sys.argv[0:])
