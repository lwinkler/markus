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

#ifndef OBJECTTRACKER_H
#define OBJECTTRACKER_H

#include "Module.h"
#include "Detector.h"

class ConfigReader;

class ObjectTrackerParameterStructure : public ModuleParameterStructure
{
public:
	ObjectTrackerParameterStructure(const ConfigReader& x_confReader) : ModuleParameterStructure(x_confReader)
	{
		//m_list.push_back(new ParameterT<int>(0, "input_blur_size",	1, 	PARAM_INT, 1, 7,	&detector.inputBlurSize,	"Strength of the blur applied to the input"));
		m_list.push_back(new ParameterT<float>(0, "background_alpha",	0.02, 	PARAM_FLOAT, 0, 1,	&detector.backgroundAlpha,	"Defines the speed at which the background adapts"));
		m_list.push_back(new ParameterT<float>(0, "foreground_thres", 	0.2, 	PARAM_FLOAT, 0, 1,	&detector.foregroundThres,	"Threshold to accept a pixel as foreground"));
		m_list.push_back(new ParameterT<int>(0, "foreground_filter_size", 3, 	PARAM_INT, 1, 7,	&detector.foregroundFilterSize,	"Size of the filter to remove noise"));

		ParameterStructure::Init();
	};
	DetectorParameter detector;
};

class ObjectTracker : public Module
{
public:
	ObjectTracker(const ConfigReader& x_configReader);
	~ObjectTracker();
	
	virtual void ProcessFrame();
	inline virtual int GetInputWidth() const {return m_param.width;};
	inline virtual int GetInputHeight() const {return m_param.height;};
	
private:
	ObjectTrackerParameterStructure m_param;
	
	cv::Mat * m_input;
	cv::Mat * m_output;
	//cv::Mat* m_img_blur;
	
	Detector detect;
	static const char * m_type;
protected:
	inline virtual const ModuleParameterStructure& GetRefParameter() const { return m_param;};
};


#endif