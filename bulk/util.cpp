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

using namespace std;


void convertByte2Float(const IplImage *byte_img, IplImage *float_img)
{
	if( (byte_img->width != float_img->width) ||
		(byte_img->height != float_img->height) ||
		(byte_img->nChannels != float_img->nChannels) ||
		(byte_img->depth != IPL_DEPTH_8U) ||
		(float_img->depth != IPL_DEPTH_32F))
	{
		printf("convertByte2Float error. Aborting ... \n");
		exit(-1);
	}
	unsigned char * runner1= (unsigned char *)byte_img->imageDataOrigin;
	float * runner2 = (float *) float_img->imageDataOrigin;
	int skip = byte_img->widthStep-byte_img->nChannels*byte_img->width;
	for(int i=0;i<byte_img->height;i++)
	{
		for(int j=0;j<byte_img->width;j++)
		{
			for(int k=0;k<byte_img->nChannels;k++)
			{
				*runner2++ = (float) (*runner1++/255.);
			}
		}
		runner1+=skip;
	}
}


void convertFloat2Byte(const IplImage *float_img, IplImage *byte_img)
{
	if( (byte_img->width != float_img->width) ||
		(byte_img->height != float_img->height) ||
		(byte_img->nChannels != float_img->nChannels) ||
		(byte_img->depth != IPL_DEPTH_8U) ||
		(float_img->depth != IPL_DEPTH_32F))
	{
		printf("convertByte2Float error. Aborting ... \n");
		exit(-1);
	}
	unsigned char * runner1= (unsigned char *)byte_img->imageDataOrigin;
	float * runner2 = (float *) float_img->imageDataOrigin;
	int skip = byte_img->widthStep-byte_img->nChannels*byte_img->width;
	for(int i=0;i<byte_img->height;i++)
	{
		for(int j=0;j<byte_img->width;j++)
		{
			for(int k=0;k<byte_img->nChannels;k++)
			{
				*runner1++ = (unsigned char) (*runner2++*255.);
			}
		}
		runner1+=skip;
	}
}

/* Set image to the right size */

void adjustSize(const IplImage* im_in, IplImage* im_out)
{
	if(im_in->width == im_out->width && im_in->height == im_out->height) 
		cvCopy(im_in, im_out);
	else 
		cvResize(im_in, im_out, CV_INTER_NN);
}

/* Set image to the right depth 
 WARNING :: buffers tmp1 and tmp2 are only generated once (static) so this method must be used only for one type of images  */

void adjust(const IplImage* im_in, IplImage* im_out, IplImage*& tmp1, IplImage*& tmp2)
{
	
	if(im_in->depth == im_out->depth)
	{
		if(tmp1==NULL)
		{
			//cout<<"create image in adjust tmp1 depth "<<im_out->depth<<endl;
			tmp1 = cvCreateImage( cvSize(im_out->width, im_out->height), im_out->depth, im_in->nChannels);
		}
		adjustSize(im_in, tmp1);
		adjustChannels(tmp1, im_out);
	}
	else if(im_in->depth == IPL_DEPTH_8U && im_out->depth == IPL_DEPTH_32F) 
	{
		if(tmp1==NULL)
		{
			//cout<<"create image in adjust IPL_DEPTH_32F tmp1"<<endl;
			tmp1 = cvCreateImage( cvSize(im_out->width, im_out->height), IPL_DEPTH_8U, im_in->nChannels);
		}
		if(tmp2==NULL)
		{
			//cout<<"create image in adjust IPL_DEPTH_32F tmp2"<<endl;
			tmp2 = cvCreateImage( cvSize(im_out->width, im_out->height), IPL_DEPTH_8U, im_out->nChannels);
		}
		adjustSize(im_in, tmp1);
		adjustChannels(tmp1, tmp2);
		convertByte2Float(tmp2, im_out);		
	}
	else if(im_in->depth == IPL_DEPTH_32F && im_out->depth == IPL_DEPTH_8U) 
	{
		if(tmp1==NULL)
		{
			//cout<<"create image in adjust IPL_DEPTH_32F tmp1"<<endl;
			tmp1 = cvCreateImage( cvSize(im_out->width, im_out->height), IPL_DEPTH_32F, im_in->nChannels);
		}
		if(tmp2==NULL)
		{
			//cout<<"create image in adjust IPL_DEPTH_32F tmp2"<<endl;
			tmp2 = cvCreateImage( cvSize(im_out->width, im_out->height), IPL_DEPTH_32F, im_out->nChannels);
		}
		adjustSize(im_in, tmp1);		
		adjustChannels(tmp1, tmp2);
		convertFloat2Byte(tmp2, im_out);		
	}
	else
	{
		printf("Cannot convert from %d to %d\n", im_in->depth, im_out->depth);
		throw(string("Error in adjust : depth ")); 
	}
	// TODO : maybe use cvConvertScale(tmp2,im_out,255,0);
}

/* Set the image to the right number of channels */

void adjustChannels(const IplImage* im_in, IplImage* im_out)
{
	if(im_in->nChannels == im_out->nChannels)
	{
		cvCopy(im_in, im_out);
	}
	else if(im_in->nChannels == 1 && im_out->nChannels == 3) 
	{
		cvCvtColor(im_in, im_out, CV_GRAY2RGB);
	}
	else if(im_in->nChannels == 3 && im_out->nChannels == 1) 
	{
		cvCvtColor(im_in, im_out, CV_RGB2GRAY);
	}
	else
	{
		cout<<"Error in adjustChannels"<<endl;
	}
}

CvScalar ColorFromStr(string x_str)
{
	int c1 = 0;
	int c2 = 0;
	int c3 = 0;
	
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
