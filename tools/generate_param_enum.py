#!/usr/bin/env python

##
## find_enum.py
## ~~~~~~~~~
##
## Copyright (c) 2017 Laurent Winkler lwinkler888 at gmail dot com
## 

""" Usage: call with <header> <enum with namespace>
"""

import sys
import clang.cindex as cindex

cindex.Config.set_library_path("/usr/lib/llvm-4.0/lib")

def find_class(node, ns_path):
	""" Find all classes by namespaces
	"""

	found = []

	# Recurse for children of this node
	for c in node.get_children():
		# print 'Search %s' % c.spelling
		if c.spelling == ns_path[0]:
			if len(ns_path) == 1:
				found += [c]
			else:
				found += find_class(c, ns_path[1:])
	
	return found

def generate_param_enum(header_file, node, full_name):

	enum_map = ''

	# print c.kind
	if node.kind != cindex.CursorKind.ENUM_DECL:
		raise Exception('Expecting an enum and not %s' % node.kind)
	for cc in node.get_children():
		# Remove prefix mk_ if present (to avoid problems with CV_8U... macros)
		a = cc.spelling[3:] if cc.spelling.startswith('mk_') else cc.spelling
		enum_map += '\n\t{"%s", %s},' % (a, cc.enum_value)

	fout.write("""

#include "%s"

// Static variables
template<> const map<string, int>  ParameterEnumT<%s>::Enum =
{%s
};
template<> const map<int, string> ParameterEnumT<%s>::ReverseEnum = ParameterEnum::CreateReverseMap(ParameterEnumT<%s>::Enum);
template<> const string ParameterEnumT<%s>::m_type = "%s";

""" % (header_file, full_name, enum_map[:-1], full_name, full_name, full_name, node.spelling))


def main():
	# if len(sys.argv) < 1:
		# print 'usage: %s <header> <enum with namespace>' % sys.argv[0]
		# exit(1)


	# add here all the enums to generate
	header_and_enums = [
		['util/enums.h', 'mk::CachedState'],
		['util/enums.h', 'mk::ImageType'],
		['opencv/modules/ml/include/opencv2/ml.hpp', 'cv::ml::SVM::KernelTypes'],
		['opencv/modules/ml/include/opencv2/ml.hpp', 'cv::ml::SVM::Types']
	]

	# Generate file
	fout.write("""/* This file was generated automatically the generate_param_enum.py parser */

#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include "ParameterEnumT.h"

using namespace std;

""")

	for header_and_enum in header_and_enums:
		# parse the code starting at the given header
		header_file = header_and_enum[0]
		index = cindex.Index.create()
		src = header_file
		tu = index.parse(src, args=['-x', 'c++'])

		enum_full_name = header_and_enum[1]
		enum_classes = find_class(tu.cursor, enum_full_name.split('::'))

		# print 'found %d node(s):' % len(all_classes)
		if len(enum_classes) != 1:
			raise Exception('Expecting to find one node for %s, found %d' % (enum_full_name, len(enum_classes)))

		generate_param_enum(header_file, enum_classes[0], enum_full_name)

# Start main
with open("bulk/ParameterEnumT.cpp", "w") as fout:
	main()
