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
#include "assert.h"

using namespace std;

// Static variables
log4cxx::LoggerPtr ParameterStructure::m_logger(log4cxx::Logger::getLogger("ParameterStructure"));




ParameterStructure::ParameterStructure(const ConfigReader& x_configReader):
	m_name(x_configReader.GetAttribute("name", "unnamed"))
{
	m_writeAllParamsToConfig = false;
}

ParameterStructure::~ParameterStructure()
{
	for(auto & elem : m_list)
		delete(elem);
	m_list.clear();
}

/**
* @brief Add a parameter to the structure
*/
void ParameterStructure::AddParameter(Parameter* xr_param)
{
	for(const auto elem : m_list)
	{
		if(elem->GetName() == xr_param->GetName())
			throw MkException("Try to add a parameter (or input/output) with an existing name \"" + xr_param->GetName() + "\" in module " + m_name,  LOC);
	}
	m_list.push_back(xr_param);

	// Directly set the right value
	xr_param->SetValueToDefault();
	/*
	if(m_configReader.GetSubConfig("parameters").IsEmpty())
	{
		LOG_WARN(m_logger, "No <parameters/> structure in configuration of " << m_configReader.GetAttribute("name"));
	}
	else
	{
		ConfigReader conf(m_configReader.Find("parameters>param[name=\"" + xr_param->GetName() + "\"]"));
		if(!conf.IsEmpty())
		{
			string value = conf.GetValue();
			xr_param->SetValue(value, PARAMCONF_XML);
		}
	}
	if(!xr_param->CheckRange())
	{
		stringstream ss;
		ss<<"Parameter "<<xr_param->GetName()<<" is out of range: ";
		xr_param->Print(ss);
		throw ParameterException(ss.str(), LOC);
	}
	*/
}

/**
* @brief Add a parameter to the structure
*/
// TODO: This method should in time disappear
void ParameterStructure::AddParameterForStream(Parameter* xr_param)
{
	AddParameter(xr_param);
}

/**
* @brief Set the value from xml configuration
*/
void ParameterStructure::LockIfRequired()
{
	for(auto& param : m_list)
	{
		param->LockIfRequired();
	}
}

/**
* @brief Set the value from xml configuration
*/
void ParameterStructure::Read(const ConfigReader& x_config)
{
	for(const auto& conf : x_config.FindAll("parameters>param"))
	{
		string name  = conf.GetAttribute("name");
		string value = conf.GetValue();

		try
		{
			if(!ParameterExists(name))
				continue;
			Parameter& param = RefParameterByName(name);
			if(!param.IsLocked())
				param.SetValue(value, PARAMCONF_XML);
		}
		catch(std::exception& e)
		{
			throw MkException("Exception while setting parameter " + name + ": " + string(e.what()), LOC);
		}
	}
	CheckRange();
}

/**
* @brief Save all values and prepare xml configuration for writing
*/
void ParameterStructure::Write(ConfigReader xr_config) const
{
	ConfigReader conf = xr_config.Find("parameters");

	for(const auto & elem : m_list)
	{
		if(m_writeAllParamsToConfig || (elem)->GetConfigurationSource() != PARAMCONF_DEF)
		{
			conf.FindRef("param[name=\"" +(elem)->GetName() + "\"]", true).SetValue((elem)->GetValueString());
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
	for(const auto & elem : m_list)
	{
		if((elem)->GetName().compare(x_name) == 0)
		{
			return *elem;
		}
	}

	throw ParameterException("Parameter not found in module " + m_name + ": " + x_name, LOC);
}

/**
* @brief Check if the parameter exists in the structure
*
* @param x_name Name
*
* @return true if exists
*/
bool ParameterStructure::ParameterExists(const string& x_name) const
{
	for(const auto & elem : m_list)
	{
		if(elem->GetName() == x_name)
		{
			return true;
		}
	}
	return false;
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
	for(const auto & elem : m_list)
	{
		if(elem->GetName() == x_name)
		{
			return *elem;
		}
	}

	throw ParameterException("Parameter not found in module " + m_name + ": " + x_name, LOC);
}


/**
* @brief Set the value of the parameter to default
*/
void ParameterStructure::SetValueToDefault()
{
	for(auto & elem : m_list)
	{
		if(!elem->IsLocked())
			elem->SetValueToDefault();
		else LOG_WARN(m_logger, "Trying to set value of locked parameter " << elem->GetName() << " to default");
	}
}

/**
* @brief  Check that the parameter values are in range [min;max] and throw an exception if not
*
* @param x_config Config to check
*/
void ParameterStructure::CheckRange(const ConfigReader& x_config) const
{
	// Check that all parameters in config are related to the module
	// if(m_configReader.Find("parameters").IsEmpty())
	// return;
	for(const auto& conf : x_config.FindAll("parameters>param"))
	{
		string name = conf.GetAttribute("name");
		try
		{
			GetParameterByName(name);
		}
		catch(ParameterException& e)
		{
			// note: This logs every time we override camera_id or job_id
			//       since the parameters are shared between Manager and Context
			LOG_WARN(m_logger, "Exception in CheckRange: " << e.what());
		}
	}
	CheckRange();
}


/**
* @brief  Check that the parameter values are in range [min;max] and throw an exception if not
*/
void ParameterStructure::CheckRange() const
{
	// Check that each parameter's value is within range
	for(const auto & elem : m_list)
	{
		if(!(elem)->CheckRange())
		{
			stringstream ss;
			ss<<"Parameter "<<(elem)->GetName()<<" is out of range: ";
			(elem)->Print(ss);
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
	for(const auto & elem : m_list)
	{
		if(!(elem)->IsHidden())
			(elem)->Print(ss);
	}
	if(m_list.size() > 0)
		LOG_INFO(m_logger, ss.str());
}

