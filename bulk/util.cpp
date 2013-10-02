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
#include <iostream>

#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

// Global variables

int g_colorArraySize = 54;
CvScalar g_colorArray[] =
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




/* Set image to the right size */

void adjustSize(const Mat* im_in, Mat* im_out)
{
	if(im_in->cols == im_out->cols && im_in->rows == im_out->rows) 
		im_in->copyTo(*im_out);
	else 
		resize(*im_in, *im_out, im_out->size(), CV_INTER_NN);
}

/* Set image to the right depth 
 WARNING :: buffers tmp1 and tmp2 are only generated once so this method must be used only for one type of images  */

void adjust(const Mat* im_in, Mat* im_out, Mat*& tmp1, Mat*& tmp2)
{
	
	if(im_in->depth() == im_out->depth())
	{
		if(tmp1==NULL)
		{
			//cout<<"create image in adjust tmp1 depth "<<im_out->depth<<endl;
			tmp1 = new Mat( cvSize(im_out->cols, im_out->rows), im_out->type());
		}
		adjustSize(im_in, tmp1);
		adjustChannels(tmp1, im_out);

	}
	else
	{
		if(tmp1==NULL)
		{
			//cout<<"create image in adjust IPL_DEPTH_32F tmp1"<<endl;
			tmp1 = new Mat( cvSize(im_out->cols, im_out->rows), im_in->type());
		}
		if(tmp2==NULL)
		{
			//cout<<"create image in adjust IPL_DEPTH_32F tmp2"<<endl;
			tmp2 = new Mat( cvSize(im_out->cols, im_out->rows), CV_MAKE_TYPE(im_in->depth(), im_out->channels()));
		}

		if(im_in->depth() == CV_8U && im_out->depth() == CV_32F)
		{
			adjustSize(im_in, tmp1);
			adjustChannels(tmp1, tmp2);
			//convertByte2Float(tmp2, im_out);
			tmp2->convertTo(*im_out, im_out->type(), 1.0 / 255);
		}
		else if(im_in->depth() == CV_32F && im_out->depth() == CV_8U)
		{
			adjustSize(im_in, tmp1);
			adjustChannels(tmp1, tmp2);

			//convertFloat2Byte(tmp2, im_out);
			tmp2->convertTo(*im_out, im_out->type(), 255);
		}
		else
		{
			throw("Cannot convert");
		}
	}
}

/* Set the image to the right number of channels */

void adjustChannels(const Mat* im_in, Mat* im_out)
{
	if(im_in->channels() == im_out->channels())
	{
		im_in->copyTo(*im_out);
	}
	else if(im_in->channels() == 1 && im_out->channels() == 3) 
	{
		cvtColor(*im_in, *im_out, CV_GRAY2RGB);
	}
	else if(im_in->channels() == 3 && im_out->channels() == 1) 
	{
		cvtColor(*im_in, *im_out, CV_RGB2GRAY);
	}
	else throw("Error in adjustChannels");
}

CvScalar colorFromStr(string x_str)
{
	int pos1 = 0;
	int pos2 = 0;
	int pos3 = 0;
	if(x_str[0] == '(')
	{
		pos1  = x_str.find(',', 1);
		pos2  = x_str.find(',', pos1 + 1);
		pos3  = x_str.find(')', pos2 + 1);
		assert(pos1 && pos2 && pos3);
		
		return cvScalar(atoi(x_str.substr(1, pos1 - 1).c_str()), 
				atoi(x_str.substr(pos1 + 1, pos2 - pos1 - 1).c_str()), 
				atoi(x_str.substr(pos2 + 1, pos3 - pos2 - 1).c_str()));
	}
	else return cvScalar(0, 0, 0);
}

CvScalar colorFromId(int x_id)
{
	return g_colorArray[x_id % g_colorArraySize];
}


void saveMat(const cv::Mat* x_mat, const std::string& x_name)
{
	if(x_mat->depth() == CV_32F)
	{
		Mat mat(x_mat->size(), CV_8UC3);
		x_mat->convertTo(mat, CV_8UC3, 255);
		IplImage img = mat;
		cvSaveImage(x_name.c_str(), &img);
	}
	else 
	{
		IplImage img = *x_mat;
		cvSaveImage(x_name.c_str(), &img);
	}
}

/* Split a string separated by a character*/
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	elems.clear();
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


/// Return a time stamp in a string format
const string timeStamp(){


	time_t rawtime;
	time(&rawtime);
	const tm* timeinfo = localtime (&rawtime);

	char dd[20] ;
	strftime(dd, sizeof(dd), "%Y%m%d_%T", timeinfo);
	return dd;
}

