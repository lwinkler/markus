#! /usr/bin/python

import requests
import sys
import json
import time
import glob

import argparse
import vplib
from vplib.HTMLTags import *

# Global arguments, will be overwritten at runtime
args = None
DEBUG = True

def arguments_parser():
	""" Define the parser and parse arguments """

	# Main parser
	parser = argparse.ArgumentParser(description='Print the status of analytics jobs running on server to a file',
			version=vplib.__version__)

	# Events file
	# Output file
	parser.add_argument('output',
			type=str,
			default='output.html',
			help='html file to generate')

	# Video file
	parser.add_argument('-H',
			dest='hostname',
			type=str,
			default='localhost',
			help='hostname of the server')

	# Delay
	parser.add_argument('-p',
			dest='port',
			default=8080,
			type=int,
			help='port associated with the hostname')

	# Delay
	parser.add_argument('-d',
			dest='delay',
			default=5,
			type=int,
			help='delay in seconds to let the command complete')

	# Debug: not working
	# parser.add_argument('-D',
			# dest='debug',
			# default=0,
			# type=int,
			# help='print the debug log')

	# JBoss logs
	parser.add_argument('-J',
			dest='logJBoss',
			default='/opt/jboss-as/standalone/log/server.log',
			type=str,
			help='log file of JBoss')

	# Markus log directory
	parser.add_argument('-M',
			dest='logDir',
			default='/tmp',
			type=str,
			help='directory for the log files of Markus')

	return parser.parse_args()

def debug(line):
	if DEBUG: print('DEBUG: ' + line)

def error(line):
	print('ERROR: ' + line)

def warning(line):
	print('WARN: ' + line)

def line(char):
	print char * 80

def check(resp):
	if resp['code'] != 200: 
		error("Server did not return code 200")
		print resp
	elif resp['message'] != 'Success': 
		error("Server did not return \"Success\"")
		print resp

def lineCount(fname):
	i=0
	with open(fname) as f:
		for i, l in enumerate(f):
			pass
	return i + 1

def appendLogFromLine(fname, nb_line):
	i=0
	logBuffer=[]
	with open(fname, "r") as f:
		for line in f:
			if i >= nb_line:
				logBuffer.append(line)
			i+=1

	debug("append %d lines from %s" % (len(logBuffer), fname))
	return logBuffer


def getQuery(url, fields, data=''):
	"""Send a GET query to the server"""
	debug("query: " + url)
	response = requests.get(url, data=data)

	debug("response:" + response.text)
	resp = json.loads(response.text)

	check(resp)

	ret = []
	for field in fields:
		ret.append(resp[field])

	return ret

def postQuery(url, fields, params={}):
	"""Send a POST query to the server"""
	debug("query: " + url)
	headers = {u'content-type': u'application/x-www-form-urlencoded'}
	response = requests.post(url, headers=headers, data=params)

	debug("response:" + response.text)
	resp = json.loads(response.text)

	check(resp)

	ret = []
	for field in fields:
		ret.append(resp[field])

	return ret

def generateSummary(jobDetails, fields):
	""" Generate a table containing the summary for each job """

	table = TABLE(border=1, style='border-collapse: collapse;')

	# gen header
	row = TR()
	for field in fields:
		row <= TH(field)
	table <= row

	# For each job: read the generated logs
	for job in jobDetails:
		row = TR()
		for field in fields:
			row <= TD(job[field])
		table <= row

	return table

def generateLogs(jobDetails, fields):
	""" Generate html code containing the logs of each jobs """

	content = DIV()

	# For each job: display logs
	for job in jobDetails:
		content <= H3(job['hash'])
		for field in fields:
			content <= H4(field)
			content <= DIV("<br/>".join(job[field]) + "<br/>")
	return content



def main():
	args = arguments_parser()

	url = 'http://%s:%s/videoaid-ws' % (args.hostname, args.port)

	# Query jobs
	[jobs] = getQuery(url + '/job', ['jobs'])

	print "Found %d jobs: %s" % (len(jobs), jobs)
	line('=')

	jobDetails=[]

	# For each job
	for job in jobs:
		# Print detail
		[value] = getQuery(url + '/job/' + job, ['value'])

		lineStart1 = lineCount(args.logJBoss)
		expr = args.logDir + '/markus_*%s/markus.log' % job
		files  = glob.glob(expr)
		if len(files) != 1:
			warning('%d files found as %s: %s' % (len(files), expr, str(files)))
			markusLog = sorted(files)[len(files)-1]
			warning("Using file %s " % markusLog)
		else:
			markusLog = files[0]
		lineStart2 = lineCount(markusLog)

		# Send command to read status
		debug('Send command Status')
		postQuery(url + '/job/command/' + job, [], {'command':'manager.manager.Status'})

		# Send command to print statistics
		debug('Send command PrintStatistics')
		postQuery(url + '/job/command/' + job, [], {'command':'manager.manager.PrintStatistics'})

 		debug(json.dumps(value, sort_keys=True, indent=4, separators=(',', ': ')))

		jobDetails.append({
			'hash' : job,
			'cameraId': value[u'cameraId'],
			'algorithmParams': value[u'algorithmParams'],
			'algorithmName': value[u'algorithmName'],
			'descr': json.dumps(value, sort_keys=True, indent=4, separators=(',', ': ')),
			'logFile1':   args.logJBoss,
			'logFile1Start': lineStart1,
			'logFile2':   markusLog,
			'logFile2Start': lineStart2
		})

	# sleep a few seconds to let the commands run
	time.sleep(args.delay)
	
	# Gather and append logs from JBoss and Markus
	for job in jobDetails:
		job['log1'] = appendLogFromLine(job['logFile1'], job['logFile1Start'])
		job['log2'] = appendLogFromLine(job['logFile2'], job['logFile2Start'])

	debug("generate report %s" % args.output)
	# generate the report
	with open(args.output, 'wt') as out:

		# Create HEAD and BODY
		head = HEAD(TITLE('Analytics server report'))
		# head <= SCRIPT(src='http://code.jquery.com/jquery-1.11.0.min.js')
		body = BODY(H1('Analytics server report'))

		# generate summary table
		body <= H2("Job summary table")
		body <= generateSummary(jobDetails, ['hash', 'cameraId', 'algorithmName', 'descr'])

		# generate html containing logs
		body <= H2("Details of each job")
		body <= generateLogs(jobDetails, ['log1', 'log2'])

		out.write(str(HTML(head + body)))

		# json.dump(job, out, sort_keys=True, indent=4, separators=(',', ': '))

if __name__ == "__main__":
	main()
