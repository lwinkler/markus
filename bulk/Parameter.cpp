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

#include "Parameter.h"
#include "ConfigReader.h"
#include "Manager.h"

using namespace std;

// Static variables
const map<string, int>  ParameterImageType::Enum        = ParameterImageType::CreateMap();
const map<int, string>  ParameterImageType::ReverseEnum = ParameterImageType::CreateReverseMap();
log4cxx::LoggerPtr Parameter::m_logger(log4cxx::Logger::getLogger("Parameter"));
log4cxx::LoggerPtr ParameterStructure::m_logger(log4cxx::Logger::getLogger("ParameterStructure"));


template<> const ParameterType ParameterT<bool>::m_type   = PARAM_BOOL;
template<> const ParameterType ParameterT<int>::m_type    = PARAM_INT;
template<> const ParameterType ParameterT<float>::m_type  = PARAM_FLOAT;
template<> const ParameterType ParameterT<double>::m_type = PARAM_DOUBLE;

template<> const string ParameterT<bool>::m_typeStr   = "bool";
template<> const string ParameterT<int>::m_typeStr    = "int";
template<> const string ParameterT<float>::m_typeStr  = "float";
template<> const string ParameterT<double>::m_typeStr = "double";

CalibrationByHeight ParameterCalibrationByHeight::DefaultFg = CalibrationByHeight(0.2,0.9,0.3);
CalibrationByHeight ParameterCalibrationByHeight::DefaultBg = CalibrationByHeight(0.1,0.4,0.1);


ParameterStructure::ParameterStructure(const ConfigReader& x_configReader):
	m_configReader(x_configReader.GetSubConfig("parameters")),
	m_moduleName(x_configReader.GetAttribute("name"))
{
	m_writeAllParamsToConfig = false;
}

ParameterStructure::~ParameterStructure()
{
	for(vector<Parameter* >::iterator it = m_list.begin() ; it != m_list.end() ; it++)
		delete(*it);
	m_list.clear();
}

/**
* @brief Initialize the parameter structure with the value from default or xml configuration
*/
void ParameterStructure::Init()
{
	// Read config file
	SetValueToDefault();
	
	// Read parameters from config
	SetFromConfig();
	
	// LOG_INFO("Parameters for "<<m_moduleName<<" initialized.");
	// PrintParameters(); // Global::log.stream(LOG_INFO));
	CheckRange(false);
}

/**
* @brief Set the value from xml configuration
*/
void ParameterStructure::SetFromConfig()
{
	ConfigReader conf = m_configReader; // .GetSubConfig("parameters");
	if(conf.IsEmpty())
		return;
	conf = conf.GetSubConfig("param");
	while(!conf.IsEmpty())
	{
		string name = conf.GetAttribute("name");
		string value = conf.GetValue();
		//SetValueByName(name, value, PARAMCONF_XML);

		try
		{
			Parameter& param = RefParameterByName(name);
			if(!param.IsLocked())
				param.SetValue(value, PARAMCONF_XML);
		}
		catch(ParameterException& e)
		{
			// note: do not output a warning, unused parameters are checked inside CheckRange
			// LOG_WARN(m_logger, "Unknown parameter in configuration: "<<name<<" in module "<<m_moduleName);
		}
		conf = conf.NextSubConfig("param");
	}
}

/**
* @brief Save all values and prepare xml configuration for writing
*/
void ParameterStructure::UpdateConfig() const
{
	// assert(!m_configReader.IsEmpty());
	ConfigReader conf = m_configReader;
	
	for(vector<Parameter*>::const_iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		if(m_writeAllParamsToConfig || (*it)->GetConfigurationSource() != PARAMCONF_DEF)
		{
			conf.RefSubConfig("param", (*it)->GetName(), true).SetValue((*it)->GetValueString());
		}
	}
}

/**
* @brief Get the parameter by name
*
* @param x_name Name
*
* @return parameter
*/
const Parameter& ParameterStructure::GetParameterByName(const string& x_name) const
{
	for(vector<Parameter*>::const_iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		if((*it)->GetName().compare(x_name) == 0)
		{
			return **it;
		}
	}
	
	throw ParameterException("Parameter not found in module " + m_moduleName + ": " + x_name, LOC);
}

/**
* @brief Get the reference to a parameter by name
*
* @param x_name Name
*
* @return parameter
*/
Parameter& ParameterStructure::RefParameterByName(const string& x_name)
{
	for(vector<Parameter*>::iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		if((*it)->GetName().compare(x_name) == 0)
		{
			return **it;
		}
	}
	
	throw ParameterException("Parameter not found in module " + m_moduleName + ": " + x_name, LOC);
}


/**
* @brief Set the value of the parameter to default
*/
void ParameterStructure::SetValueToDefault()
{
	for(vector<Parameter*>::iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		if(!(*it)->IsLocked())
			(*it)->SetValueToDefault();
	}
}

/**
* @brief  Check that the parameter values are in range [min;max] and throw an exception if not
*
* @param x_checkRelated Check that all parameters in config are related to the module
*/
void ParameterStructure::CheckRange(bool x_checkRelated) const
{
	if(x_checkRelated)
	{
		// Check that all parameters in config are related to the module
		ConfigReader conf = m_configReader; // .GetSubConfig("parameters");
		if(conf.IsEmpty())
			return;
		conf = conf.GetSubConfig("param");
		while(!conf.IsEmpty())
		{
			string name = conf.GetAttribute("name");

			try
			{
				GetParameterByName(name);
			}
			catch(ParameterException& e)
			{
				LOG_WARN(m_logger, e.what());
			}
			conf = conf.NextSubConfig("param");
		}
	}


	// Check that each parameter's value is within range
	for(vector<Parameter*>::const_iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		if(!(*it)->CheckRange())
		{
			stringstream ss;
			ss<<"Parameter "<<(*it)->GetName()<<" is out of range: ";
			(*it)->Print(ss);
			throw ParameterException(ss.str(), LOC);
		}
	}
}

