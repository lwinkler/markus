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

#include "util.h"
#include "Manager.h"
#include "Event.h"
#include "MkException.h"
#include "AnnotationSrtFileReader.h"
#include "AnnotationAssFileReader.h"
#include <fstream>
#include <cstdio>
#include <opencv2/opencv.hpp>
#include <execinfo.h>
#include <sys/time.h>

using namespace std;
using namespace cv;

// Global variables
int g_colorArraySize = 54;
Scalar g_colorArray[] =
{
	CV_RGB(128,128,128), // gray//CV_RGB(255,255,255), // white
	CV_RGB(255,0,0), // red
	CV_RGB(0,255,0), // green
	CV_RGB(0,0,255), // blue
	CV_RGB(255,255,0), // yellow
	CV_RGB(0,150,255), // blue
	CV_RGB(130,77,191), // purple
	CV_RGB(255,100,0), // yellow
	CV_RGB(185,135,95), // brown
	CV_RGB(160, 32, 240),
	CV_RGB(255, 165, 0),
	CV_RGB(132, 112, 255),
	CV_RGB(0, 250, 154),
	CV_RGB(255, 192, 203),
	CV_RGB(0, 255, 255),
	CV_RGB(185, 185, 185),
	CV_RGB(216, 191, 216),
	CV_RGB(255, 105, 180),
	CV_RGB(0, 255, 255),
	CV_RGB(240, 255, 240),
	CV_RGB(173, 216, 230),
	CV_RGB(127, 255, 212),
	CV_RGB(100, 149, 237),
	CV_RGB(255, 165, 0),
	CV_RGB(255, 255, 0),
	CV_RGB(210, 180, 140),
	CV_RGB(211, 211, 211),
	CV_RGB(222, 184, 135),
	CV_RGB(205, 133, 63),
	CV_RGB(139, 69, 19),
	CV_RGB(165, 42, 42),
	CV_RGB(84, 134, 11),
	CV_RGB(210, 105, 30),
	CV_RGB(255, 127, 80),
	CV_RGB(255, 0, 255),
	CV_RGB(70, 130, 180),
	CV_RGB(95, 158, 160),
	CV_RGB(199, 21, 133),
	CV_RGB(255, 182, 193),
	CV_RGB(255, 140, 0),
	CV_RGB(240, 255, 255),
	CV_RGB(152, 251, 152),
	CV_RGB(143, 188, 143),
	CV_RGB(240, 230, 140),
	CV_RGB(240, 128, 128),
	CV_RGB(0, 191, 255),
	CV_RGB(250, 128, 114),
	CV_RGB(189, 183, 107),
	CV_RGB(255, 218, 185),
	CV_RGB(60, 179, 113),
	CV_RGB(178, 34, 34),
	CV_RGB(30, 144, 255),
	CV_RGB(255, 140, 0),
	CV_RGB(175, 238, 238)
};

/// Handler for errors and exceptions: print full stack without symbols
/*
void printStack(int sig)
{
	static const int max_frames = 100;
	void* addrlist[max_frames+1];
	stringstream ss;

	// print out all the frames to stderr
	ss << "Error: signal " << sig <<endl;

	// retrieve current stack addresses
	size_t addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));
	FILE* pf = fopen("markus.error", "w");
	int  fd = fileno(pf);

	// Print stack trace (without symbols) to file and stderr
	backtrace_symbols_fd(addrlist, addrlen, fd);
	backtrace_symbols_fd(addrlist, addrlen, STDERR_FILENO);
	fclose(pf);
	exit(1);
}
*/




/* Set image to the right size */

void adjustSize(const Mat& im_in, Mat& im_out)
{
	if(!im_in.cols || !im_in.rows)
	{
		throw MkException("Module input image has width or height with zero value", LOC);
	}
	else
	{
		if(im_in.cols == im_out.cols && im_in.rows == im_out.rows)
		{
			im_in.copyTo(im_out);
		}
		else if(im_in.cols >= im_out.cols)
		{
			// note: Maybe one day, parametrize the interpolation method
			resize(im_in, im_out, im_out.size(), 0, 0, CV_INTER_AREA);
		}
		else
		{
			resize(im_in, im_out, im_out.size(), 0, 0, CV_INTER_LINEAR);
		}
	}
}

/* Set image to the right depth
 WARNING :: buffers tmp1 and tmp2 are only generated once so this method must be used only for one type of images  */

