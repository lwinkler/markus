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
#define LOG_EVENT(logger, message)   LOG4CXX_WARN(logger, "EVENT" << message) // Global::logger.Stream(LOG_EVENT)<<x<<std::endl // TODO: define this
// #define LOG_ERROR(x)   LOG4CXX_ERROR(m_logger, x) // Global::logger.Stream(LOG_ERROR)<<x<<std::endl
// #define LOG_WARN(x)    LOG4CXX_WARN(m_logger, x) // Global::logger.Stream(LOG_WARN)<<x<<std::endl
// #define LOG_INFO(x)    LOG4CXX_INFO(m_logger, x) // Global::logger.Stream(LOG_INFO)<<x<<std::endl
// #define LOG_DEBUG(x)   LOG4CXX_DEBUG(m_logger, x) // if(Global::logger.HasDebugMode()){Global::logger.Stream(LOG_DEBUG)<<x<<std::endl;}

/// this file contains some usefull functions and methods

void adjust(const cv::Mat* im_in, cv::Mat* im_out, cv::Mat*& tmp1, cv::Mat*& tmp2);//, Mat* tmp2);
void adjustSize(const cv::Mat* im_in, cv::Mat* im_out);
void adjustChannels(const cv::Mat* im_in, cv::Mat* im_out);

cv::Scalar colorFromStr(std::string x_str);
cv::Scalar colorFromId(int x_id);

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

const std::string timeStamp();
const std::string msToTimeStamp(TIME_STAMP x_ms);



/*typedef enum
{
	LOG_EVENT,
	LOG_ERROR,
	LOG_WARN,
	LOG_INFO,
	LOG_DEBUG,
	LOG_SIZE
} logLevel;

class Logging
{
	public:
		Logging();
		~Logging();
		// char m_mode;
		std::ostream & Stream(logLevel x_level);
		void SetMode(char x_mode);
		inline bool HasDebugMode() const {return m_oss[LOG_DEBUG] != m_cnull;}

	private:
		std::ofstream m_logFile;
		std::ofstream m_cnull;
		std::vector<std::ostream*> m_oss;
		char m_mode;
};*/

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
	private:
		static std::string m_configFile;
		static std::string m_outputDir;
};

#endif
