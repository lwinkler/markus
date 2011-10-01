// +--------------------------------------------------------------------------+
// | File      : Detector.h                                                   |
// | Utility   : Background extraction/pre-processing                         |
// | Author    : Laurent Winkler                                              |
// | Date      : July 2010                                                    |
// | Remarque  : none                                                         |
// +--------------------------------------------------------------------------+

#include "Detector.h"

#include <iostream>
#include <cstdio>
#include <highgui.h>

using namespace std;

float Detector::m_background_alpha = 0;
const float Detector::m_background_alpha_r = 100 / 0.1;  // RESOLUTION / MAX
int Detector::m_background_alpha_d = 0;

float Detector::m_foreground_thres = 0;
const float Detector::m_foreground_thres_r = 100 / 1.0;
int Detector::m_foreground_thres_d = 0;

int Detector::m_inputBlurSize = 0;
int Detector::m_foregroundFilterSize = 0;


Detector::Detector(int width, int height, int depth, int channels)
{
	m_background_alpha = 0;
	m_background_alpha_d = 0;
	m_foreground_thres = 0;
	m_foreground_thres_d = 0;
	m_foregroundFilterSize = 0;

	m_foreground = cvCreateImage(cvSize(width, height), depth, 1);
	m_foreground_rff = cvCreateImage(cvSize(width, height), depth, 1);
	m_background = cvCreateImage(cvSize(width, height), depth, channels);
	m_lastImg = cvCreateImage(cvSize(width, height), depth, channels);
	m_temporalDiff = cvCreateImage(cvSize(width, height), depth, 1);
	m_blobsImg = cvCreateImage(cvSize(width, height), depth, channels);
	Reset();
	
	
}

void detectorBarCallback0(int pos) 
{
	Detector::SetInputBlurSize( pos);
	cout<<"Input blur filter size set to "<<Detector::GetInputBlurSize()<<endl;
}

void detectorBarCallback1(int pos) 
{
	Detector::SetBackgroundAlpha( pos / Detector::m_background_alpha_r);
	cout<<"Background alpha set to "<<Detector::GetBackgroundAlpha()<<endl;
}

void detectorBarCallback2(int pos) 
{
	Detector::SetForegroundThres( pos / Detector::m_foreground_thres_r);	
	cout<<"Foreground threshold set to "<<Detector::GetForegroundThres()<<endl;
}

void detectorBarCallback3(int pos) 
{
	Detector::SetForegroundFilterSize(pos);	
	cout<<"Foreground filter size set to "<<Detector::GetForegroundFilterSize()<<endl;
}

void Detector::CreateParamWindow()
{
	cvNamedWindow("Parameters", CV_WINDOW_AUTOSIZE); 
	cvMoveWindow("Parameters", 0, 0);
	
	cvCreateTrackbar( "Input blur filter size ", "Parameters", &m_inputBlurSize, 7, detectorBarCallback0 );
	cvCreateTrackbar( "Background alpha     (0.1%) ", "Parameters", &m_background_alpha_d, 100, detectorBarCallback1 );
	cvCreateTrackbar( "Foreground threshold (1%) ", "Parameters", &m_foreground_thres_d, 100, detectorBarCallback2 );
	cvCreateTrackbar( "Foreground morph filter size ", "Parameters", &m_foregroundFilterSize, 7, detectorBarCallback3 );

}

Detector::~Detector(void)
{
	cvReleaseImage(&m_background);
	cvReleaseImage(&m_foreground);
	cvReleaseImage(&m_foreground_rff);
	cvReleaseImage(&m_lastImg);
	cvReleaseImage(&m_temporalDiff);
}

void Detector::Reset()
{
	m_emptyTemporalDiff = true;
	m_emptyBackgroundSubtraction = true;
}



