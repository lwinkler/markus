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


void convertFloat2Byte(const IplImage *float_img,IplImage *byte_img)
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
		cvResize(im_in, im_out);
}

/* Set image to the right depth 
 WARNING :: buffers tmp1 and tmp2 are only generated once (static) so this method must be used only for one type of images  */

void adjust(const IplImage* im_in, IplImage* im_out, IplImage*& tmp1, IplImage*& tmp2)
{
	
	if(im_in->depth == im_out->depth)
	{
		if(tmp1==NULL)
		{
			cout<<"create image in adjust tmp1 depth "<<im_out->depth<<endl;
			tmp1 = cvCreateImage( cvSize(im_out->width, im_out->height), im_out->depth, im_in->nChannels);
		}
		adjustSize(im_in, tmp1);
		adjustChannels(tmp1, im_out);
	}
	else if(im_in->depth == IPL_DEPTH_8U && im_out->depth == IPL_DEPTH_32F) 
	{
		if(tmp1==NULL)
		{
			cout<<"create image in adjust IPL_DEPTH_32F tmp1"<<endl;
			tmp1 = cvCreateImage( cvSize(im_out->width, im_out->height), IPL_DEPTH_8U, im_in->nChannels);
		}
		if(tmp2==NULL)
		{
			cout<<"create image in adjust IPL_DEPTH_32F tmp2"<<endl;
			tmp2 = cvCreateImage( cvSize(im_out->width, im_out->height), IPL_DEPTH_32F, im_out->nChannels);
		}
		adjustSize(im_in, tmp1);
		adjustChannels(tmp1, tmp2);
		convertByte2Float(tmp2, im_out);		
	}
	else if(im_in->depth == IPL_DEPTH_32F && im_out->depth == IPL_DEPTH_8U) 
	{
		if(tmp1==NULL)
		{
			cout<<"create image in adjust IPL_DEPTH_32F tmp1"<<endl;
			tmp1 = cvCreateImage( cvSize(im_out->width, im_out->height), IPL_DEPTH_32F, im_in->nChannels);
		}
		if(tmp2==NULL)
		{
			cout<<"create image in adjust IPL_DEPTH_32F tmp2"<<endl;
			tmp2 = cvCreateImage( cvSize(im_out->width, im_out->height), IPL_DEPTH_32F, im_out->nChannels);
		}
		adjustSize(im_in, tmp1);		
		adjustChannels(tmp1, tmp2);
		convertFloat2Byte(tmp2, im_out);		
	}
	else
	{
		std::cout<<"Error in adjust : depth="<<im_in->depth<<endl;
		exit(-1);
	}
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

