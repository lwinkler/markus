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
	};
	float backgroundAlpha;
	float foregroundThres;
	int inputBlurSize;
	int foregroundFilterSize;
};

class Detector
{
private:
	// Background subtraction	
	IplImage* m_foreground;
	IplImage* m_foreground_rff;
	IplImage* m_background;
	// Temporal differencing
	IplImage* m_lastImg;
	IplImage* m_temporalDiff;
	IplImage* m_blobsImg;
	bool m_emptyBackgroundSubtraction;
	bool m_emptyTemporalDiff;

	const DetectorParameter& m_param;
	
public:
	Detector(const DetectorParameter& param, int width, int height, int depth, int channels);
	~Detector(void);

	void Reset();
	void BlurInput(const IplImage* src, IplImage* dst);
	void UpdateBackground(IplImage* img);
	void UpdateBackgroundMask(IplImage* img, IplImage* x_mask);
	void ExtractForeground(IplImage* img);
	void ExtractForegroundMax(IplImage* img);
	void RemoveFalseForegroundNeigh();
	void RemoveFalseForegroundMorph();

	void TemporalDiff(IplImage* img);
	
	void ExtractBlobs();

	inline IplImage* GetForeground(){ return m_foreground;};
	inline IplImage* GetForegroundRff(){ return m_foreground_rff;};
	inline IplImage* GetBackground(){ return m_background;};
	inline IplImage* GetTemporalDiff(){ return m_temporalDiff;};
};

#endif

