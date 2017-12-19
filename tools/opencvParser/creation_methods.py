#!/usr/bin/env python

##
## creation_methods.py
## ~~~~~~~~~
##
## Copyright (c) 2017 Laurent Winkler lwinkler888 at gmail dot com
## 
## This file is part of Markus source files

			

def generate(fout, methods):
	fout.write("""

#include "Feature2D.h"

using namespace std;
using namespace cv;

using vector_float = vector<float>;
using vector_int = vector<int>;
using SimpleBlobDetector_Params = SimpleBlobDetector::Params;

namespace mk {
cv::Ptr<cv::Feature2D> Feature2D::create(const mkjson& x_config) {
	const mkjson& params(x_config.at("parameters"));
	const string name = x_config.at("name");
	const int num = x_config.at("number");

""")

	last_class_name = ''

	# Iterate the list of methods
	for meth in methods:
		method_name = meth[0].split('.')[-1]
		class_name = meth[0][:-len(method_name) - 1].replace('.', '::')
		fct_num = 0 if last_class_name != class_name else fct_num + 1
		print 'Create ' + method_name + " method for child class " + class_name

		# Create the list of arguments
		myargs = ""
		for arg in meth[3]:
			# print arg
			arg_type = arg[0]
			arg_name = arg[1]
			arg_val  = arg[2]

			if arg_val:
				# myargs += 'params.value<%s>("%s"), ' % (arg.type.spelling, arg.spelling)
				if arg_type[-1] == '&':
					myargs += 'params.value<%s>("%s", %s), ' % (arg_type[:-1], arg_name, arg_val)
				else:
					myargs += 'params.value<%s>("%s", %s), ' % (arg_type, arg_name, arg_val)
			else:
				# myargs += 'params.value<%s>("%s"), ' % (arg.type.spelling, arg.spelling)
				if arg_type[-1] == '&':
					myargs += 'params.at("%s").get<%s>(), ' % (arg_name, arg_type[:-1])
				else:
					myargs += 'params.at("%s").get<%s>(), ' % (arg_name, arg_type)
		fout.write("""
	if(name == "%s" && num == %d)
		return %s::create(%s);
		""" % (class_name[4:], fct_num, class_name, '' if (len(myargs) <= 2) else myargs[:-2]))
		last_class_name = class_name
					# print parent_class
	# print parent_class.type.get_declaration()
	# print parent_class.type.get_declaration().get_children()
	# print parent_class.lexical_parent.spelling

	fout.write("""
	throw MkException("Unknown name and function number for creation of descriptor: " + name + string("/") + to_string(num), LOC);
}
} // namespace mk

""")

def generate_methods(methods):
	# Start main
	with open('modules/Feature2D/Feature2D.create.gen.cpp', 'w') as fout:
		generate(fout, methods)


