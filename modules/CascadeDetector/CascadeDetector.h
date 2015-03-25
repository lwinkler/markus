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

#include <QThread>

#include "ModuleAsync.h"
#include "Parameter.h"
#include "StreamObject.h"

/*! \class CascadeDetector
 *  \brief Module class for detection based on cascade filters (Haar, ...)
 *
 */



class DetectionThread : public QThread
{
public:
	explicit DetectionThread(QObject* parent = 0)
	{
		m_timerThread = 0;
		m_scaleFactor = 0;
		m_minNeighbors = 0;
		m_minSide = 0;
	}
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
	const std::vector<cv::Rect>& GetDetectedObjects() const { return m_detected;}
	long long m_timerThread;

protected:
	float m_scaleFactor;
	cv::Mat m_smallImg;
	int m_minNeighbors;
	int m_minSide;

	std::vector<cv::Rect> m_detected;
};

/**
* @brief Detect objects from a video stream using a cascade filter (c.f. Haar patterns)
*/
class CascadeDetector : public ModuleAsync
{
public:
	class Parameters : public ModuleAsync::Parameters
	{

	public:
		Parameters(const ConfigReader& x_confReader) :
			ModuleAsync::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterInt("min_neighbors", 2, 1, 100, 	&minNeighbors,	"Minimum numbers of neighbors (higher: less sensitive)")); // Note : Seems to be a bug with minNeighbors = 1 with most filters
			m_list.push_back(new ParameterInt("min_side", 0, 0, 200, 		&minSide,	"Minimum size of the detected object"));
			m_list.push_back(new ParameterFloat("scale_factor", 1.2, 1, 2, 	&scaleFactor,	"Scale factor for scanning (higher: less sensitive)"));
			m_list.push_back(new ParameterString("filter_file", "modules/CascadeDetector/lbpcascade_frontalface.xml",  &filterFile,
												 "File with filter data of the detected object"));
			// m_list.push_back(new ParameterString("color", "(255,255,255)",		&color,	"Color to draw the output"));
			m_list.push_back(new ParameterString("object_label", "casc", 			&objectLabel,	"Label to be applied to the objects detected by the cascade filter (e.g. face)"));

			RefParameterByName("type").SetRange("[CV_8UC1]");
			RefParameterByName("type").SetDefault("CV_8UC1");
			// Limit size to accelerate unit tests
			RefParameterByName("width").SetRange("[1:1280]");
			RefParameterByName("height").SetRange("[1:960]");

			Init();
		}

		int minNeighbors;
		int minSide;
		float scaleFactor;
		std::string filterFile;
		std::string objectLabel;
	};

	CascadeDetector(ParameterStructure& xr_params);
	~CascadeDetector(void);
	MKCLASS("CascadeDetector")
	MKDESCR("Detect objects from a video stream using a cascade filter (c.f. Haar patterns)")

	inline virtual const Parameters& GetParameters() const { return m_param;}
	virtual void LaunchThread();
	virtual void NormalProcess();
	virtual void CopyResults();
	void Reset();

private:
	Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	// input
	cv::Mat m_input;
	cv::Mat m_lastInput;

	// output
	std::vector<Object> m_detectedObjects;

	// debug
#ifdef MARKUS_DEBUG_STREAMS
	cv::Mat m_debug;
#endif

	// thread
	DetectionThread m_thread;
	virtual const QThread & GetRefThread() {return m_thread;}
};

#endif

