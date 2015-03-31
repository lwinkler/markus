#! /usr/bin/python

import requests
import sys
import os
import json
import time
import glob
import glob

DEBUG=False

def debug(line):
	if DEBUG: print('DEBUG: ' + line)

def error(line):
	print('ERROR: ' + line)

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

def getQuery(url, fields, data=''):
	"""Send a GET query to the server"""
	# data = '{"query":{"bool":{"must":[{"text":{"record.document":"SOME_JOURNAL"}},{"text":{"record.articleTitle":"farmers"}}],"must_not":[],"should":[]}},"from":0,"size":50,"sort":[],"facets":{}}'
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
	# data = '{"query":{"bool":{"must":[{"text":{"record.document":"SOME_JOURNAL"}},{"text":{"record.articleTitle":"farmers"}}],"must_not":[],"should":[]}},"from":0,"size":50,"sort":[],"facets":{}}'
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

def main():
	if len(sys.argv) == 1 : 
		print 'usage: %s host port jbossLog markusLogDir'

	# Args
	port      = sys.argv[2] if len(sys.argv) > 2 else '8080'
	logJboss  = sys.argv[3] if len(sys.argv) > 3 else '/opt/jboss-as/standalone/log/server.log'
	logDir    = sys.argv[4] if len(sys.argv) > 4 else '/tmp'

	url = 'http://%s:%s/videoaid-ws' % (sys.argv[1], port)

	# Query jobs
	[jobs] = getQuery(url + '/job', ['jobs'])

	print "Found %d jobs: %s" % (len(jobs), jobs)
	line('-')

	# For each job
	for job in jobs:

		# Print detail
		value = getQuery(url + '/job/' + job, ['value'])
		print "Description of job:"
		print value

		lineStart1 = lineCount(logJboss)
		expr = logDir + '/markus_*%s/markus.log' % job
		files  = glob.glob(expr)
		if len(files) != 1: error('%d files found as %s' % (len(files), expr))
		markusLog = files[0]
		lineStart2 = lineCount(markusLog)

		# Send command to read status
		print 'Send command Status'
		postQuery(url + '/job/command/' + job, [], {'command':'manager.manager.Status'})

		# Send command to print statistics
		print 'Send command PrintStatistics'
		postQuery(url + '/job/command/' + job, [], {'command':'manager.manager.PrintStatistics'})

		# sleep 1 sec
		time.sleep(5)
		print
		print '# jboss log'

		debug("Tail %s from line %d to %d" % (logJboss, lineStart1, lineCount(logJboss)))
		os.system("tail --lines=+%d %s" % (lineStart1, logJboss))

		print
		print '# markus log' + markusLog
		debug("Tail %s from line %d to %d" % (markusLog, lineStart2, lineCount(markusLog)))
		os.system("tail --lines=+%d %s" % (lineStart2, markusLog))

		line('-')


if __name__ == "__main__":
	main()
