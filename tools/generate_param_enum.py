#!/usr/bin/env python

##
## find_enum.py
## ~~~~~~~~~
##
## Copyright (c) 2017 Laurent Winkler lwinkler888 at gmail dot com
## 
## This file is part of Markus source files

""" Usage: call to generate enum source file ParameterEnumT.gen.cpp
"""

import sys
import clang.cindex as cindex
from pprint import pprint

cindex.Config.set_library_path("/usr/lib/llvm-4.0/lib")

def find_node(node, full_name):
	""" Find class by namespaces
	"""
	classes = find_nodes(node, full_name.split('::'))
	if len(classes) != 1:
		raise Exception('Expecting to find one node for %s, found %d' % (full_name, len(classes)))
	
	return classes[0]

def find_nodes(node, ns_path):
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
				found += find_nodes(c, ns_path[1:])
	
	return found

def find_node(node, full_name):
	""" Find class by namespaces
	"""
	classes = find_nodes(node, full_name.split('::'))
	if len(classes) != 1:
		raise Exception('Expecting to find one node for %s, found %d' % (full_name, len(classes)))
	
	return classes[0]

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
template<> const string ParameterEnumT<%s>::className = "Parameter%s";

""" % (header_file, full_name, enum_map[:-1], full_name, full_name, full_name, node.spelling))


def generate_enums():
	# if len(sys.argv) < 1:
		# print 'usage: %s <header> <enum with namespace>' % sys.argv[0]
		# exit(1)


	# add here all the enums to generate
	header_and_enums = [
			{
				'header': 'util/enums.h', 
				'classes': [
					'mk::CachedState',
					'mk::ImageType'
					],
			},
			{
				'header': 'opencv/modules/ml/include/opencv2/ml.hpp', 
				'classes': [
					'cv::ml::SVM::KernelTypes',
					'cv::ml::SVM::Types'
				]
			},
			{
				'header': 'opencv/modules/imgproc/include/opencv2/imgproc.hpp', 
				'classes': [
					'cv::MorphShapes',
					'cv::MorphTypes'
				]
			}
		]

	# Generate file
	fout.write("""/* This file was generated automatically the generate_param_enum.py parser */

#include "ParameterEnumT.h"

using namespace std;
using namespace mk;

""")

	for header_and_enum in header_and_enums:
		# parse the code starting at the given header
		header_file = header_and_enum['header']
		index = cindex.Index.create()
		src = header_file
		tu = index.parse(src, args=['-x', 'c++'])

		for enum_full_name in header_and_enum['classes']:
			enum_class = find_node(tu.cursor, enum_full_name)

			generate_param_enum(header_file, enum_class, enum_full_name)
	
# Start main
with open('bulk/ParameterEnumT.gen.cpp', 'w') as fout:
	generate_enums()

