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

#include <opencv2/core/core.hpp>
#include "define.h"

class Event;
class AnnotationFileReader;
class FeaturePtr;

/// this file contains some usefull functions and methods. To be included in .cpp files
void adjust(const cv::Mat& im_in, cv::Mat& im_out, cv::Mat*& tmp1, cv::Mat*& tmp2);
void adjustSizeAndChannels(const cv::Mat& im_in, cv::Mat& im_out, cv::Mat*& tmp1);
void adjustSize(const cv::Mat& im_in, cv::Mat& im_out);
void adjustChannels(const cv::Mat& im_in, cv::Mat& im_out);

cv::Scalar colorFromStr(std::string x_str);
cv::Scalar colorFromId(int x_id);

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::string join(const std::vector<std::string> &elems, char delim, const std::string& format = "%s");

const std::string timeStamp(int x_pid = 0);
const std::string msToTimeStamp(TIME_STAMP x_ms);
TIME_STAMP timeStampToMs(const std::string& x_timeStamp);
TIME_STAMP getAbsTimeMs();
TIME_STAMP timeStampFromFileName(const std::string& x_fileName);
void createEmptyConfigFile(const std::string& x_fileName, bool x_header = true);
bool compareFiles(const std::string& x_file1, const std::string& x_file2);
std::string getFileContents(const std::string& x_filename);
double diagonal(const cv::Mat& x_image);
std::string basename(const std::string& x_pathName);
void printStack(int sig);
void execute(const std::string& x_cmd, std::ostream& xr_stdout);
void execute(const std::string& x_cmd, std::vector<std::string>& xr_result);
bool boolValue(const std::string& x_value);
void addExternalImage(const cv::Mat& x_image, const std::string& x_name, const std::string& x_fileWithPath, Event& xr_event);

template<class T> void mergeVector(std::vector<T>& vdest, const std::vector<T>& vori)
{
	for(const auto& ori : vori)
	{
		// note: There is probably a more efficient of smarter way to do this
		if(std::find(vdest.begin(), vdest.end(), ori) == vdest.end())
			vdest.push_back(ori);
	}
}

AnnotationFileReader* createAnnotationFileReader(const std::string& x_fileName, int x_width, int x_height);
void singleLine(std::string& str);
double convertAspectRatio(const std::string& x_string);
std::string convertAspectRatio(const cv::Size& x_size);
inline double aspectRatio(cv::Size& x_size)
{
	return static_cast<double>(x_size.height) / x_size.width;
}

inline cv::Point_<double> ratio(cv::Size& x_size1, cv::Size& x_size2)
{
	return cv::Point_<double>(
		static_cast<double>(x_size1.width) / x_size2.width,
		static_cast<double>(x_size1.height) / x_size2.height
	);
}

#endif
