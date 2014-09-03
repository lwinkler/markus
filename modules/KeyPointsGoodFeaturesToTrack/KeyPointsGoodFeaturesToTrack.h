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

#ifndef KEYPOINTS_GOODFEATURESTOTRACK_H
#define KEYPOINTS_GOODFEATURESTOTRACK_H

#include "Module.h"
#include "StreamObject.h"



/**
* @brief Extract different types of keypoints
*/
class KeyPointsGoodFeaturesToTrack : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : Module::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterInt("max_corners", 1000, 0, INT_MAX, &maxCorners, "Maximum number of corners to return. If there are more corners than are found, the strongest of them is returned."));
			m_list.push_back(new ParameterDouble("quality_level", 0.01, 0, 1, &qualityLevel,"Parameter characterizing the minimal accepted quality of image corners."));
			m_list.push_back(new ParameterDouble("min_dist", 1, 0, MAX_WIDTH, &minDistance,"Minimum possible Euclidean distance between the returned corners"));
			m_list.push_back(new ParameterInt("block_size", 3, 1, MAX_WIDTH, &blockSize,"Size of an average block for computing a derivative covariation matrix over each pixel neighborhood"));
			m_list.push_back(new ParameterBool("use_harris_detector", 0, 0, 1, &useHarrisDetector,"Parameter indicating whether to use a Harris detector"));
			m_list.push_back(new ParameterDouble("k", 0.04, 0, 1, &k,"Free parameter of the Harris detector"));

			// RefParameterByName("type").SetDefault("CV_8UC3");
			// RefParameterByName("type").SetRange("[CV_8UC1,CV_8UC3]");

			Init();
		};
		int maxCorners;
		double qualityLevel;
		double minDistance;
		int blockSize;
		bool useHarrisDetector;
		double k;
	};

	KeyPointsGoodFeaturesToTrack(const ConfigReader& x_configReader);
	~KeyPointsGoodFeaturesToTrack();
	MKCLASS("KeyPointsGoodFeaturesToTrack")
	MKDESCR("Extract different types of keypoints from image")
	
	inline virtual const Parameters& GetParameters() const { return m_param;}
	virtual void ProcessFrame();
	void Reset();

private:
	inline virtual Parameters & RefParameters() {return m_param;}
	Parameters m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	// input
	cv::Mat m_input;
	std::vector <Object> m_objectsIn;

	// output
	std::vector <Object> m_objectsOut;
	//std::vector <Object> m_objectsKeyPoints;


	// state variables
	cv::FeatureDetector *m_detector;

#ifdef MARKUS_DEBUG_STREAMS
	//cv::RNG m_rng;
	cv::Mat m_debug;
#endif
};


#endif
