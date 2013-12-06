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

// time stamp: use for all time stamps on frames in [ms]
#define TIME_STAMP unsigned long long
#define TIME_STAMP_INITIAL 0
#define TIME_STAMP_MIN -100000 // for initialization as well

// Logging functions
#define LOG_ERROR LOG4CXX_ERROR
#define LOG_WARN  LOG4CXX_WARN
#define LOG_INFO  LOG4CXX_INFO
#define LOG_DEBUG LOG4CXX_DEBUG
#define LOG_EVENT(logger, message)   LOG4CXX_WARN((logger), "EVENT " << message) // Global::logger.Stream(LOG_EVENT)<<x<<std::endl // TODO: define this

#define SYSTEM(x) {std::string cmd; cmd = (x) ; if(system(cmd.c_str())) throw MkException("Execution of command failed: " + cmd, LOC);}

/// this file contains some usefull functions and methods

void adjust(const cv::Mat* im_in, cv::Mat* im_out, cv::Mat*& tmp1, cv::Mat*& tmp2);//, Mat* tmp2);
void adjustSize(const cv::Mat* im_in, cv::Mat* im_out);
void adjustChannels(const cv::Mat* im_in, cv::Mat* im_out);

cv::Scalar colorFromStr(std::string x_str);
cv::Scalar colorFromId(int x_id);

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

const std::string timeStamp();
const std::string msToTimeStamp(TIME_STAMP x_ms);


class Global
{
	public:
		static log4cxx::LoggerPtr logger;
		static const std::string& OutputDir();
		static inline void SetConfigFile(const std::string& x_configFile){
			assert(m_configFile.size() == 0);
			m_configFile = x_configFile;
		}
		static void Infos();
		static inline const std::string& GetConfigFile(){return m_configFile;}
		static void Finalize();
	private:
		static std::string m_configFile;
		static std::string m_outputDir;
};

#endif
