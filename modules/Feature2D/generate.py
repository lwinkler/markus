#!/usr/bin/env python

##
## generate.py
## ~~~~~~~~~
##
## Copyright (c) 2017 Laurent Winkler lwinkler888 at gmail dot com
## 
## This file is part of Markus source files

""" Usage: call with <header> <enum with namespace>
"""

import sys
import clang.cindex as cindex
from pprint import pprint

cindex.Config.set_library_path("/usr/lib/llvm-4.0/lib")

def describe_node(node):
	""" Describe a node. For debug purposes
	"""

	print "node %s" % (node.spelling)

	pprint(node.__dict__)
	# pprint(node._tu.__dict__)
	# pprint(node._tu.index.__dict__)
	# pprint(node._tu.index.obj.__dict__)
	# print dir(node)

	# Recurse for children of this node
	def descr(node, tabs):
		for c in node.get_children():
			print "%s- %s: %s %s" % (tabs, c.spelling, c.kind, c.data)
			# print dir(c)
			# pprint(c._tu.index.obj.__dict__)
			descr(c, tabs + '\t')
	descr(node, '\t')

def find_inherited_in_node(node, parent):
	found = []

	# Recurse for children of this node
	for c in node.get_children():
		# print 'Search %s' % c.spelling
		if c.kind == cindex.CursorKind.CXX_BASE_SPECIFIER and c.spelling == parent:
			print "Found child %s (%s)" % (node.spelling, c.spelling)
			# describe_node(c)
			found += [node]
			break
		else:
			found += find_inherited_in_node(c, parent)

	
	return found

def find_inherited_in_nodes(nodes, parent):
	""" Find all classes inheriting from one parent
	"""

	found = []

	# Recurse for children of this node
	for c in nodes:
		found += find_inherited_in_node(c, parent)
	
	return found

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
	
def generate_features2d():
	fout.write("""

#include "Feature2D.h"

using namespace std;
using namespace cv;

namespace mk {
cv::Ptr<cv::Feature2D> Feature2D::create(const mkjson& x_config) {
	const mkjson& params(x_config.at("parameters"));
	const string name = x_config.at("name");
	const int num = x_config.at("number");

""")
	header = 'opencv/modules/features2d/include/opencv2/features2d.hpp'
	index = cindex.Index.create()
	src = header
	tu = index.parse(src, args=['-x', 'c++'])

	cv_nodes = find_nodes(tu.cursor, ['cv'])
	parent_class = find_node(tu.cursor, 'cv::Feature2D')
	# describe_node(parent_class)
	# print dir(parent_class)
	print "Inherited by: "
	children = find_inherited_in_nodes(cv_nodes, 'class cv::Feature2D')
	for c in children:
		print c.spelling
		fct_num = 0
		for cc in c.get_children():
			if cc.spelling == 'create':
				print 'Found create'
				myargs = ""
				for arg in  cc.get_arguments():
					# print dir(arg)
					describe_node(arg)
					# myargs += 'params.value<%s>("%s"), ' % (arg.type.spelling, arg.spelling)
					if arg.type.spelling[-1] == '&':
						myargs += 'params.at("%s").get<%s>(), ' % (arg.spelling, arg.type.spelling[:-1])
					else:
						myargs += 'params.at("%s").get<%s>(), ' % (arg.spelling, arg.type.spelling)
				fout.write("""
	if(name == "%s" && num == %d)
		return cv::%s::create(%s);
""" % (c.spelling, fct_num, c.spelling, '' if (len(myargs) <= 2) else myargs[:-2]))
				fct_num += 1
					# print parent_class
	# print parent_class.type.get_declaration()
	# print parent_class.type.get_declaration().get_children()
	# print parent_class.lexical_parent.spelling

	fout.write("""
	throw MkException("Unknown name and functio number for creation of descriptor: " + name + string("/") + to_string(num), LOC);
}
} // namespace mk

""")

# Start main
with open('modules/Feature2D/Feature2D.create.gen.cpp', 'w') as fout:
	generate_features2d()



