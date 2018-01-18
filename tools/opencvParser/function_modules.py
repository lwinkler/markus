#!/usr/bin/env python

##
## function_modules.py
## ~~~~~~~~~
##
## Copyright (c) 2017 Laurent Winkler lwinkler888 at gmail dot com
## 
## This file is part of Markus source files

import os, sys, re, string

# Global variables
std_types = ['bool', 'float', 'double', 'int', 'long'] # , 'std::string']
forbidden = ['param', 'width', 'height', 'type', 'class', 'cached', 'master', 'fps']

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

def generate_markus_module_for_function(hname, modules_dir, decl):

	module_name = decl[0].replace('cv.', '').replace('.', '_')
	module_dir  = modules_dir + "/" + module_name

	d = create_substitutions(hname, module_name, decl)
	os.mkdir(module_dir)
	create_file(module_dir + '/' + module_name + '.h',   'tools/opencvParser/Module.template.h'  , d)
	create_file(module_dir + '/' + module_name + '.cpp', 'tools/opencvParser/Module.template.cpp', d)

