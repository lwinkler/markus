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

#ifndef FACE_DETECTOR_H
#define FACE_DETECTOR_H

#include <cv.h>
#include "ModuleAsync.h"
#include "Parameter.h"

#include <QThread>

/*! \class FaceDetector
 *  \brief Class containing methods/attributes of a slit camera
 *
 */


class FaceDetectorParameterStructure : public ModuleAsyncParameterStructure
{
	
public:
	FaceDetectorParameterStructure(ConfigReader& x_confReader, const std::string& x_moduleName) : 
		ModuleAsyncParameterStructure(x_confReader, x_moduleName)
	{
		m_list.push_back(new ParameterT<int>(0, "minNeighbors", 0, PARAM_INT, 0, 100, &minNeighbors));
		m_list.push_back(new ParameterT<int>(0, "minFaceSide", 0, PARAM_INT, 0, 200, &minFaceSide));
		m_list.push_back(new ParameterT<float>(0, "scaleFactor", 1.2, PARAM_FLOAT, 1, 2, &scaleFactor));
		m_list.push_back(new ParameterT<std::string>(0, "filterFile", "modules/FaceDetector/lbpcascade_frontalface.xml", PARAM_STR, &filterFile));
		
		ParameterStructure::Init();
	};
	
	int minNeighbors;
	int minFaceSide;
	float scaleFactor;
	std::string filterFile;
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
	std::vector<cv::Rect> GetDetectedObjects() const{ return m_faces;};

private:
	cv::Mat m_smallImg;
	int m_minNeighbors;
	int m_minFaceSide;
	float m_scaleFactor;
	
	std::vector<cv::Rect> m_faces;
};

class FaceDetector : public ModuleAsync
{
private:
	FaceDetectorParameterStructure m_param;
	static const char * m_type;
	
	//cv::CascadeClassifier m_cascade;
	std::vector<cv::Rect> m_faces;
	
	DetectionThread m_thread;
public:
	FaceDetector(const std::string& x_name, ConfigReader& x_configReader);
	~FaceDetector(void);
	//void CreateParamWindow();
	
	virtual void LaunchThread(const IplImage * img, const double x_timeSinceLastProcessing);
	virtual void NormalProcess(const IplImage * img, const double x_timeSinceLastProcessing);

protected:
	virtual const QThread & GetRefThread(){return m_thread;};

private:
	inline virtual const FaceDetectorParameterStructure& GetRefParameter() const { return m_param;};

};

#endif

