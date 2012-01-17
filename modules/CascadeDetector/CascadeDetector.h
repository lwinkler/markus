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
		m_list.push_back(new ParameterT<int>(0, "minNeighbors", 2, PARAM_INT, 1, 100, &minNeighbors,	"Minimum numbers of neighbors (higher: less sensitive)")); // TODO : Seems to be a bug with minNeighbors = 1 with most filters
		m_list.push_back(new ParameterT<int>(0, "minFaceSide", 0, PARAM_INT, 0, 200, &minFaceSide,	"Minimum size of the detected object"));
		m_list.push_back(new ParameterT<float>(0, "scaleFactor", 1.2, PARAM_FLOAT, 1, 2, &scaleFactor,	"Scale factor for scanning (higher: less sensitive)"));
		m_list.push_back(new ParameterT<std::string>(0, "filterFile", "modules/CascadeDetector/lbpcascade_frontalface.xml", PARAM_STR, &filterFile,
														"File with filter data of the detected object"));
		m_list.push_back(new ParameterT<std::string>(0, "color", "(255,255,255)", PARAM_STR, &color,	"Color to draw the output"));
		
		ParameterStructure::Init();
	};
	
	int minNeighbors;
	int minFaceSide;
	float scaleFactor;
	std::string filterFile;
	std::string color;
};

class DetectionThread : public QThread
{
public:
	explicit DetectionThread(QObject* parent = 0) {};
	virtual ~DetectionThread()
	{
		exit();
		wait();
	}
	void SetData(const cv::Mat & xr_smallImg, int x_minNeighbors, int x_minFaceSide, float x_scaleFactor)
	{
		xr_smallImg.copyTo(m_smallImg);
		m_minNeighbors = x_minNeighbors;
		m_minFaceSide = x_minFaceSide;
		m_scaleFactor = x_scaleFactor;
	}
	
	virtual void run();
	
	cv::CascadeClassifier m_cascade;
	std::vector<cv::Rect> GetDetectedObjects() const{ return m_detectedObjects;};

private:
	cv::Mat m_smallImg;
	int m_minNeighbors;
	int m_minFaceSide;
	float m_scaleFactor;
	
	std::vector<cv::Rect> m_detectedObjects;
};

class CascadeDetector : public ModuleAsync
{
private:
	CascadeDetectorParameterStructure m_param;
	static const char * m_type;
	
	//cv::CascadeClassifier m_cascade;
	std::vector<Object> m_detectedObjects;
	cv::Mat * m_debug;
	
	DetectionThread m_thread;
public:
	CascadeDetector(const ConfigReader& x_configReader);
	~CascadeDetector(void);
	//void CreateParamWindow();
	
	virtual void LaunchThread(const cv::Mat * img, const double x_timeSinceLastProcessing);
	virtual void NormalProcess(const cv::Mat * img, const double x_timeSinceLastProcessing);
	virtual void CopyResults();
	
	inline virtual int GetInputWidth() const {return m_param.width;};
	inline virtual int GetInputHeight() const {return m_param.height;};


protected:
	virtual const QThread & GetRefThread(){return m_thread;};

private:
	inline virtual const CascadeDetectorParameterStructure& GetRefParameter() const { return m_param;};

};

#endif