void adjust(const Mat& im_in, Mat& im_out, Mat*& tmp1, Mat*& tmp2)
{

	if(im_in.depth() == im_out.depth())
	{
		adjustSizeAndChannels(im_in, im_out, tmp1);
	}
	else
	{
		if(tmp1==nullptr)
		{
			//cout<<"create image in adjust IPL_DEPTH_32F tmp1"<<endl;
			tmp1 = new Mat( Size(im_out.cols, im_out.rows), im_in.type());
		}
		if(tmp2==nullptr)
		{
			//cout<<"create image in adjust IPL_DEPTH_32F tmp2"<<endl;
			tmp2 = new Mat( Size(im_out.cols, im_out.rows), CV_MAKE_TYPE(im_in.depth(), im_out.channels()));
		}

		if(im_in.depth() == CV_8U && im_out.depth() == CV_32F)
		{
			adjustSizeAndChannels(im_in, *tmp2, tmp1);
			//convertByte2Float(tmp2, im_out);
			tmp2->convertTo(im_out, im_out.type(), 1.0 / 255);
		}
		else if(im_in.depth() == CV_32F && im_out.depth() == CV_8U)
		{
			adjustSizeAndChannels(im_in, *tmp2, tmp1);
			//convertFloat2Byte(tmp2, im_out);
			tmp2->convertTo(im_out, im_out.type(), 255);
		}
		else
		{
			throw MkException("Cannot convert format", LOC);
		}
	}
}

/* Set image to the right size and channel numbers */

void adjustSizeAndChannels(const Mat& im_in, Mat& im_out, Mat*& tmp1)
{
	if(im_in.channels() == im_out.channels())
	{
		adjustSize(im_in, im_out);
		return;
	}
#if 0
	if(tmp1==nullptr)
	{
		//cout<<"create image in adjust tmp1 depth "<<im_out->depth<<endl;
		tmp1 = new Mat( Size(im_in.cols, im_in.rows), im_out.type());
	}
	adjustChannels(im_in, *tmp1);
	adjustSize(*tmp1, im_out);
#else
	// note: this version seems faster
	if(tmp1==nullptr)
	{
		//cout<<"create image in adjust tmp1 depth "<<im_out->depth<<endl;
		tmp1 = new Mat( Size(im_out.cols, im_out.rows), im_out.type());
	}
	adjustSize(im_in, *tmp1);
	adjustChannels(*tmp1, im_out);
#endif
}

/* Set the image to the right number of channels */

void adjustChannels(const Mat& im_in, Mat& im_out)
{
	if(im_in.channels() == im_out.channels())
	{
		im_in.copyTo(im_out);
	}
	else if(im_in.channels() == 1 && im_out.channels() == 3)
	{
		cvtColor(im_in, im_out, CV_GRAY2BGR);
	}
	else if(im_in.channels() == 3 && im_out.channels() == 1)
	{
		cvtColor(im_in, im_out, CV_BGR2GRAY);
	}
	else throw MkException("Error in adjustChannels", LOC);
}

/* Return an OpenCV color from a "(b,g,r)" string */
Scalar colorFromStr(string x_str)
{
	if(x_str[0] == '(')
	{
		int pos1  = x_str.find(',', 1);
		int pos2  = x_str.find(',', pos1 + 1);
		int pos3  = x_str.find(')', pos2 + 1);
		if(!(pos1 && pos2 && pos3))
			throw MkException("Error in colorFromStr", LOC);

		return Scalar(atoi(x_str.substr(1, pos1 - 1).c_str()),
					  atoi(x_str.substr(pos1 + 1, pos2 - pos1 - 1).c_str()),
					  atoi(x_str.substr(pos2 + 1, pos3 - pos2 - 1).c_str()));
	}
	else return Scalar(0, 0, 0);
}

Scalar colorFromId(int x_id)
{
	return g_colorArray[x_id % g_colorArraySize];
}