/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Background update  */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void Detector::BlurInput(IplImage* x_img, IplImage* x_output)
{
	if(m_inputBlurSize % 2 == 0) m_inputBlurSize++; //odd numbers only !
	if(m_inputBlurSize > 1)
	{
		cvSmooth(x_img, x_output, CV_GAUSSIAN, m_inputBlurSize);
	}
	else cvCopy(x_img, x_output);
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Background update  */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void Detector::UpdateBackground(IplImage* x_img)
{
	
	//cout<<" img depth"<<x_img->depth<<" backgr"<<m_background->depth<<endl;
	if(m_emptyBackgroundSubtraction) {
		cvCopy(x_img, m_background);
		m_emptyBackgroundSubtraction = false;
	}
	else {
		
		assert(x_img->depth == m_background->depth);
		assert(x_img->width == m_background->width);
		//assert(x_img->widthStep == m_background->widthStep);
		assert(x_img->height == m_background->height);
		//assert(x_img->imageSize == m_background->imageSize);
		assert(x_img->nChannels == m_background->nChannels);
		
		if(m_background->depth == IPL_DEPTH_32F)
		{
			cvRunningAvg(x_img, m_background, m_background_alpha);
		}
		else
		{
			// Image in unsigned char
			uchar * p_runner1 = (uchar *) x_img->imageDataOrigin;
			uchar * p_runner2 = (uchar *) m_background->imageDataOrigin;
			int cpt = x_img->imageSize;
			
			while (cpt)
			{
				*p_runner2 = (uchar) ((*p_runner2) * (1 - m_background_alpha) + (*p_runner1) * m_background_alpha);
				p_runner1++;
				p_runner2++;
				cpt--;
			}
		
		}
		//cout<<"alpha"<<m_background_alpha<<endl;
		//cout<<"thres"<<m_foreground_thres<<endl;

		//cvSet(m_foreground, cvScalar(0,0,0));
	}
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Background update  */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void Detector::UpdateBackgroundMask(IplImage* x_img, IplImage* x_mask)
{
	
	//cout<<" img depth"<<x_img->depth<<" backgr"<<m_background->depth<<endl;
	if(m_emptyBackgroundSubtraction) {
		cvCopy(x_img, m_background);
		m_emptyBackgroundSubtraction = false;
	}
	else {
		
		assert(x_img->depth == m_background->depth);
		assert(x_img->width == m_background->width);
		//assert(x_img->widthStep == m_background->widthStep);
		assert(x_img->height == m_background->height);
		//assert(x_img->imageSize == m_background->imageSize);
		assert(x_img->nChannels == m_background->nChannels);
		
		if(m_background->depth == IPL_DEPTH_32F)
		{
			cvRunningAvg(x_img, m_background, m_background_alpha, x_mask);
		}
		else
		{
			// Image in unsigned char
			uchar * p_runner1 = (uchar *) x_img->imageDataOrigin;
			uchar * p_runner2 = (uchar *) m_background->imageDataOrigin;
			uchar * p_runner3 = (uchar *) x_mask->imageDataOrigin;
			int cpt = x_img->imageSize;
			
			while (cpt)
			{
				if(*p_runner3)
					*p_runner2 = (uchar) ((*p_runner2) * (1 - m_background_alpha) + (*p_runner1) * m_background_alpha);
				p_runner1++;
				p_runner2++;
				if(cpt % x_img->nChannels == 0)p_runner3++;
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

void Detector::ExtractForeground(IplImage* x_img)
{
	
	static IplImage* tmp = cvCreateImage(cvSize(x_img->width, x_img->height), x_img->depth, x_img->nChannels);
	
	cvAbsDiff(x_img, m_background, tmp);
	cvCvtColor(tmp, m_foreground, CV_RGB2GRAY);
	cvThreshold(m_foreground, m_foreground, m_foreground_thres* 255, 255, CV_THRESH_BINARY);
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

void Detector::ExtractForegroundMax(IplImage* x_img)
{
	
	static IplImage* tmp = cvCreateImage(cvSize(x_img->width, x_img->height), x_img->depth, x_img->nChannels);
	
	cvAbsDiff(x_img, m_background, tmp);
	
	char* p_tmp = (char*) tmp->imageDataOrigin;
	char* p_fore = (char*) m_foreground->imageDataOrigin;
	int cpt = m_foreground->imageSize;
	
	while(cpt)
	{
		int max = 0;
		for( char* p_tmp2  = p_tmp; p_tmp2 < p_tmp + (char)x_img->nChannels; p_tmp2++)
		{
			if(*(p_tmp2) > max) max = *(p_tmp2);
		}
		if(max >= m_foreground_thres * 255)
			*p_fore = (uchar) 255;
		else 
			*p_fore= (uchar) 0;
		
		p_tmp  += (char) tmp->nChannels;
		p_fore ++;
		
		cpt--;
	}

	
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Remove false background pixels by 8 neighbor vote   */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/

void Detector::RemoveFalseForegroundNeigh()
{
	if(m_foreground->depth==IPL_DEPTH_8U && m_foreground->depth==IPL_DEPTH_8U)
	{
		for(int i = 1; i< m_foreground->width - 1; i++)
			for(int j = 1; j < m_foreground->height - 1; j++)
			{
				int ind = i + j * m_foreground_rff->widthStep;
				m_foreground_rff->imageData[ind] = 0;
					//m_foreground->imageData[i + j * m_foreground->widthStep];
				int cpt = 0;
				if(m_foreground->imageData[ind])
					for(int ii = -1; ii < 2 ; ii++)
					{
						for(int jj = -1; jj < 2 ; jj++)
						{
							//cout<<ii<<" "<<jj<<" "<<(int)(uchar)m_foreground->imageData[i + ii + (j + jj) * m_foreground->widthStep]<<endl;
							if((ii || jj) && m_foreground->imageData[i + ii + (j + jj) * m_foreground->widthStep])
								cpt++;
						}
					}
				if(cpt > 3) m_foreground_rff->imageData[ind] = 255;
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
	if(m_foregroundFilterSize == 0) m_foregroundFilterSize=1;
	if(m_foregroundFilterSize > 1)
	{
		static IplConvKernel* element = cvCreateStructuringElementEx(3, 3, 0, 0, CV_SHAPE_ELLIPSE); // CV_SHAPE_RECT
		static int size = m_foregroundFilterSize;
		
		if(element->nCols != m_foregroundFilterSize)
		{
			cvReleaseStructuringElement(&element);
			element = cvCreateStructuringElementEx(m_foregroundFilterSize, m_foregroundFilterSize, 0, 0, CV_SHAPE_ELLIPSE); // CV_SHAPE_RECT
		}
		
		cvMorphologyEx(m_foreground, m_foreground_rff, NULL, element, CV_MOP_OPEN, 1);
	
	}
	else cvCopy(m_foreground, m_foreground_rff);
}


/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/* Temporal differencing  */
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void Detector::TemporalDiff(IplImage* x_img)
{
	if(m_emptyTemporalDiff) {
		cvCopy(x_img, m_lastImg);
		m_emptyTemporalDiff = false;
	}
	else {
		static IplImage* tmp = cvCreateImage(cvSize(x_img->width, x_img->height), x_img->depth, x_img->nChannels);
	
		cvSub(x_img, m_lastImg, tmp);
		cvAbsDiff(x_img, m_lastImg, tmp);
		cvCvtColor(tmp, m_temporalDiff, CV_RGB2GRAY);
		
		cvCopy(x_img, m_lastImg);
	}



}

