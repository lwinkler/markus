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

#include <highgui.h>

using namespace std;
using namespace cv;

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
	else if(im_in->depth() == CV_8U && im_out->depth() == CV_32F) 
	{
		if(tmp1==NULL)
		{
			//cout<<"create image in adjust IPL_DEPTH_32F tmp1"<<endl;
			tmp1 = new Mat( cvSize(im_out->cols, im_out->rows), CV_MAKE_TYPE(CV_8U, im_in->channels()));
		}
		if(tmp2==NULL)
		{
			//cout<<"create image in adjust IPL_DEPTH_32F tmp2"<<endl;
			tmp2 = new Mat( cvSize(im_out->cols, im_out->rows), CV_MAKE_TYPE(CV_8U, im_out->channels()));
		}
		adjustSize(im_in, tmp1);
		adjustChannels(tmp1, tmp2);
		//convertByte2Float(tmp2, im_out);
		tmp2->convertTo(*im_out, im_out->type(), 1.0 / 255);
	}
	else if(im_in->depth() == CV_32F && im_out->depth() == CV_8U) 
	{
		if(tmp1==NULL)
		{
			//cout<<"create image in adjust IPL_DEPTH_32F tmp1"<<endl;
			tmp1 = new Mat( cvSize(im_out->cols, im_out->rows), CV_MAKE_TYPE(CV_32F, im_in->channels()));
		}
		if(tmp2==NULL)
		{
			//cout<<"create image in adjust IPL_DEPTH_32F tmp2"<<endl;
			tmp2 = new Mat( cvSize(im_out->cols, im_out->rows), CV_MAKE_TYPE(CV_32F, im_out->channels()));
		}
		adjustSize(im_in, tmp1);		
		adjustChannels(tmp1, tmp2);

		//convertFloat2Byte(tmp2, im_out);		
		tmp2->convertTo(*im_out, im_out->type(), 255);
	}
	else
	{
		printf("Cannot convert from %d to %d\n", im_in->depth(), im_out->depth());
		//throw(string("Error in adjust : depth ")); 
		assert(false);
	}
	// TODO : maybe use cvConvertScale(tmp2,im_out,255,0);
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

CvScalar ColorFromStr(string x_str)
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

