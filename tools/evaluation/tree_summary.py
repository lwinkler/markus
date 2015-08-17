#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Summaries the results of a simulation in a tree view (HTML)

Example:
    To analyse an events file with its ground truth:
        $ python ./tools/evaluation/tree_summary.py [--no-levels-list] video-annotations/categories.xml simulation_20152514_225544

Created 2015-04-10 Loïc Monney
"""

import re
import os
import pickle
import vplib
import argparse
from vplib.HTMLTags import *
from vplib.time import Time
from xml.dom import minidom
from analyse_events import Evaluation, Video, statistics
    
def arguments_parser():
    """ Define the parser and parse arguments """

    # Main parser
    parser = argparse.ArgumentParser(description='Build result tree', version=vplib.__version__)

    # Events file
    parser.add_argument('INPUT_FILE',
                        type=str,
                        help='the file containing the tree')
    
    # Events file
    parser.add_argument('DIRECTORY',
                        type=str,
                        help='the directory containing the evaluation results')
                        
    # Events file
    parser.add_argument('-o',
    			dest='OUTPUT_HTML',
                        default='evaluation-tree.html',
                        type=str,
                        help='the output html file')

    # Disable liste of movies
    parser.add_argument('--no-levels-list',
                        action='store_true',
                        help='don\'t add the list of movies for each levels')
                        
    return parser.parse_args()

def getText(nodelist):
    """ Get the text contained in a XML node """
    
    rc = []
    for node in nodelist.childNodes:
        if node.nodeType == node.TEXT_NODE:
            rc.append(node.data)
    return ''.join(rc)
    
def build_stats(child_videos, videos, images, out):
	""" Compute the stats for the given tree level """
	
	stats = {
		"det": 0,
		"pos" : 0,
		"tp": 0,
		"fp": 0,
		"fn": 0,
		"dup_ev": 0,
		"dup_gt": 0,
		"ambs": 0,
	}
	nb = 0
	duration = Time()	
	
	for cv in child_videos:
		dname = getText(cv)
		if videos.has_key(dname):
			if not args.no_levels_list:
				out <= IMG(src=images[dname], height=60)
				out <= SPAN(dname)
			for s in stats.keys():
				stats[s] += getattr(videos[dname]['evals'], s)
			duration += videos[dname]['stats']['Video duration'][0]
			nb += 1
		else:
			print "Missing results for: %s" % dname
			if not args.no_levels_list:
				out <= SPAN("** " + dname, style='font-style: italic')
		
		if not args.no_levels_list:
			out <= BR()
	
	if nb > 0:
		stats = statistics(Evaluation(**stats), Video(duration=duration))
		for s in stats.keys():
			v, f = stats[s]
			out <= SPAN(s + ": " + (f % v))
			out <= BR()


def gen_html(node, videos, images, body, parents = []):
	""" Generate the HTML report """
	
	for cn in node.childNodes:
		if cn.nodeType == 1:
			if cn.tagName == 'level':
				name = cn.getAttribute('name')
				child_videos = cn.getElementsByTagName('video')
		
				parents.append(name)
				
				div = DIV(style="float:left; border:1px solid black;padding:3px;margin:2px;page-break-inside: avoid;")
				
				# Title
				title = ""
				for p in parents:
					title += " > " + p
				title = title.strip()
				div <= SPAN(title, style="font-weight:bold")
				div <= BR()
				
				# Table
				build_stats(child_videos, videos, images, div)
				body <= div
		
				# Recursive gen
				gen_html(cn, videos, images, body, parents)
		
				parents.pop()

def main():
	""" Main function, do the job """

	# Global scope for args
	global args

	# Parse the arguments
	args = arguments_parser()
    
    	# Load the stats of all listed videos
	xmldoc = minidom.parse(args.INPUT_FILE)
	videos = xmldoc.getElementsByTagName('video')
	videos_stats = dict();
	images = dict();
	for v in videos:
		dname =  getText(v)
		droot = os.path.join(args.DIRECTORY, 'results', dname, 'analysis')
		evals_path = os.path.join(droot, 'evals.pkl')
		stats_path = os.path.join(droot, 'stats.pkl')
		if os.path.isfile(stats_path):
			evals = pickle.load(open(evals_path, 'r'))
			stats = pickle.load(open(stats_path, 'r'))
			videos_stats[dname] = { 'evals': evals, 'stats': stats }
			images[dname] = os.path.join(droot, 'thumbnail.jpg')
	
	# Build stats for each node of the tree
	head = HEAD(STYLE('body { font-size:10pt }'))
	body = BODY()
	content = gen_html(xmldoc.getElementsByTagName('categories')[0], videos_stats, images, body);
	# print str(HTML(body))
	with open(args.OUTPUT_HTML, 'w') as f:
		f.write(str(HTML(head + body)))
	

if __name__ == "__main__":
    main()
