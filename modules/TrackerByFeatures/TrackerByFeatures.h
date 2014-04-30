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

#ifndef TRACKER_BY_FEATURES_H
#define TRACKER_BY_FEATURES_H

#include "list"
#include "Module.h"
#include "StreamObject.h"
#include "Template.h"

class ConfigReader;

class TrackerByFeaturesParameterStructure : public ModuleParameterStructure
{
public:
	TrackerByFeaturesParameterStructure(const ConfigReader& x_confReader) : ModuleParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterDouble("max_matching_distance"   , 0.1 , 0    , 100    , &maxMatchingDistance     , "Tolerance of the tracker."));
		m_list.push_back(new ParameterDouble("time_disappear"          , 1.0 , 0    , 300    , &timeDisappear           , "Time before disappearence of an object [s]"));
		m_list.push_back(new ParameterBool  ("symetric_match"          , true, 0    , 1      , &symetricMatch           , "Each match between objects and templates must be symetrical"));
		m_list.push_back(new ParameterString("features"                , "x,y,width,height"                , &features                , "List of features to use for tracking (only scalar values, must be present in objects to track)"));
		m_list.push_back(new ParameterDouble("alpha"                   , 0.01, 0    , 1      , &alpha                   , "Alpha for feature update, used to set the mean value dynamically. Sets the adaptibility of the tracker and is used to calculate mean and variation features."));
		m_list.push_back(new ParameterBool  ("handle_split"            , 0   , 0    , 1      , &handleSplit             , "Handle the splitting of one object into multiple objects."));


		ParameterStructure::Init();
	}
	double maxMatchingDistance;
	double timeDisappear;
	bool symetricMatch;
	double alpha;
	bool handleSplit;
	std::string features;
};

/**
* @brief Track objects by matching a set of features (typically x,y,width andci height)
*/
class TrackerByFeatures : public Module
{
public:
	TrackerByFeatures(const ConfigReader& x_configReader);
	~TrackerByFeatures();
	virtual const std::string& GetDescription() const {static std::string descr = "Track objects by matching a set of features (typically x,y,width andci height)"; return descr;}
	
	virtual void ProcessFrame();
	void Reset();
	
private:
	TrackerByFeaturesParameterStructure m_param;
	inline virtual const ModuleParameterStructure& GetParameters() const { return m_param;}
	static log4cxx::LoggerPtr m_logger;
protected:
	void MatchTemplates();
	void CleanTemplates();
	void DetectNewTemplates();
	void UpdateTemplates();
	const Template * MatchObject(const Object& x_obj)const;
	int MatchTemplate(Template& x_temp);

	// input and output
	std::vector <Object> m_objects;

	// state
	std::list <Template> m_templates;

	// temporary
	std::vector <std::string>    m_featureNames;

	// debug
#ifdef MARKUS_DEBUG_STREAMS
	cv::Mat m_debug;
#endif
};


#endif
