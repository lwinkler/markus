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

#include "ParameterStructure.h"

using namespace std;

// Static variables
log4cxx::LoggerPtr ParameterStructure::m_logger(log4cxx::Logger::getLogger("ParameterStructure"));




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
