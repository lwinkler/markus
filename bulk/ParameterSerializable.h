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
	ParameterSerializable(const std::string& x_name, const std::string& x_default, Serializable * xp_value, const std::string& x_description) :
		Parameter(x_name, x_description),
		m_default(x_default),
		mr_value(*xp_value)
	{
	}
	MKPARAMTYPE(PARAM_SERIALIZABLE)
	MKTYPE("serializable")

	inline std::string GetValueString() const override
	{
		return mr_value.SerializeToString();
	}
	inline std::string GetDefaultString() const override {return m_default;}
	inline std::string GetRange() const override {return "";}
	inline virtual void SetRange(const std::string& x_range) override {}
	// inline const Serializable& GetDefault() const {	return m_default;}

	inline void SetValue(const Serializable& x_value, ParameterConfigType x_confType)
	{
		if(m_isLocked)
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		mr_value = x_value;
		m_confSource = x_confType;
	}

	virtual void SetValue(const std::string& rx_value, ParameterConfigType x_confType /*= PARAMCONF_UNKNOWN*/) override
	{
		if(m_isLocked)
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		std::istringstream istr(rx_value);
		if(rx_value == "")	// This case happens with unit testing
		{
			LOG_WARN(m_logger, "Serializable parameter is set to empty string value");
			m_confSource = x_confType;
			return;
		}
		mr_value.Deserialize(istr, "");
		m_confSource = x_confType;
	}
	virtual void SetDefault(const std::string& rx_value) override
	{
		m_default = rx_value;
	}
	const Serializable& GetValue() const
	{
		return mr_value;
	}
	virtual bool CheckRange() const override
	{
		// Note: so far it is not possible to check the range of serializable params
		return true;
	}
	virtual void GenerateValues(int x_nbSamples, std::vector<std::string>& rx_values, const std::string& x_range = "") const override;
	virtual void SetValueToDefault() override
	{
		if(m_isLocked)
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		std::stringstream ss;
		ss << m_default;
		mr_value.Deserialize(ss, "");
		m_confSource = PARAMCONF_DEF;
	}

private:
	static log4cxx::LoggerPtr m_logger;
	std::string m_default;
	Serializable& mr_value;
};


#endif
