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

#ifndef SEGMENTER_CONTOUR_H
#define SEGMENTER_CONTOUR_H

#include "Module.h"
#include "StreamObject.h"

class ConfigReader;

class SegmenterContourParameterStructure : public ModuleParameterStructure
{
public:
	SegmenterContourParameterStructure(const ConfigReader& x_confReader) : ModuleParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterInt(   0, "minWidth",  0, 	 PARAM_INT, 0, MAX_WIDTH,  &minWidth,	"Minimal width of an object to segment."));
		m_list.push_back(new ParameterInt(   0, "minHeight", 0, 	 PARAM_INT, 0, MAX_HEIGHT, &minHeight,	"Minimal height of an object to segment."));
		m_list.push_back(new ParameterString(0, "objectLabel", "object", &objectLabel,"Label to be applied to the objects detected by the cascade filter (e.g. face)"));
		m_list.push_back(new ParameterString(0, "features",     "x,y,width,height",      &features,   "List of features to extract, separated with ','"));
		
		ParameterStructure::Init();
	};
	
	std::string objectLabel;
	int minWidth;
	int minHeight;
	std::string features;
};

class SegmenterContour : public Module
{
public:
	SegmenterContour(const ConfigReader& x_configReader);
	~SegmenterContour();
	
	virtual void ProcessFrame();
	void Reset();
	
protected:
	
	// for streams
	cv::Mat * m_input;
	cv::Mat * m_debug;

	static const char * m_type;
	std::vector<Object> m_regions;
	std::vector<std::string> m_featureNames;
	StreamObject* m_outputObjectStream;

private:
	SegmenterContourParameterStructure m_param;
	cv::RNG m_rng;
	inline virtual ModuleParameterStructure& RefParameter() { return m_param;};
	bool m_computeFitEllipse;
	bool m_computeMinRect;
};


#endif
