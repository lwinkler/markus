#! /usr/bin/python

# TODO: Remove dependency to vplib by using a standart html lib by using beautifulsoup4

import os.path
import requests
import json
import time
import glob
import argparse
import vplib
from vplib.HTMLTags import *
import nagiosplugin
import logging

# Global arguments, will be overwritten at runtime
args = None
_log = logging.getLogger('nagiosplugin')

def arguments_parser():
	""" Define the parser and parse arguments """

	# Main parser
	parser = argparse.ArgumentParser(description='Print the status of analytics jobs running on server to a file',
			version=vplib.__version__)

	# Events file
	# Output file
	parser.add_argument('-o',
			dest='output',
			type=str,
			default='',
			help='html file to generate, contains the details')

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

	# Markus log directory
	parser.add_argument('-V',
			dest='verbose',
			default=0,
			type=int,
			help='verbosity level of check (from 0 to 3)')

	# Nb of jobs in error state before raising a warning
	parser.add_argument('-w',
			dest='warning',
			default=0,
			type=int,
			help='Nb of jobs in error state before raising a warning')

	# Nb of jobs in error state before raising a critical error
	parser.add_argument('-c',
			dest='critical',
			default=2,
			type=int,
			help='Nb of jobs in error state before raising a critical error')


	return parser.parse_args()

def check(resp):
	if resp['code'] != 200: 
		_log.error("Server did not return code 200")
		_log.error(resp)
	elif resp['message'] != 'Success': 
		_log.error("Server did not return \"Success\"")
		_log.error(resp)

def lineCount(fname):
	return sum(1 for line in open(fname))

def appendLogFromLine(fname, nb_line):
	i=0
	logBuffer=[]
	with open(fname, "r") as f:
		for line in f:
			if i >= nb_line:
				logBuffer.append(line)
			i+=1

	_log.debug("append %d lines from %s" % (len(logBuffer), fname))
	return logBuffer


def getQuery(url, fields, data=''):
	"""Send a GET query to the server"""
	_log.debug("query: " + url)
	response = requests.get(url, data=data)

	_log.debug("response:" + response.text)
	resp = json.loads(response.text)

	check(resp)

	ret = []
	for field in fields:
		ret.append(resp[field])

	return ret

def postQuery(url, fields, params={}):
	"""Send a POST query to the server"""
	_log.debug("query: " + url)
	headers = {u'content-type': u'application/x-www-form-urlencoded'}
	response = requests.post(url, headers=headers, data=params)

	_log.debug("response:" + response.text)
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

def checkStatus(jobDetails, rules):
	""" Check that the result of the job is valid, given a set of rules """

	errors = []

	for rule in rules:
		if rule['contains']:
			valid = False
			for detail in jobDetails[rule['target']]:
				if rule['text'] in detail:
					valid = True
					break
		else:
			valid = True
			for detail in jobDetails[rule['target']]:
				if rule['text'] in detail:
					valid = False
					break
		if not valid:
			errors.append("%s %s '%s'" % (rule['target'], 'does not contain' if rule['contains'] else 'contains', rule['text']))


	return errors

def generateReport(jobDetails, filename):
	if filename == '':
		return
	# generate report
	with open(filename, 'wt') as out:

		# Create HEAD and BODY
		head = HEAD(TITLE('Analytics server report'))
		# head <= SCRIPT(src='http://code.jquery.com/jquery-1.11.0.min.js')
		body = BODY(H1('Analytics server report'))

		# generate summary table
		body <= H2("Job summary table")
		body <= generateSummary(jobDetails, ['hash', 'cameraId', 'status', 'algorithmName', 'descr'])

		# generate html containing logs
		body <= H2("Details of each job")
		body <= generateLogs(jobDetails, ['log1', 'log2'])

		out.write(str(HTML(head + body)))

		# json.dump(job, out, sort_keys=True, indent=4, separators=(',', ': '))

class VerifyJobs(nagiosplugin.Resource):
	""" Nagios class that checks the state of the jobs, based on: http://pythonhosted.org/nagiosplugin/tutorial/check_load.html"""

	def __init__(self, args, rules, jobDetails):
		self.args = args
		self.rules = rules
		self.jobDetails = jobDetails

	# --------------------------------------------------------------------------------
	def probe(self):
		""" Send different commands to the running jobs """

		# Gather and append logs from JBoss and Markus
		if len(self.jobDetails) == 0:
			_log.warning("No jobs found")
		nb_errors = 0
		nb_jobs_errors = 0
		for job in self.jobDetails:
			job['log1'] = appendLogFromLine(job['logFile1'], job['logFile1Start'])
			job['log2'] = appendLogFromLine(job['logFile2'], job['logFile2Start'])
			errors = checkStatus(job, self.rules)
			_log.debug("Found %d error(s) in job %s" %(len(errors), job['hash']))
			job['status'] = 'ok' if len(errors)==0 else "errors: " + "<br/>".join(errors)
			job['errors'] = errors
			if len(errors) > 0: 
				nb_errors += len(errors)
				nb_jobs_errors += 1
				_log.debug("Found errors with job %s: %s" % (job['hash'], str(errors)))

		return [
			nagiosplugin.Metric('jobs', value=len(self.jobDetails)),
			nagiosplugin.Metric('job_errors', value=nb_jobs_errors),
			nagiosplugin.Metric('errors', value=nb_errors)]


