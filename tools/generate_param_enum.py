#!/usr/bin/env python

##
## find_enum.py
## ~~~~~~~~~
##
## Copyright (c) 2017 Laurent Winkler lwinkler888 at gmail dot com
## 

""" Usage: call with <filename> <typename>
"""

import sys
import os
import clang.cindex as cindex
from subprocess import call
from pprint import pprint

cindex.Config.set_library_path("/usr/lib/llvm-4.0/lib")

def find_class(node, ns_path):
	""" Find all classes by namespaces
	"""

	found = []

	# Recurse for children of this node
	for c in node.get_children():
		if c.spelling == ns_path[0]:
			if len(ns_path) == 1:
				found += [c]
			else:
				found += find_class(c, ns_path[1:])
	
	return found


def main():
	if len(sys.argv) < 1:
		print "usage: %s <header> <arguments...>" % sys.argv[0]
		exit(1)

	index = cindex.Index.create()
	src = sys.argv[1]
	# return [src, index.parse(src, ["-D_POP_PARSER_"] + sys.argv[2:])]

	tu = index.parse(src, args=['-x', 'c++'])
	# print dir(tu)
	# [src, tu] = init_tu(sys.argv)
	# tu = index.parse(src, ["-I."])

	all_classes = find_class(tu.cursor, sys.argv[2].split("::"))

	print "found %d node(s):" % len(all_classes)
	if len(all_classes) != 1:
		exit(1)
	for c in all_classes:
		# print "class %s in %s" % (c.spelling, c.location)
		# print c.kind
		if c.kind != cindex.CursorKind.ENUM_DECL:
			raise Exception("Expecting an enum and not %s" % c.kind)
		for cc in c.get_children():
			print "%s %s" % (cc.spelling, cc.enum_value)
			# print cc.data
			# print dir(cc.data)
			# for ccc in cc.data:
				# print dir(ccc)
				# print ccc.numerator
				# print ccc.real

# Start main
main()
