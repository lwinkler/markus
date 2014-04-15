/*----------------------------------------------------------------------------------
*
*    MARKUS : a manager for video analysis modules
* 
*    author : Laurent Winkler <lwinkler888@gmail.com>
* 
* 
*    This file is part of Markus.
*
*    Markus is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Lesser General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Markus is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with Markus.  If not, see <http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------------------*/

#ifndef UTIL_H
#define UTIL_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <log4cxx/logger.h>


/// this file contains some usefull functions and methods

#define POW2(x) (x) * (x)
#define DIST(x, y) sqrt((x) * (x) + (y) * (y))
#define DIFF_REL(f) ((f).value - (f).mean) * ((f).value - (f).mean) / (f).sqVariance
#define CLEAN_DELETE(x) if((x) != NULL){delete((x));(x) = NULL;}


// time stamp: use for all time stamps on frames in [ms]
#define TIME_STAMP unsigned long long
#define TIME_STAMP_INITIAL 0
#define TIME_STAMP_MIN -100000 // for initialization as well

// Logging functions
#define LOG_ERROR LOG4CXX_ERROR
#define LOG_WARN  LOG4CXX_WARN
#define LOG_INFO  LOG4CXX_INFO
#define LOG_DEBUG LOG4CXX_DEBUG
#define LOG_EVENT(logger, descriptor)   LOG4CXX_WARN((logger), "@notif@ EVENT" << " " << descriptor ) // TODO Maybe remove this

#define SYSTEM(x) {std::string cmd; cmd = (x);\
	if(system(cmd.c_str())) {\
	std::stringstream ss;\
	ss<<"Execution of command failed: "<<cmd;\
	throw MkException(ss.str(), LOC);}}

void adjust(const cv::Mat& im_in, cv::Mat& im_out, cv::Mat*& tmp1, cv::Mat*& tmp2);
void adjustSize(const cv::Mat& im_in, cv::Mat& im_out);
void adjustChannels(const cv::Mat& im_in, cv::Mat& im_out);

cv::Scalar colorFromStr(std::string x_str);
cv::Scalar colorFromId(int x_id);

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

const std::string timeStamp();
const std::string msToTimeStamp(TIME_STAMP x_ms);
TIME_STAMP getAbsTimeMs();
std::string jsonify(const std::string& x_name, const std::string& x_value);
std::string jsonify(const std::string& x_name, TIME_STAMP x_value);
void createEmtpyConfigFile(const std::string& x_fileName);


#endif
