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

#ifndef CASCADE_DETECTOR_H
#define CASCADE_DETECTOR_H

#include <cv.h>
#include <QThread>

#include "ModuleAsync.h"
#include "Parameter.h"
#include "StreamObject.h"

/*! \class CascadeDetector
 *  \brief Module class for detection based on cascade filters (Haar, ...)
 *
 */


class CascadeDetectorParameterStructure : public ModuleAsyncParameterStructure
{
	
public:
	CascadeDetectorParameterStructure(const ConfigReader& x_confReader) : 
		ModuleAsyncParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterInt(0, "minNeighbors", 2, PARAM_INT, 1, 100, 	&minNeighbors,	"Minimum numbers of neighbors (higher: less sensitive)")); // TODO : Seems to be a bug with minNeighbors = 1 with most filters
		m_list.push_back(new ParameterInt(0, "minSide", 0, PARAM_INT, 0, 200, 		&minSide,	"Minimum size of the detected object"));
		m_list.push_back(new ParameterFloat(0, "scaleFactor", 1.2, PARAM_FLOAT, 1, 2, 	&scaleFactor,	"Scale factor for scanning (higher: less sensitive)"));
		m_list.push_back(new ParameterString(0, "filterFile", "modules/CascadeDetector/lbpcascade_frontalface.xml",  &filterFile,
														"File with filter data of the detected object"));
		m_list.push_back(new ParameterString(0, "color", "(255,255,255)",		&color,	"Color to draw the output"));
		m_list.push_back(new ParameterString(0, "objectLabel", "casc", 			&objectLabel,	"Label to be applied to the objects detected by the cascade filter (e.g. face)"));
		
		ParameterStructure::Init();
	};
	
	int minNeighbors;
	int minSide;
	float scaleFactor;
	std::string filterFile;
	std::string color;
	std::string objectLabel;
};

class DetectionThread : public QThread
{
public:
	explicit DetectionThread(QObject* parent = 0) {m_timerThread = 0;};
	virtual ~DetectionThread()
	{
		exit();
		wait();
	}
	void SetData(const cv::Mat & xr_smallImg, int x_minNeighbors, int x_minSide, float x_scaleFactor)
	{
		xr_smallImg.copyTo(m_smallImg);
		m_minNeighbors 	= x_minNeighbors;
		m_minSide 	= x_minSide;
		m_scaleFactor 	= x_scaleFactor;
	}
	
	virtual void run();
	
	cv::CascadeClassifier m_cascade;
	const std::vector<cv::Rect>& GetDetectedObjects() const{ return m_detected;};
	long long m_timerThread;

protected:
	float m_scaleFactor;
	cv::Mat m_smallImg;
	int m_minNeighbors;
	int m_minSide;
	
	std::vector<cv::Rect> m_detected;
};

class CascadeDetector : public ModuleAsync
{
protected:
	CascadeDetectorParameterStructure m_param;
	static const char * m_type;
	
	std::vector<Object> m_detectedObjects;
	cv::Mat * m_input;
	cv::Mat * m_lastInput;
	cv::Mat * m_debug;
	
	DetectionThread m_thread;
public:
	CascadeDetector(const ConfigReader& x_configReader);
	~CascadeDetector(void);
	//void CreateParamWindow();
	
	virtual void LaunchThread();
	virtual void NormalProcess();
	virtual void CopyResults();
	void Reset();

protected:
	virtual const QThread & GetRefThread(){return m_thread;};

private:
	inline virtual CascadeDetectorParameterStructure& RefParameter() { return m_param;};

};

#endif

