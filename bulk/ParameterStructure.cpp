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
#include "config.h"
#include "assert.h"

// TODO: This class has a range attr that is not used

namespace mk {
using namespace std;

// Static variables
const string ParameterStructure::className = "ParameterStructure";


ParameterStructure::ParameterStructure(const std::string& x_name):
	Parameter(x_name, "")
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
			throw MkException("Try to add a parameter (or input/output) with an existing name \"" + xr_param->GetName() + "\" in module " + GetName(),  LOC);
	}
	m_list.push_back(xr_param);

	// Directly set the right value
	xr_param->SetValueToDefault();
}

/**
* @brief Set the value from json configuration
*/
void ParameterStructure::LockIfRequired()
{
	for(auto& param : m_list)
	{
		param->LockIfRequired();
	}
}

/**
* @brief Set the value from json configuration
*/
void ParameterStructure::Read(const mkconf& x_config)
{
	if(x_config.find("inputs") != x_config.end())
	{
		for(const auto& inputConf : x_config.at("inputs"))
		{
			if(inputConf.is_object() && inputConf.find("connected") != inputConf.end())
				continue; // the input is connected: do not read
					
			try
			{
				if(!ParameterExists(inputConf.at("name").get<string>()))
					continue;
				Parameter& param = RefParameterByName(inputConf.at("name").get<string>());
				if(!param.IsLocked())
					param.SetValue(inputConf.at("value"), PARAMCONF_JSON);
			}
			catch(std::exception& e)
			{
				throw MkException("Exception while setting parameter " + inputConf.at("name").get<string>() + ": " + string(e.what()), LOC);
			}
		}
	}
	CheckRangeAndThrow();
}

/**
* @brief Save all values and prepare json configuration for writing
*/
void ParameterStructure::Write(mkconf& xr_config) const
{
	for(const auto & elem : m_list)
	{
		if(m_writeAllParamsToConfig || elem->GetConfigurationSource() != PARAMCONF_DEF)
		{
			replaceOrAppendInArray(xr_config["inputs"], "name", elem->GetName())["value"] = elem->GetValue();
		}
	}
	xr_config["name"] = GetName();
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
		if(elem->GetName() == x_name)
		{
			return *elem;
		}
	}

	throw ParameterException("Parameter not found in module " + GetName() + ": " + x_name, LOC);
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

	throw ParameterException("Parameter not found in module " + GetName() + ": " + x_name, LOC);
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
void ParameterStructure::CheckRangeAndThrow(const mkconf& x_config) const
{
	// Check that all parameters in config are related to the module
	for(const auto& inputConf : x_config.at("inputs"))
	{
		try
		{
			GetParameterByName(inputConf.at("name").get<string>());
		}
		catch(ParameterException& e)
		{
			// note: This logs every time we override camera_id or job_id
			//       since the parameters are shared between Manager and Context
			LOG_WARN(m_logger, "Exception in CheckRangeAndThrow: " << e.what());
		}
	}
	CheckRangeAndThrow();
}


/**
* @brief  Check that the parameter values are in range [min;max] and throw an exception if not
*/
void ParameterStructure::CheckRangeAndThrow() const
{
	// Check that each parameter's value is within range
	for(const auto & elem : m_list)
	{
		if(!elem->CheckRange())
		{
			stringstream ss;
			ss<<"Parameter "<<elem->GetName()<<" is out of range: ";
			elem->Print(ss);
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
		if(!elem->IsHidden())
			elem->Print(ss);
	}
	if(m_list.size() > 0)
		LOG_INFO(m_logger, ss.str());
}

void ParameterStructure::SetValue(const mkjson& x_value, ParameterConfigType x_confType)
{
	for(auto& elem : m_list)
	{
		elem->SetValue(x_value.at(elem->GetName()), x_confType);
	}
}

void ParameterStructure::SetDefault(const mkjson& x_value)
{
	for(auto& elem : m_list)
	{
		elem->SetDefault(x_value.at(elem->GetName()));
	}
}

bool ParameterStructure::CheckRange() const
{
	for(const auto& elem : m_list)
	{
		if(!elem->CheckRange())
			return false;
	}
	return true;
}

mkjson ParameterStructure::GenerateValues(int x_nbSamples) const
{
	mkjson values;
	for(const auto& elem1 : m_list)
	{
		mkjson allValues = elem1->GenerateValues(x_nbSamples);
		mkjson value;
		for(const auto& av : allValues)
		{
			for(const auto& elem2 : m_list)
			{
				value[elem2->GetName()] = elem2->GetName() == elem1->GetName() ? av : elem2->GetDefault();
			}
		}
		values.push_back(value);
	}
	return values;
}

mkjson ParameterStructure::GetValue() const
{
	mkjson js;
	for(const auto& elem : m_list)
	{
		js[elem->GetName()] = elem->GetValue();
	}
	return js;
}

mkjson ParameterStructure::GetDefault() const
{
	mkjson js;
	for(const auto& elem : m_list)
	{
		js[elem->GetName()] = elem->GetDefault();
	}
	return js;
}

} // namespace mk
