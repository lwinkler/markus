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

#include "Detector.h"

#include <iostream>
#include <cstdio>
#include <highgui.h>

using namespace std;
using namespace cv;

Detector::Detector(const DetectorParameter& x_param, int x_width, int x_height, int x_type) :
	m_param(x_param)
{
	m_foreground 		= new Mat(cvSize(x_width, x_height), CV_8UC1);
	m_foreground_rff	= new Mat(cvSize(x_width, x_height), CV_8UC1);
	m_background 		= new Mat(cvSize(x_width, x_height), x_type);
	m_lastImg 		= new Mat(cvSize(x_width, x_height), x_type);
	m_temporalDiff 		= new Mat(cvSize(x_width, x_height), CV_8UC1);
	m_blobsImg 		= new Mat(cvSize(x_width, x_height), x_type);
	
	Reset();
}


Detector::~Detector(void)
{
	delete(m_background);
	delete(m_foreground);
	delete(m_foreground_rff);
	delete(m_lastImg);
	delete(m_temporalDiff);
}

void Detector::Reset()
{
	m_emptyTemporalDiff = false;
	m_emptyBackgroundSubtraction = false;
}



/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Background update  */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void Detector::BlurInput(const Mat* x_img, Mat* x_output)
{
	int blurSize = 0;
	if(m_param.inputBlurSize % 2 == 0) 
		blurSize = m_param.inputBlurSize + 1; //odd numbers only !
	else blurSize = m_param.inputBlurSize;
	if(blurSize > 1)
	{
		cvSmooth(x_img, x_output, CV_GAUSSIAN, blurSize);
	}
	else cvCopy(x_img, x_output);
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Background update  */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void Detector::UpdateBackground(Mat* x_img)
{
	float backgroundAlpha = m_param.backgroundAlpha;
	//cout<<" img depth"<<x_img->depth<<" backgr"<<m_background->depth<<endl;
	if(m_emptyBackgroundSubtraction) 
	{
		cvCopy(x_img, m_background);
		m_emptyBackgroundSubtraction = false;
	}
	else 
	{
		// assert(x_img->depth == m_background->depth);
		assert(x_img->cols == m_background->cols);
		//assert(x_img->widthStep == m_background->widthStep);
		assert(x_img->rows == m_background->rows);
		//assert(x_img->imageSize == m_background->imageSize);
		assert(x_img->channels() == m_background->channels());
		
		if(m_background->depth() == IPL_DEPTH_32F)
		{
			assert(false);
			// HACK runningAvg(x_img, m_background, backgroundAlpha);
		}
		else
		{
			// Image in unsigned char
			uchar * p_runner1 = (uchar *) x_img->datastart;
			uchar * p_runner2 = (uchar *) m_background->datastart;
			int cpt = x_img->size().area();
			
			while (cpt)
			{
				*p_runner2 = (uchar) ((*p_runner2) * (1 - backgroundAlpha) + (*p_runner1) * backgroundAlpha);
				p_runner1++;
				p_runner2++;
				cpt--;
			}
		
		}
		//cout<<"alpha"<<backgroundAlpha<<endl;
		//cout<<"thres"<<m_param.foregroundThres<<endl;

		//cvSet(m_foreground, cvScalar(0,0,0));
	}
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Background update  */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void Detector::UpdateBackgroundMask(Mat* x_img, Mat* x_mask)
{
	float backgroundAlpha = m_param.backgroundAlpha;
	//cout<<" img depth"<<x_img->depth<<" backgr"<<m_background->depth<<endl;
	if(m_emptyBackgroundSubtraction) {
		cvCopy(x_img, m_background);
		m_emptyBackgroundSubtraction = false;
	}
	else {
		
		assert(x_img->depth() == m_background->depth());
		assert(x_img->cols == m_background->cols);
		//assert(x_img->widthStep == m_background->widthStep);
		assert(x_img->rows == m_background->rows);
		//assert(x_img->imageSize == m_background->imageSize);
		assert(x_img->channels() == m_background->channels());
		
		if(m_background->depth() == IPL_DEPTH_32F)
		{
			cvRunningAvg(x_img, m_background, backgroundAlpha, x_mask);
		}
		else
		{
			// Image in unsigned char
			uchar * p_runner1 = (uchar *) x_img->datastart;
			uchar * p_runner2 = (uchar *) m_background->datastart;
			uchar * p_runner3 = (uchar *) x_mask->datastart;
			int cpt = x_img->size().area();
			
			while (cpt)
			{
				if(*p_runner3)
					*p_runner2 = (uchar) ((*p_runner2) * (1 - backgroundAlpha) + (*p_runner1) * backgroundAlpha);
				p_runner1++;
				p_runner2++;
				if(cpt % x_img->channels() == 0)p_runner3++;
				cpt--;
			}
		
		}
		//cout<<"alpha"<<m_background_alpha<<endl;
		//cout<<"thres"<<m_foreground_thres<<endl;

		//cvSet(m_foreground, cvScalar(0,0,0));
	}
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Extract foreground with threshold */ 
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/

void Detector::ExtractForeground(Mat* x_img)
{
	static Mat* tmp = new Mat(cvSize(x_img->cols, x_img->rows), x_img->depth(), x_img->channels());
	
	cvAbsDiff(x_img, m_background, tmp);
	cvCvtColor(tmp, m_foreground, CV_RGB2GRAY);
	cvThreshold(m_foreground, m_foreground, m_param.foregroundThres* 255, 255, CV_THRESH_BINARY);
	//cvAdaptiveThreshold(m_foreground, m_foreground, 255, 0, 1);//, int adaptiveMethod, int thresholdType, int blockSize, double C)
	/*assert(x_img->depth == m_background->depth);
	assert(x_img->width == m_background->width);
	assert(x_img->widthStep == m_background->widthStep);
	assert(x_img->height == m_background->height);
	assert(x_img->imageSize == m_background->imageSize);
	assert(x_img->nChannels == m_background->nChannels);
	assert(x_img->depth == m_foreground->depth);
	assert(x_img->width == m_foreground->width);
	//assert(x_img->widthStep == m_foreground->widthStep);
	assert(x_img->height == m_foreground->height);
	//assert(x_img->imageSize == m_foreground->imageSize);
	//assert(x_img->nChannels == m_foreground->nChannels);
	
	if(x_img->depth == IPL_DEPTH_8U)
	{
		int cpt = x_img->imageSize;
		char* p_img = (char*) x_img->imageDataOrigin;
		char* p_back = (char*) m_background->imageDataOrigin;
		char* p_fore = (char*) m_foreground->imageDataOrigin;

		while(cpt)
		{
			int max = 0;
			for( int k=0; k< x_img->nChannels; k++)
			{
				int measure = abs((*(p_img + k) - *(p_back + k)));
				if(measure > max) max = measure;
			}
			uchar val;
			val = (uchar) 255;
			
			if(max < m_foreground_thres * 255)
				val = (uchar) 0;
			
			//char* p_fore2;
			//for( int k=0; k< m_foreground->nChannels; k++)
			//{
			*p_fore= (uchar) val;
			//}
			//printf("k=%d %p\n",0,p_fore);
			
			p_img  += (char) x_img->nChannels;
			p_back += (char) m_background->nChannels;
			p_fore ++;//= (char) m_foreground->nChannels;
			cpt --;
		}
		
	}	
	else if(x_img->depth == IPL_DEPTH_32F)
	{
		throw("Error : in ExtractForeground");
		
		
	}
	else throw("Error : in ExtractForeground");
		
		*/
}


/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Extract foreground with threshold on maximal difference in RGB */ 
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/

void Detector::ExtractForegroundMax(Mat* x_img)
{
	float foregroundThres = m_param.foregroundThres;
	static Mat* tmp = new Mat(cvSize(x_img->cols, x_img->rows), x_img->depth(), x_img->channels());
	
	cvAbsDiff(x_img, m_background, tmp);
	
	char* p_tmp = (char*) tmp->datastart;
	char* p_fore = (char*) m_foreground->datastart;
	int cpt = m_foreground->size().area();
	
	while(cpt)
	{
		int max = 0;
		for( char* p_tmp2  = p_tmp; p_tmp2 < p_tmp + (char)x_img->channels(); p_tmp2++)
		{
			if(*(p_tmp2) > max) max = *(p_tmp2);
		}
		if(max >= foregroundThres * 255)
			*p_fore = (uchar) 255;
		else 
			*p_fore= (uchar) 0;
		
		p_tmp  += (char) tmp->channels();
		p_fore ++;
		
		cpt--;
	}
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Remove false background pixels by 8 neighbor vote   */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/

void Detector::RemoveFalseForegroundNeigh()
{
	if(m_foreground->depth() == IPL_DEPTH_8U && m_foreground->depth() == IPL_DEPTH_8U)
	{
		for(int i = 1; i< m_foreground->cols - 1; i++)
			for(int j = 1; j < m_foreground->rows - 1; j++)
			{
				int ind = i + j * m_foreground_rff->cols;
				m_foreground_rff->data[ind] = 0;
					//m_foreground->imageData[i + j * m_foreground->widthStep];
				int cpt = 0;
				if(m_foreground->data[ind])
					for(int ii = -1; ii < 2 ; ii++)
					{
						for(int jj = -1; jj < 2 ; jj++)
						{
							//cout<<ii<<" "<<jj<<" "<<(int)(uchar)m_foreground->imageData[i + ii + (j + jj) * m_foreground->widthStep]<<endl;
							if((ii || jj) && m_foreground->data[i + ii + (j + jj) * m_foreground->cols])
								cpt++;
						}
					}
				if(cpt > 3) m_foreground_rff->data[ind] = 255;
				//cout<<"cpt"<<cpt<<endl;
			}
	}
	else throw("Error in RemoveFalseForeground");
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Remove false background pixels a morphological filter   */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/

void Detector::RemoveFalseForegroundMorph()
{
	int filterSize = m_param.foregroundFilterSize;
	if(filterSize == 0) filterSize=1;
	if(filterSize > 1)
	{
		static IplConvKernel* element = cvCreateStructuringElementEx(3, 3, 0, 0, CV_SHAPE_ELLIPSE); // CV_SHAPE_RECT
		static int size = filterSize;
		
		if(element->nCols != filterSize)
		{
			cvReleaseStructuringElement(&element);
			element = cvCreateStructuringElementEx(filterSize, filterSize, 0, 0, CV_SHAPE_ELLIPSE); // CV_SHAPE_RECT
		}
		
		cvMorphologyEx(m_foreground, m_foreground_rff, NULL, element, CV_MOP_OPEN, 1);
	
	}
	else cvCopy(m_foreground, m_foreground_rff);
}


/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Temporal differencing  */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void Detector::TemporalDiff(Mat* x_img)
{
	if(m_emptyTemporalDiff) 
	{
		cvCopy(x_img, m_lastImg);
		m_emptyTemporalDiff = false;
	}
	else 
	{
		static Mat* tmp = new Mat(cvSize(x_img->cols, x_img->rows), x_img->depth(), x_img->channels());
	
		subtract(*x_img, *m_lastImg, *tmp);
		cvAbsDiff(x_img, m_lastImg, tmp);
		cvtColor(*tmp, *m_temporalDiff, CV_RGB2GRAY);
		
		cvCopy(x_img, m_lastImg);
	}



}

