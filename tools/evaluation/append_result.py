#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Append data from a html table to another. For plotting purposes.

Created Jan 2015 by Laurent Winkler
"""
import os
import re
import vplib
import argparse
from bs4 import BeautifulSoup

# Global arguments, will be overwritten at runtime
args = None

def arguments_parser():
	""" Define the parser and parse arguments """

	# Main parser
	parser = argparse.ArgumentParser(description= 'Read HTML table and append to another', version=vplib.__version__)

	# Events file
	parser.add_argument('INPUT_FILE',
						type=str,
						help='The input html file')

	# Ground truth file
	parser.add_argument('OUTPUT_FILE',
						type=str,
						help='The existing html file to modify')

	# Clear table
	parser.add_argument('--clear',
						action="store_true",
						help='Clear the table before inserting')

	# output to tmp.html
	parser.add_argument('--temp',
						action="store_true",
						help='Write output to tmp.html instead of overriding')

	# benchmark file, for fps data
	parser.add_argument('-b',
						dest='bench',
						default='',
						type=str,
						help='XML file for computation speed information (FPS). Usually named benchmark.xml')

	# Title to use for the entry in table
	parser.add_argument('-t',
						dest='title',
						default='',
						type=str,
						help='Title to use for the entry in table')

	# Id of the output table
	parser.add_argument('-d',
						dest='output_table',
						default='datatable',
						type=str,
						help='The html id of the table to be modified in the output html file')



	return parser.parse_args()

def retrieve_fps():
	f = open(args.bench, 'r')
	soup = BeautifulSoup(f)
	return soup.find('benchmark').find('manager').find('fps').get_text()

def multiple_replace(dict, text):
	# Create a regular expression  from the dictionary keys
	regex = re.compile("(%s)" % "|".join(map(re.escape, dict.keys())))

	# For each match, look-up corresponding value in dictionary
	return regex.sub(lambda mo: dict[mo.string[mo.start():mo.end()]], text) 

# Write our own prettify function: This can be improved
def prettify(self, encoding=None, formatter="minimal", indent_width=8):
	dict = {
		"<td>\n"  : "<td>",
		"</td>\n" : "</td>",
		"<tr>\n"  : "<tr>"
	} 
	# r = re.compile(r'^(\s*)', re.MULTILINE)
	# return re.sub('        ', '\t', r.sub(r'\1' * indent_width, self.prettify(encoding, formatter)))
	return multiple_replace(dict, self.prettify(encoding, formatter))

def main():
	"""Main function"""

	# Global scope for args
	global args

	# Parse the arguments
	args = arguments_parser()

	htmlf1 = open(args.INPUT_FILE,  'r')
	htmlf2 = open(args.OUTPUT_FILE, 'r')
	soup1 = BeautifulSoup(htmlf1)
	soup2 = BeautifulSoup(htmlf2)

	# Find table in input file
	tables = soup1.find("table", {'id': 'result_table'})

	# Values to extract
	title  = args.INPUT_FILE.replace(".html", "")
	entry1 = ""
	entry2 = ""
	entry3 = ""

	if args.title != "":
		title = args.title

	if args.clear:
		soup2.find("table", {"id": args.output_table}).find("tbody").string = ""
		print "Clear table"
	else:

		# FPS info
		if args.bench != "":
			entry3 = retrieve_fps()

		# Parse input file
		for row in tables.findAll("tr"):
			try:
				# Find name as the <b> tag
				name = row.findAll("td")[0].findAll("b")[0].get_text()
				# print name
				cells = row.findAll("td")
				# Find value as the content of the second cell (total)
				value = cells[1].get_text()

				if name == "Detected":
					entry1 = value.replace('%', '').strip()
					# N/A means no alarm should be detected
					if entry1 == 'N/A': 
						entry1 = "100"
				if name == "False alarm rate":
					entry2 = value.replace('alarms/hour', '').strip()

			except IndexError as e:
				print "skip row: " + str(e)
		
		print "Insert values in table: %s, %s, %s, title:%s" % (entry1, entry2, entry3, title)
		
		new_tr = soup2.new_tag("tr")

		new_td = soup2.new_tag("td")
		new_td.string = title
		new_tr.append(new_td)
		new_td = soup2.new_tag("td")
		new_td.string = entry1
		new_tr.append(new_td)
		new_td = soup2.new_tag("td")
		new_td.string = entry2
		new_tr.append(new_td)
		new_td = soup2.new_tag("td")
		new_td.string = entry3
		new_tr.append(new_td)

		soup2.find("table", {"id": args.output_table}).find("tbody").append(new_tr)
	
	htmlf1.close()
	htmlf2.close()

	if args.temp:
		with open("tmp.html", "wb") as file:
			file.write(prettify(soup2))
			# file.write(soup2.prettify(None, "minimal"))
	else:
		with open(args.OUTPUT_FILE, "wb") as file:
			file.write(prettify(soup2))
			# file.write(soup2.prettify(None, "minimal"))

if __name__ == '__main__':
	main()
