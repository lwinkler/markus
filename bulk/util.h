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

#include <opencv/cv.h>
// #include <cstdio>
// #include <cstdlib>
#include <iostream>
#include <fstream>

/// this file contains some usefull functions and methods

void adjust(const cv::Mat* im_in, cv::Mat* im_out, cv::Mat*& tmp1, cv::Mat*& tmp2);//, Mat* tmp2);
void adjustSize(const cv::Mat* im_in, cv::Mat* im_out);
void adjustChannels(const cv::Mat* im_in, cv::Mat* im_out);

void saveMat(const cv::Mat * x_mat, const std::string& x_name);

CvScalar colorFromStr(std::string x_str);
CvScalar colorFromId(int x_id);

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

const std::string timeStamp();


// Logging functions
#define LOG_ERROR(x)   MkLog::log.stream(LOG_ERROR)<<x<<std::endl;
#define LOG_WARNING(x) MkLog::log.stream(LOG_WARNING)<<x<<std::endl;
#define LOG_INFO(x)    MkLog::log.stream(LOG_INFO)<<x<<std::endl;
#define LOG_DEBUG(x)   MkLog::log.stream(LOG_DEBUG)<<x<<std::endl;

typedef enum
{
	LOG_EVENT,
	LOG_ERROR,
	LOG_WARNING,
	LOG_INFO,
	LOG_DEBUG
} logLevel;

class Log {
	public:
		Log();
		~Log();
		// char m_mode;
		std::ostream & stream(logLevel x_level);
		inline void ShowDebug(bool x_show) {m_showDebug = x_show;}

	private:
		std::ofstream m_logFile;
		std::ofstream m_cnull;
		bool m_showDebug;
};

class MkLog{
	public:
		static Log log;	
};

#endif
