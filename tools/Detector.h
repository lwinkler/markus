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

#ifndef DETECTOR_H
#define DETECTOR_H

#include <cv.h>

/*! \class Detector
 *  \brief Class containing methods/attributes to detect moving objects in a scene
 *
 * Main parts are background modelization and foreground extraction
 */

class DetectorParameter
{
public:
	DetectorParameter()
	{
		backgroundAlpha = 0.02;
		foregroundThres = 0.2;
		inputBlurSize = 3;
		foregroundFilterSize = 3;
		backgroundDepth = IPL_DEPTH_32F; // TODO : Fix this
	};
	float backgroundAlpha;
	float foregroundThres;
	int inputBlurSize;
	int foregroundFilterSize;
	int backgroundDepth;
};

class Detector
{
private:
	// Background subtraction	
	cv::Mat* m_foreground;
	cv::Mat* m_foreground_rff;
	cv::Mat* m_background;
	// Temporal differencing
	cv::Mat* m_lastImg;
	cv::Mat* m_temporalDiff;
	cv::Mat* m_blobsImg;
	bool m_emptyBackgroundSubtraction;
	bool m_emptyTemporalDiff;

	const DetectorParameter& m_param;
	
public:
	Detector(const DetectorParameter& param, int width, int height, int type);
	~Detector(void);

	void Reset();
	void BlurInput(const cv::Mat* src, cv::Mat* dst);
	void UpdateBackground(cv::Mat* img);
	void UpdateBackgroundMask(cv::Mat* img, cv::Mat* x_mask);
	void ExtractForeground(cv::Mat* img);
	void ExtractForegroundMax(cv::Mat* img);
	void RemoveFalseForegroundNeigh();
	void RemoveFalseForegroundMorph();

	void TemporalDiff(cv::Mat* img);
	
	void ExtractBlobs();

	inline cv::Mat* GetForeground(){ return m_foreground;};
	inline cv::Mat* GetForegroundRff(){ return m_foreground_rff;};
	inline cv::Mat* GetBackground(){ return m_background;};
	inline cv::Mat* GetTemporalDiff(){ return m_temporalDiff;};
//void cvCopy(cv::Mat* arg1, cv::Mat* arg2);
//    void absDiff(cv::Mat arg1, cv::Mat arg2, cv::Mat arg3);
//    void cvSmooth(const cv::Mat* arg1, cv::Mat* arg2, int arg3, int arg4);

};

#endif