/**
* @brief Print parameters to logs with details
*/
void ParameterStructure::PrintParameters() const
{
	stringstream ss;
	// string confType = "";
	for(vector<Parameter*>::const_iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		(*it)->Print(ss);
	}
	if(m_list.size() > 0)
		LOG_INFO(m_logger, ss.str());
}


/**
* @brief Export the parameter for module description
*
* @param rx_os         Output stream
* @param x_indentation Number of tabs for indentation
*/
void ParameterEnum::Export(ostream& rx_os, int x_indentation)
{
	string tabs(x_indentation, '\t');
	rx_os<<tabs<<"<param name=\""<<m_name<<"\">"<<endl;
	tabs = string(x_indentation + 1, '\t');
	rx_os<<tabs<<"<type>"<<GetTypeString()<<"</type>"<<endl;
	rx_os<<tabs<<"<value default=\""<<GetReverseEnum().at(m_default)<<"\">"<<GetReverseEnum().at(GetValue())<<"</value>"<<endl;
	rx_os<<tabs<<"<description>"<<m_description<<"</description>"<<endl;
	tabs = string(x_indentation, '\t');
	rx_os<<tabs<<"</param>"<<endl;
}

/**
* @brief Static function used for map initialization
*
* @return map
*/
map<string,int> ParameterImageType::CreateMap()
{
	map<string, int> map1;
	map1["CV_8UC1"] = CV_8UC1;
	map1["CV_8UC2"] = CV_8UC2;
	map1["CV_8UC3"] = CV_8UC3;
	//map1["CV_8UC(n)"] = CV_8UC(n);
	map1["CV_8SC1"] = CV_8SC1;
	map1["CV_8SC2"] = CV_8SC2;
	map1["CV_8SC3"] = CV_8SC3;
	map1["CV_8SC4"] = CV_8SC4;
	//map1["CV_8SC(n)"] = CV_8SC(n);
	map1["CV_16UC1"] = CV_16UC1;
	map1["CV_16UC2"] = CV_16UC2;
	map1["CV_16UC3"] = CV_16UC3;
	map1["CV_16UC4"] = CV_16UC4;
	//map1["CV_16UC(n)"] = CV_16UC(n);
	map1["CV_16SC1"] = CV_16SC1;
	map1["CV_16SC2"] = CV_16SC2;
	map1["CV_16SC3"] = CV_16SC3;
	map1["CV_16SC4"] = CV_16SC4;
	//map1["CV_16SC(n)"] = ;
	map1["CV_32SC1"] = CV_32SC1;
	map1["CV_32SC2"] = CV_32SC2;
	map1["CV_32SC3"] = CV_32SC3;
	map1["CV_32SC4"] = CV_32SC4;
	//map1["CV_32SC1"] = CV_32SC1;
	map1["CV_32FC1"] = CV_32FC1;
	map1["CV_32FC2"] = CV_32FC2;
	map1["CV_32FC3"] = CV_32FC3;
	map1["CV_32FC4"] = CV_32FC4;
	//map1["CV_32FC1"] = CV_32FC1;
	map1["CV_64FC1"] = CV_64FC1;
	map1["CV_64FC2"] = CV_64FC2;
	map1["CV_64FC3"] = CV_64FC3;
	map1["CV_64FC4"] = CV_64FC4;
	//map1["CV_64FC1"] = CV_64FC1;

	return map1;
}

/**
* @brief Create a reverse map. Used internally
*
* @return map
*/
map<int,string> ParameterImageType::CreateReverseMap()
{
	const map<string, int> map1 = CreateMap();
	map<int, string> out;
	for(map<string, int>::const_iterator it = map1.begin() ; it != map1.end() ; it++)
		out[it->second] = it->first;

	return out;
}

ParameterImageType::ParameterImageType(const string& x_name, int x_default, int * xp_value, const string x_description) :
	ParameterEnum(x_name, x_default, xp_value, x_description)
{
}

/**
* @brief Set the value of the parameter
*
* @param rx_value   String containing the value
* @param x_confType Source of the configuration (default, xml, GUI, ...)
*/
void ParameterEnum::SetValue(const string& rx_value, ParameterConfigType x_confType)
{
	if(m_isLocked) 
		throw ParameterException("You tried to set the value of a locked parameter.", LOC);
	*mp_value = GetEnum().at(rx_value);
	m_confSource = x_confType;
}

/**
* @brief Set the value of the parameter
*
* @param rx_value   Integer containing the value
* @param x_confType Source of the configuration (default, xml, GUI, ...)
*/
void ParameterEnum::SetValue(int rx_value, ParameterConfigType x_confType)
{
	if(m_isLocked) 
		throw ParameterException("You tried to set the value of a locked parameter.", LOC);
	*mp_value = rx_value;
	m_confSource = x_confType;
}


/**
* @brief Set the default value 
*
* @param rx_value Value to set
*/
void ParameterEnum::SetDefault(const string& rx_value)
{
	m_default = GetEnum().at(rx_value);
}

/**
* @brief Check that the parameter value is in range
*
* @return true if valid
*/
bool ParameterEnum::CheckRange() const
{
	return true;
}

/**
* @brief Print the parameter with default value and range
*
* @param os Output stream
*/
void ParameterEnum::Print(ostream& os) const
{
	os<<m_name<<" = "<<GetReverseEnum().at(GetValue())<<" ["<<GetValue()<<"] ("<<configType[m_confSource]<<"); ";
}

