#!/usr/bin/env python

##
## generate.py
## ~~~~~~~~~
##
## Copyright (c) 2017 Laurent Winkler lwinkler888 at gmail dot com
## 
## This file is part of Markus source files

import os, sys, re, string
import function_modules
import creation_methods
from hdr_parser import *

def has_parent(class_name, parent_name, cv_classes):
	cl = cv_classes[class_name]
	if not cl[1]:
		return False
	if not cl[1].startswith(': '):
		raise Exception('Expected semicolon at the begining of ' + cl[0])
	
	for par in cl[1][2:].split(' '):
		pn = par.replace('::', '.')
		# print pn + "==" + parent_name
		if pn == parent_name:
			return True
		if has_parent(pn, parent_name, cv_classes):
			return True
	return False


if __name__ == '__main__':

	cv_constants  = {}
	cv_classes    = {}
	cv_structures = {}

	parser = CppHeaderParser()
	decls = []
	nbOk = 0
	tot  = 0

	modules_dir   = 'modules_gen'
	opencv_incdir = 'opencv/modules'
	opencv_hdr_list = [
		opencv_incdir + "/core/include/opencv2/core.hpp",
		opencv_incdir + "/core/include/opencv2/core/ocl.hpp",
		# opencv_incdir + "/flann/include/opencv2/flann/miniflann.hpp",
		opencv_incdir + "/ml/include/opencv2/ml.hpp",
		opencv_incdir + "/imgproc/include/opencv2/imgproc.hpp",
		opencv_incdir + "/calib3d/include/opencv2/calib3d.hpp",
		opencv_incdir + "/features2d/include/opencv2/features2d.hpp",
		opencv_incdir + "/video/include/opencv2/video/tracking.hpp",
		opencv_incdir + "/video/include/opencv2/video/background_segm.hpp",
		opencv_incdir + "/objdetect/include/opencv2/objdetect.hpp",
		opencv_incdir + "/imgcodecs/include/opencv2/imgcodecs.hpp",
		opencv_incdir + "/videoio/include/opencv2/videoio.hpp",
		opencv_incdir + "/highgui/include/opencv2/highgui.hpp"
	]

	for hname in opencv_hdr_list:
		for decl in parser.parse(hname):
			if len(decl) == 0:
				continue

			vname = decl[0].split(' ')[-1]
			if decl[0].startswith('const '):
				cv_constants[vname] = decl[0]
				continue
			elif decl[0].startswith('class '):
				last_class = vname
				cv_classes[vname] = decl
				continue
			elif decl[0].startswith('struct '):
				last_struct = vname
				cv_structures[vname] = decl
				continue
			elif len(cv_classes) > 0 and decl[0].startswith(last_class):
				# print "VIRT %s %s %s" % (decl, decl[2], '/V' in decl[2])
				# skip methods of classes
				continue
			elif len(cv_structures) > 0 and decl[0].startswith(last_struct):
				# skip methods of classes
				continue

			try:
				function_modules.generate_markus_module_for_function(hname, modules_dir, decl)
				print "Create module '%s' (from %s)" % (decl[0], hname)
				nbOk += 1
			except Exception as e:
				print "Cannot create module '%s' (from %s): %s" % (decl[0], hname, e)
			tot += 1

	print "Could create %d modules out of %d" % (nbOk, tot)
	
	# search all create methods for Feature2D
	method_name = 'create'
	parent = 'cv.Feature2D'
	methods = []
	for hname in opencv_hdr_list:
		for decl in parser.parse(hname):
			vname = decl[0].split(' ')[-1]
			if vname.endswith('.' + method_name):
				if has_parent(vname[:-len('.' + method_name)], parent, cv_classes):
					methods += [decl]
	
	creation_methods.generate_create_method_feature2d(methods)

	# search all create methods for StatModel
	method_name = 'create'
	parent = 'cv.ml.StatModel'
	methods = []
	for hname in opencv_hdr_list:
		for decl in parser.parse(hname):
			vname = decl[0].split(' ')[-1]
			if vname.endswith('.' + method_name):
				if has_parent(vname[:-len('.' + method_name)], parent, cv_classes):
					methods += [decl]

	creation_methods.generate_create_method_stat_model(methods)

	# search all create methods for BackgroundSubtraction
	method_name = 'create'
	parent = 'cv.BackgroundSubtractor'
	methods = []
	for hname in opencv_hdr_list:
		for decl in parser.parse(hname):
			vname = decl[0].split(' ')[-1]
			mn = vname.split('.')[-1]
			if mn.startswith(method_name):
				cn = vname[:-len('.' + mn)]
				ret = '' if not decl[4].startswith('Ptr<') else decl[4][4:-1]
				if 'cv.' + ret in cv_classes and has_parent('cv.' + ret, parent, cv_classes):
					methods += [decl]

	creation_methods.generate_create_method_background_subtractor(methods)