class VerifySummary(nagiosplugin.Summary):
	"""Present result of verification"""

	def __init__(self, rules, jobDetails, output):
		self.rules = rules
		self.jobDetails = jobDetails
		self.output = output

	def ok(self, results):
		generateReport(self.jobDetails, self.output)
		return "No errors found in %d jobs" % len(self.jobDetails)

	def problem(self, results):
		log = ""
		
		for detail in self.jobDetails:
			if len(detail['errors']):
				log += "Job %s has %d error(s) (%s)\n" % (detail['hash'], len(detail['errors']), ', '.join(detail['errors']))
		# log += "rules:\n" + str(self.rules)
		generateReport(self.jobDetails, self.output)
		return log

def main():

	# read args
	args = arguments_parser()

	url = 'http://%s:%s/videoaid-ws' % (args.hostname, args.port)

	[jobs] = getQuery(url + '/job', ['jobs'])
	jobDetails = []

	_log.info("Found %d jobs: %s" % (len(jobs), jobs))

	# For each job
	for job in jobs:
		# Print detail
		[value] = getQuery(url + '/job/' + job, ['data'])

		lineStart1 = lineCount(args.logJBoss)
		expr = args.logDir + '/markus_*%s/markus.log' % job
		files  = glob.glob(expr)
		if len(files) != 1:
			_log.warning('%d files found as %s: %s' % (len(files), expr, str(files)))
			files = sorted(files, key=lambda x: os.path.getmtime(x))
			markusLog = files[len(files)-1]
			_log.warning("Using file %s " % markusLog)
		else:
			markusLog = files[0]
		lineStart2 = lineCount(markusLog)

		# Send command to read status
		_log.debug('Send command Status')
		postQuery(url + '/job/command/' + job, [], {'command':'manager.manager.Status'})

		# Send command to print statistics
		_log.debug('Send command PrintStatistics')
		postQuery(url + '/job/command/' + job, [], {'command':'manager.manager.PrintStatistics'})

		_log.debug(json.dumps(value, sort_keys=True, indent=4, separators=(',', ': ')))

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
		_log.info(json.dumps(value, sort_keys=True, indent=4, separators=(',', ': ')))


	# sleep a few seconds to let the commands run
	time.sleep(args.delay)

	# A set of rules for the output of job description
	rules = [
		{'name': 'statusCode1', 'target': 'log1', 'contains': True, 'text': '"code":1000', 
			'descr': 'JBoss must receive the status code 1000. Another value indicates that an exception was caught'},
		{'name': 'statusCode2', 'target': 'log2', 'contains': True, 'text': '"code":1000',
			'descr': 'Markus process must receive the status code 1000. Another value indicates that an exception was caught'},
		{'name': 'cmdSent1', 'target': 'log2', 'contains': True, 'text': 'Command manager.manager.Status returned value', 
			'descr': 'Markus process must receive and execute command "Status"'},
		{'name': 'cmdSent2', 'target': 'log2', 'contains': True, 'text': 'Command manager.manager.PrintStatistics returned value', 
			'descr': 'Markus process must receive and execute command "PrintStatistics"'},
		{'name': 'processFrames', 'target': 'log2', 'contains': False, 'text': 'Manager: 0 frames processed', 
			'descr': 'Markus process must have processed frames since last restart'}
	]

		
	# Call check and explanation routines of nagios
	check = nagiosplugin.Check(
			VerifyJobs(args, rules, jobDetails),
			nagiosplugin.ScalarContext('jobs', fmt_metric="{value} jobs found"),
			nagiosplugin.ScalarContext('errors', fmt_metric="{value} total errors in jobs"),
			nagiosplugin.ScalarContext('job_errors', '0:%d' % args.warning, '0:%d' % args.critical, fmt_metric="{value} jobs raised errors"), # Levels for warning and critical
			VerifySummary(rules, jobDetails, args.output))
	check.main(verbose=args.verbose)




if __name__ == "__main__":
	main()
