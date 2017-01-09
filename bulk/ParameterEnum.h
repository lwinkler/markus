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

#ifndef PARAMETER_ENUM_H
#define PARAMETER_ENUM_H

#include "define.h"
#include "Parameter.h"
#include "MkException.h"
#include <log4cxx/logger.h>

/// Parameter of type enum
class ParameterEnum : public Parameter
{
public:
	ParameterEnum(const std::string& x_name, int x_default, int * xp_value, const std::string& x_description):
		Parameter(x_name, x_description),
		m_default(x_default),
		mr_value(*xp_value)
	{}
	
	MKPARAMTYPE(PARAM_ENUM)

	void SetValue(const ConfigReader& rx_value, ParameterConfigType x_confType) override;
	void SetDefault(const ConfigReader& rx_value) override;
	inline ConfigReader GetValue() const override {return GetReverseEnum().at(mr_value);}
	inline ConfigReader GetDefault() const override {return GetReverseEnum().at(m_default);}
	bool CheckRange() const override;
	Json::Value GenerateValues(int x_nbSamples, const Json::Value& x_range) const override;
	void Print(std::ostream& os) const override;
	void SetValueToDefault() override
	{
		if(IsLocked())
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		mr_value = m_default;
		m_confSource = PARAMCONF_DEF;
	}
	virtual void Export(std::ostream& rx_os) const override;
	const std::string& GetType() const override = 0;
	virtual const std::map<std::string, int>& GetEnum() const = 0;
	virtual const std::map<int, std::string>& GetReverseEnum() const = 0;

protected:
	static std::map<int, std::string> CreateReverseMap(const std::map<std::string, int>& x_map);

	int m_default;
	int& mr_value;
private:
	static log4cxx::LoggerPtr m_logger;
};

#endif
