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

#ifndef PARAMETER_STRUCTURE_H
#define PARAMETER_STRUCTURE_H

#include "define.h"
#include "Parameter.h"
#include "MkException.h"
#include "config.h"
#include <log4cxx/logger.h>

namespace mk {
/// Represents a set of parameters for a configurable objects
// Note: Disable copies of parameters as a safety
class ParameterStructure : public Parameter
{
public:
	explicit ParameterStructure(const std::string& x_name);
	virtual ~ParameterStructure();
	void Read(const mkconf& x_config);
	void Write(mkconf& xr_config) const;
	void SetValueToDefault();
	virtual void CheckRangeAndThrow() const;
	void CheckRangeAndThrow(const mkconf& x_config) const;
	void PrintParameters() const;
	//void SetValueByName(const std::string& x_name, const std::string& x_value, ParameterConfigType x_configType = PARAMCONF_UNKNOWN);
	const Parameter & GetParameterByName(const std::string& x_name) const;
	const std::vector<Parameter*>& GetList() const {return m_list;}
	void LockParameterByName(const std::string& x_name) {RefParameterByName(x_name).Lock();}
	void LockIfRequired();
	bool ParameterExists(const std::string& x_name) const;
	void AddParameter(Parameter* xr_param);

	// inherited from parameter
	void SetValue(const mkjson& x_value, ParameterConfigType x_confType) override;
	void SetDefault(const mkjson& x_value) override;
	inline const std::string& GetClass() const override {return className;}
	bool CheckRange() const override;
	mkjson GenerateValues(int x_nbSamples, const mkjson& x_range) const override;
	mkjson GetValue() const override;
	mkjson GetDefault() const override;

	static const std::string className;

protected:
	Parameter & RefParameterByName(const std::string& x_name);
	bool m_writeAllParamsToConfig;

private:
	std::vector<Parameter*> m_list;
};


} // namespace mk
#endif
