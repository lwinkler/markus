#!/usr/bin/env python

##
## creation_methods.py
## ~~~~~~~~~
##
## Copyright (c) 2017 Laurent Winkler lwinkler888 at gmail dot com
## 
## This file is part of Markus source files

			

def generate(fout, header, footer, methods):
	fout.write(header)

	last_class_name = ''

	# Iterate the list of methods
	for meth in methods:
		method_name = meth[0].split('.')[-1]
		class_name = meth[0][:-len(method_name) - 1].replace('.', '::')
		fct_num = 0 if last_class_name != class_name else fct_num + 1
		ret = 'Error' if not meth[4].startswith('Ptr<') else meth[4][4:-1]
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
		return %s::%s(%s);
		""" % (ret, fct_num, class_name, method_name, '' if (len(myargs) <= 2) else myargs[:-2]))
		last_class_name = class_name
					# print parent_class
	# print parent_class.type.get_declaration()
	# print parent_class.type.get_declaration().get_children()
	# print parent_class.lexical_parent.spelling

	fout.write(footer)

def generate_create_method_feature2d(methods):
	# Start main
	with open('modules/Feature2D/Feature2D.create.gen.cpp', 'w') as fout:

		header = """

#include "Feature2D.h"

using namespace std;
using namespace cv;

using vector_float = vector<float>;
using vector_int = vector<int>;
using SimpleBlobDetector_Params = SimpleBlobDetector::Params;

namespace mk {
cv::Ptr<cv::Feature2D> Feature2D::create(const CreationFunction& x_funct) {
	const mkjson& params(x_funct.parameters);
	const string name = x_funct.name;
	const int num = x_funct.number;

"""

		footer = """
	throw MkException("Unknown name and function number for creation of descriptor: " + name + string("/") + to_string(num), LOC);
}
} // namespace mk

"""
		generate(fout, header, footer, methods)


def generate_create_method_stat_model(methods):
	# Start main
	with open('modules/StatModel/StatModel.create.gen.cpp', 'w') as fout:

		header = """

#include "StatModel.h"

using namespace std;
using namespace cv;

// using vector_float = vector<float>;
// using vector_int = vector<int>;
// using SimpleBlobDetector_Params = SimpleBlobDetector::Params;

namespace mk {
cv::Ptr<cv::ml::StatModel> StatModel::create(const CreationFunction& x_funct) {
	const mkjson& params(x_funct.parameters);
	const string name = x_funct.name;
	const int num = x_funct.number;

"""

		footer = """
	throw MkException("Unknown name and function number for creation of descriptor: " + name + string("/") + to_string(num), LOC);
}
} // namespace mk

"""
		generate(fout, header, footer, methods)

def generate_create_method_background_subtractor(methods):
	# Start main
	with open('modules/BackgroundSubtractor/BackgroundSubtractor.create.gen.cpp', 'w') as fout:

		header = """

#include "BackgroundSubtractor.h"

using namespace std;
using namespace cv;

// using vector_float = vector<float>;
// using vector_int = vector<int>;
// using SimpleBlobDetector_Params = SimpleBlobDetector::Params;

namespace mk {
cv::Ptr<cv::BackgroundSubtractor> BackgroundSubtractor::create(const CreationFunction& x_funct) {
	const mkjson& params(x_funct.parameters);
	const string name = x_funct.name;
	const int num = x_funct.number;

"""

		footer = """
	throw MkException("Unknown name and function number for creation of descriptor: " + name + string("/") + to_string(num), LOC);
}
} // namespace mk

"""
		generate(fout, header, footer, methods)
