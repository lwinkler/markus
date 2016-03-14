#!/usr/bin/env python


import os, sys, re, string
from hdr_parser import *

std_types = ['bool', 'float', 'double', 'int', 'long', 'string']

def translateType(decl):
	if decl[0] in std_types:
		return decl[0]
	if decl[0] in ['Mat', 'InputArray']:
		return 'cv::Mat'
	raise ValueError("Cannot translate type '%s'" % decl[0])

def translateTypeToInit(decl):
	type1 = translateType(decl)
	if type1 in std_types:
		return ''
	if type1 == "cv::Mat":
		return "m_%s(cv::Size(m_param.width, m_param.height), m_param.type)" % decl[1]
	raise ValueError("Cannot translate type '%s' to init" % type1)

def translateTypeToStream(decl):
	type1 = translateType(decl)
	if type1 in std_types:
		return "new StreamT<%s>(\"%s\", m_%s, *this, \"\")" % (type1, decl[1], decl[1])
	if type1 == "cv::Mat":
		return "new StreamImage(\"%s\", m_%s, *this, \"\")" % (decl[1], decl[1])
	raise ValueError("Cannot translate type '%s' to stream" % type1)


def create_substitutions(decl):
	""" Create the array or substitutions for the parser """

	init = ""
	for param in decl[3]:
		s = translateTypeToInit(param)
		if len(s) > 0:
			init += '\n\t, ' + s

	members = "\n\t".join(["%s m_%s;" % (translateType(param), param[1]) for  param in decl[3]])
	fct = "cv::%s(%s);" % (decl[0].split('.')[-1], ', '.join(["m_%s" % param[1] for param in decl[3]]))

	streams  = ""
	inputNb  = 0
	outputNb = 0

	for param in decl[3]:
		if '/O' not in param[3]:
			streams += "\n\tAddInputStream(%d, %s);" % (inputNb, translateTypeToStream(param))
			inputNb += 1
		if '/O' in param[3] or '/IO' in param[3]:
			streams += "\n\tAddOutputStream(%d, %s);" % (outputNb, translateTypeToStream(param))
			outputNb += 1

	return {
		'moduleName': decl[0].split('.')[-1],
		'init':       init,
		'members':    members,
		'function':   fct,
		'streams':    streams
	}

def create_file(file_name, template_file_name, substitutions):
	with open(file_name, 'w') as fout:
		with open(template_file_name, 'r') as templateFile:
			src = string.Template(templateFile.read())
			fout.write(src.substitute(substitutions))

def create_markus_module(hname, decl):

	module_name = decl[0].split('.')[-1]
	module_dir  = "modules_gen/" + module_name

	d = create_substitutions(decl)
	os.mkdir(module_dir)
	create_file(module_dir + '/' + module_name + '.h',   'tools/opencvParser/Module.template.h'  , d)
	create_file(module_dir + '/' + module_name + '.cpp', 'tools/opencvParser/Module.template.cpp', d)

if __name__ == '__main__':

	cv_constants  = []
	cv_classes    = []
	cv_structures = []

	parser = CppHeaderParser()
	decls = []
	for hname in opencv_hdr_list:
		for decl in parser.parse(hname):
			if len(decl) == 0:
				continue

			sp = decl[0].split(' ')
			if sp[0] == 'const':
				cv_constants.append(sp[1])
				continue
			elif sp[0] == 'class':
				cv_classes.append(sp[1])
				continue
			elif sp[0] == 'struct':
				cv_structures.append(sp[1])
				continue
			elif len(cv_classes) > 0 and decl[0].startswith(cv_classes[-1]):
				# skip methods of classes
				continue
			elif len(cv_structures) > 0 and decl[0].startswith(cv_structures[-1]):
				# skip methods of classes
				continue

			# print decl
			#if True: 
			if decl[0] == "cv.bitwise_xor":
				try:
					create_markus_module(hname, decl)
					print "Create module '%s' (from %s)" % (decl[0], hname)
				except Exception as e:
					print "Cannot create module '%s' (from %s): %s" % (decl[0], hname, e)