/// Split a string separated by a character
vector<string> &split(const string &s, char delim, vector<string> &elems)
{
	stringstream ss(s);
	string item;
	elems.clear();
	while (getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems; // Return an input parameter so the reference is valid !
}

/// join strings
string join(const vector<string> &elems, char delim, const string& format)
{
	if(elems.size() == 0)
		return "";
	stringstream ss;
	auto it = elems.begin();
	char buffer[256];
	for( ; it != elems.end() - 1 ; it++)
	{
		snprintf(buffer, sizeof(buffer), format.c_str(), it->c_str());
		ss << buffer << delim;
	}
	snprintf(buffer, sizeof(buffer), format.c_str(), it->c_str());
	ss << buffer;

	return ss.str();
}

/// Return a time stamp in a string format
const string timeStamp(int x_pid)
{
	time_t rawtime;
	time(&rawtime);
	const tm* timeinfo = localtime (&rawtime);

	char dd[20];
	strftime(dd, sizeof(dd), "%Y%m%d_%H%M%S", timeinfo);

	if(x_pid != 0)
	{
		stringstream ss;
		ss<<dd<<"_"<<x_pid;
		return ss.str();
	}
	else return string(dd);
}

/// Convert a time in miliseconds to a time stamp (format used in subtitle files)
const string msToTimeStamp(TIME_STAMP x_ms)
{
	TIME_STAMP t = x_ms;
	int msecs = t % 1000;
	t /= 1000;
	int secs = t % 60;
	t /= 60;
	int mins = t % 60;
	t /= 60;
	int hours = t;

	char str[32];
	snprintf(str, sizeof(str), "%02d:%02d:%02d,%03d", hours, mins, secs, msecs);

	return string(str);
}

/// Convert a time stamp (format used in subtitle files) to a time in ms
TIME_STAMP timeStampToMs(const string& x_timeStamp)
{
	int hours, mins, secs, msecs;
	sscanf(x_timeStamp.c_str(), "%02d:%02d:%02d,%03d", &hours, &mins, &secs, &msecs);
	TIME_STAMP t = msecs;
	t += secs *    1000;
	t += mins *   60000;
	t += hours * 360000;
	return t;
}


/// Return an absolute timestamp in miliseconds. Absolute timestamps are based on processor time and are used on server side
///
/// important note: gettimeofday may hypothetically have some abrupt time changes if the clock is set but this is the
///                 only way so far to have a realistic time stamp as clock_gettime(CLOCK_MONOTONIC, &tp) starts from an
///                 arbitrary time.
TIME_STAMP getAbsTimeMs()
{
	struct timeval tp;
	gettimeofday(&tp, nullptr);
	return tp.tv_sec * 1000 + tp.tv_usec / 1000;

	/*
		struct timespec tp;
		if(clock_gettime(CLOCK_MONOTONIC, &tp) == -1)
			throw MkException("Cannot get time", LOC);
		return tp.tv_sec * 1000 + tp.tv_nsec / 1000000;
		*/
}

/// Return a time stamp if the file name clearly contains a date. Return 0 otherwise
TIME_STAMP timeStampFromFileName(const string& x_fileName)
{
	struct tm tm;
	if(sscanf(basename(x_fileName).c_str(), "%4d%2d%2d_%2d%2d%2d_", &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec) != 6)
		return 0;
	// printf("TS%4d-%2d-%2d_%2d-%2d-%2d\n",  tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	tm.tm_year -= 1900; // year start at 1900
	tm.tm_mon  --;      // months since january
	TIME_STAMP t = timegm(&tm);
	// printf("TS%4d-%2d-%2d_%2d-%2d-%2d\n",  tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	t *= 1000;
	return t;
}

// Convert a string to json object
/*
string jsonify(const string& x_name, const string& x_value)
{
	stringstream ss;
	ss<<"\""<<x_name<<"\": \""<<x_value<<"\"";
	return ss.str();
}

// Convert a string to json object
string jsonify(const string& x_name, TIME_STAMP x_value)
{
	stringstream ss;
	ss<<"\""<<x_name<<"\": "<<x_value;
	return ss.str();
}
*/



// Create an empty config file
void createEmptyConfigFile(const string& x_fileName, bool x_header)
{
	ofstream of(x_fileName.c_str());
	of<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"<<endl;

	if(x_header)
		of<<"<!-- Markus XML configuration file -->"<<endl;

	of.close();
}


/**
* @brief Compare 2 files
*
* @param x_file1 File 1
* @param x_file2 File 2
*
* @return true if files are identical
*/
bool compareFiles(const string& x_file1, const string& x_file2)
{
	ifstream file1(x_file1.c_str());
	ifstream file2(x_file2.c_str());

	if(!file1.is_open())
		throw MkException("Cannot open file " + x_file1, LOC);
	if(!file2.is_open())
		throw MkException("Cannot open file " + x_file2, LOC);

	string line1;
	string line2;

	while(getline(file1, line1))
	{
		if(!getline(file2, line2))
			return false;
		if(line1 != line2)
			return false;
	}
	if(getline(file1, line2))
		return false;
	return true;
}

/**
* @brief Return the content of a file
*
* @param filename
*
* @return
*/
string getFileContents(const string& x_fileName)
{
	ifstream in(x_fileName.c_str(), ios::in | ios::binary);
	if (in)
	{
		string contents;
		in.seekg(0, ios::end);
		contents.resize(in.tellg());
		in.seekg(0, ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return contents;
	}
	else throw MkException("Cannot open file " + x_fileName, LOC);
}

/**
* @brief Return the diagonal of an image
*
* @param x_image
*
* @return
*/
double diagonal(const Mat& x_image)
{
	return sqrt(POW2(x_image.cols) + POW2(x_image.rows));
}

/**
* @brief Return the base name of a file (= the name of the file without directory)
*
* @param x_pathName
*
* @return
*/
std::string basename(const std::string& x_pathName )
{
	int lastindex = x_pathName.find_last_of("/");
	return x_pathName.substr(lastindex + 1);
}

/**
* @brief Execute a command, keeping stdout
*
* @param x_cmd     Command to execute
* @param xr_stdout Stream to store stdout
*
* @return number of lines in output
*/
void execute(const string& x_cmd, ostream& xr_stdout)
{
	FILE* pipe = popen(x_cmd.c_str(), "r");
	if (!pipe)
		throw MkException("Error at execution of command: " + x_cmd, LOC);
	char buffer[256];

	// Append result to ...
	while(!feof(pipe))
		if(fgets(buffer, sizeof(buffer), pipe) != nullptr)
		{
			xr_stdout << buffer;
		}
	pclose(pipe);
}

/**
* @brief Execute a command, keeping stdout
*
* @param x_cmd     Command to execute
* @param xr_result Vector of strings to store output
*
* @return number of lines in output
*/
void execute(const string& x_cmd, vector<string>& xr_result)
{
	FILE* pipe = popen(x_cmd.c_str(), "r");
	xr_result.clear();
	if (!pipe)
		throw MkException("Error at execution of command: " + x_cmd, LOC);
	char buffer[128];
	while(!feof(pipe))
	{
		// Append result to string vector
		if(fgets(buffer, 128, pipe) != nullptr)
		{
			xr_result.push_back(string(buffer));
			// Remove last \n
			if(! xr_result.back().empty() && xr_result.back().back() == '\n')
				xr_result.back().pop_back();
			if(xr_result.back().empty())
				xr_result.pop_back();
		}
	}
	pclose(pipe);
}

/**
* @brief Convert a string to a boolean value
*
* @param x_value     String value representing the boolean
*
* @return boolean value
*/
bool boolValue(const std::string& x_value)
{
	if(x_value.empty())
		return false;
	if(x_value == "true" || x_value == "True" || x_value == "TRUE" || x_value == "1")
		return true;
	if(x_value == "false" || x_value == "False" || x_value == "FALSE" || x_value == "0")
		return false;

	throw MkException("Ambiguous value cannot be converted to bool: " + x_value, LOC);
}

/**
* @brief Write image to disk and add the path of the image to the event
*
* @param x_image        Image to add
* @param x_fileWithPath Path to image
* @param xr_event       Event to modify
*
*/
void addExternalImage(const Mat& x_image, const std::string& x_name, const std::string& x_fileWithPath, Event& xr_event)
{
	imwrite(x_fileWithPath, x_image);
	// LOG_DEBUG(m_logger, "Add external file to event " << x_name << ": " << x_fileWithPath);
	xr_event.AddExternalFile(x_name, x_fileWithPath);
}


/**
* @brief Create a reader for an annotation file (*.ass or *.srt)
*
* @param x_fileName     Name of the file, for
* @param x_fileWithPath Path to image
* @param xr_event       Event to modify
* @return Pointer to the new object
*
*/
AnnotationFileReader* createAnnotationFileReader(const string& x_fileName, int x_width, int x_height)
{
	AnnotationFileReader* p = nullptr;
	if(x_fileName.empty())
	{
		// TODO by LW: This only exists for intrusion.tracklets, this should normally throw
		// throw MkException("Name for file is empty", LOC);
		cout <<"WARNING: Name for file is empty" << endl;
		p = new AnnotationSrtFileReader();
		return p;
	}
	else if(x_fileName.substr(x_fileName.find_last_of(".") + 1) == "ass")
	{
		p = new AnnotationAssFileReader(x_width, x_height);
	}
	else if(x_fileName.substr(x_fileName.find_last_of(".") + 1) == "srt")
	{
		p = new AnnotationSrtFileReader();
	}
	else throw MkException("Invalid file name : " + x_fileName, LOC);
	p->Open(x_fileName);
	return p;
}
