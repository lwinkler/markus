#!/usr/bin/env python


import os, sys, re, string
from hdr_parser import *

# Global variables
std_types = ['bool', 'float', 'double', 'int', 'long'] # , 'std::string']
forbidden = ['param', 'width', 'height', 'type', 'class', 'cached', 'master', 'fps', ]

def translate_type(decl):
	""" Translate types to internal types of Markus """
	# if decl[0] == 'string':
		# return 'std::string'
	if decl[0] in std_types:
		return decl[0]
	if decl[0] in ['Mat', 'InputArray']:
		return 'cv::Mat'
	raise ValueError("Cannot translate type '%s'" % decl[0])

def translate_name(decl):
	""" Translate variable names by avoiding reserved names """
	if decl[1] in forbidden:
		return "_%s" % decl[1]
	else:
		return decl[1]

def translate_type_to_init(decl):
	""" Generate the initialization of the member """
	type1 = translate_type(decl)
	name1 = translate_name(decl)
	if type1 in std_types:
		return ''
	if type1 == "cv::Mat":
		return "m_%s(cv::Size(m_param.width, m_param.height), m_param.type)" % name1
	raise ValueError("Cannot translate type '%s' to init" % type1)

def translate_type_to_stream(decl):
	""" Generate the stream of the member """
	type1 = translate_type(decl)
	name1 = translate_name(decl)
	if type1 in std_types:
		return "new StreamNum<%s>(\"%s\", m_%s, *this, \"\")" % (type1, name1, name1)
	if type1 == "cv::Mat":
		return "new StreamImage(\"%s\", m_%s, *this, \"\")" % (name1, name1)
	raise ValueError("Cannot translate type '%s' to stream" % type1)


def create_substitutions(hname, module_name, decl):
	""" Create the array or substitutions for the parser """

	init = ""
	for param in decl[3]:
		s = translate_type_to_init(param)
		if len(s) > 0:
			init += '\n\t, ' + s

	members = "\n\t".join(["%s m_%s;" % (translate_type(param), translate_name(param)) for  param in decl[3]])
	fct = "%s(%s);" % (decl[0].replace('.', '::'), ', '.join(["m_%s" % translate_name(param) for param in decl[3]]))

	streams  = ""

	for param in decl[3]:
		if '/O' not in param[3]:
			streams += "\n\tAddInputStream(%s);" % (translate_type_to_stream(param))
		if '/O' in param[3] or '/IO' in param[3]:
			streams += "\n\tAddOutputStream(%s);" % (translate_type_to_stream(param))

	return {
		'includes':   "#include<%s>" % hname,
		'moduleName': module_name,
		'category':   hname.split('/')[-1].replace('.', '_'),
		'init':       init,
		'members':    members,
		'function':   fct,
		'streams':    streams,
		'description': ''
	}

def create_file(file_name, template_file_name, substitutions):
	with open(template_file_name, 'r') as templateFile:
		src = string.Template(templateFile.read())
	with open(file_name, 'w') as fout:
		fout.write(src.substitute(substitutions))

def create_markus_module_for_function(hname, modules_dir, decl):

	module_name = decl[0].replace('cv.', '').replace('.', '_')
	module_dir  = modules_dir + "/" + module_name

	d = create_substitutions(hname, module_name, decl)
	os.mkdir(module_dir)
	create_file(module_dir + '/' + module_name + '.h',   'tools/opencvParser/Module.template.h'  , d)
	create_file(module_dir + '/' + module_name + '.cpp', 'tools/opencvParser/Module.template.cpp', d)

def has_parent(class_name, parent_name, cv_classes):
	cl = cv_classes[class_name]
	if not cl[1]:
		return False
	if not cl[1].startswith(': '):
		raise Exception('Expected semicolon at the begining of ' + cl[0])
	
	for par in cl[1][2:].split(' '):
		pn = par.replace('::', '.')
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

	if len(sys.argv) != 3:
		print "usage: %s <opencv_includes> <output_directory>" % sys.argv[0]
		exit(0)

	modules_dir   = sys.argv[2]
	opencv_incdir = sys.argv[1]
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
				print decl
				last_class = vname
				cv_classes[vname] = decl
				continue
			elif decl[0].startswith('struct '):
				last_struct = vname
				cv_structures[vname] = decl
				continue
			elif len(cv_classes) > 0 and decl[0].startswith(last_class):
				print "VIRT %s %s %s" % (decl, decl[2], '/V' in decl[2])
				# skip methods of classes
				continue
			elif len(cv_structures) > 0 and decl[0].startswith(last_struct):
				# skip methods of classes
				continue

			try:
				create_markus_module_for_function(hname, modules_dir, decl)
				print "Create module '%s' (from %s)" % (decl[0], hname)
				nbOk += 1
			except Exception as e:
				print "Cannot create module '%s' (from %s): %s" % (decl[0], hname, e)
			tot += 1
	
	# search all methods
	method_name = 'create'
	parent = 'cv.Feature2D'
	for hname in opencv_hdr_list:
		for decl in parser.parse(hname):
			vname = decl[0].split(' ')[-1]
			if vname.endswith('.' + method_name) and vname[:-len('.' + method_name)]:
				print decl


	print "Could create %d modules out of %d" % (nbOk, tot)



