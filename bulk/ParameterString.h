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

#ifndef PARAMETER_STRING_H
#define PARAMETER_STRING_H

#include "define.h"
#include "Parameter.h"
#include <log4cxx/logger.h>

/// Parameter of type string
class ParameterString : public Parameter
{
public:
	ParameterString(const std::string& x_name, std::string x_default, std::string* xp_value, const std::string& x_description) :
		Parameter(x_name, x_description),
		m_default(x_default),
		mr_value(*xp_value) {}
	MKPARAMTYPE(PARAM_STR)
	MKTYPE("string")

	virtual void SetValue(const ConfigReader& rx_value, ParameterConfigType x_confType) override
	{
		if(IsLocked())
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		mr_value = rx_value.asString();
		m_confSource = x_confType;
	}
	virtual void SetDefault(const ConfigReader& x_value) override
	{
		m_default = x_value.asString();
	}
	inline virtual ConfigReader GetValue() const override {return mr_value;}
	inline virtual ConfigReader GetDefault() const override {return m_default;}
	inline virtual bool CheckRange() const override
	{
		return true;
	}
	virtual Json::Value GenerateValues(int x_nbSamples, const Json::Value& x_range) const override;
	virtual void SetValueToDefault() override
	{
		if(IsLocked())
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		mr_value = m_default;
		m_confSource = PARAMCONF_DEF;
	}

	inline const std::string& GetDefault() {return m_default;}
private:
	std::string m_default;
	std::string& mr_value;
	static log4cxx::LoggerPtr m_logger;
};

#endif
