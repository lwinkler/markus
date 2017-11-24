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

#ifndef PARAMETER_SERIALIZABLE_H
#define PARAMETER_SERIALIZABLE_H

#include "define.h"
#include "Parameter.h"
#include "Serializable.h"
#include <log4cxx/logger.h>

/// Parameter for a variable of non-primary type but that can be serialized and deserialized to a JSON string
class ParameterSerializable : public Parameter
{
public:
	ParameterSerializable(const std::string& x_name, const ConfigReader& x_default, mkjson * xp_value, const std::string& x_description) :
		Parameter(x_name, x_description),
		m_default(x_default),
		mr_value(*xp_value)
	{
	}
	MKPARAMTYPE(PARAM_SERIALIZABLE)
	MKTYPE("serializable")

	inline ConfigReader GetValue() const override
	{
		// TODO LW: Simplify: return val
		std::stringstream ss;
		ss << mr_value;
		Json::Value val;
		ss >> val;
		return val;
	}
	inline ConfigReader GetDefault() const override {return m_default;}

	inline void SetValue(const mkjson& x_value, ParameterConfigType x_confType)
	{
		if(IsLocked())
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		mr_value = x_value;
		m_confSource = x_confType;
	}

	void SetValue(const ConfigReader& rx_value, ParameterConfigType x_confType /*= PARAMCONF_UNKNOWN*/) override;
	void SetDefault(const ConfigReader& rx_value) override
	{
		m_default = rx_value;
	}
	bool CheckRange() const override
	{
		// Note: so far it is not possible to check the range of serializable params
		return true;
	}
	Json::Value GenerateValues(int x_nbSamples, const Json::Value& x_range) const override;
	void SetValueToDefault() override;

private:
	static log4cxx::LoggerPtr m_logger;
	ConfigReader m_default;
	mkjson& mr_value;
};


#endif
