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

#ifndef PARAMETER_H
#define PARAMETER_H

#include "define.h"
#include "MkException.h"
#include <log4cxx/logger.h>

enum ParameterType
{
	PARAM_INT,
	PARAM_UINT,
	PARAM_FLOAT,
	PARAM_DOUBLE,
	PARAM_BOOL,
	PARAM_STR,
	PARAM_POLYGON,
	// PARAM_SERIALIZABLE,
	// PARAM_OBJECT_HEIGHT,
	PARAM_ENUM,
	PARAM_STREAM_OBJECTS,
	PARAM_STREAM_IMAGE,
	PARAM_STREAM_EVENT,
	PARAM_STREAM_STATE,
	PARAM_UNKNOWN,
	PARAM_SIZE
};

enum ParameterConfigType
{
	PARAMCONF_UNSET = 0,
	PARAMCONF_DEF   = 1,
	PARAMCONF_JSON,
	PARAMCONF_GUI,
	PARAMCONF_CMD,
	PARAMCONF_UNKNOWN,
	PARAMCONF_SIZE
};

/// Class representing a generic parameter for use in a module
class Parameter
{
public:
	static const char configType[PARAMCONF_SIZE][16];
	// friend void to_json(mkjson& _json, const Parameter& _ser);
	// friend void from_json(const mkjson& _json, Parameter& _ser);

	Parameter(const std::string& x_name, const std::string& x_description):
		m_name(x_name),
		m_description(x_description),
		m_range(mkjson::object())
	{}
	virtual ~Parameter() {}

	virtual void SetValue(const mkjson& x_value, ParameterConfigType x_confType) = 0;
	virtual void SetDefault(const mkjson& x_value) = 0;
	inline void SetDefaultAndValue(const mkjson& x_value){SetDefault(x_value); SetValueToDefault();}
	inline const std::string& GetName() const {return m_name;}
	virtual const ParameterType& GetParameterType() const = 0;
	virtual const std::string& GetType() const = 0;
	inline const std::string& GetDescription() const {return m_description;}
	inline const ParameterConfigType& GetConfigurationSource() const {return m_confSource;}
	virtual void SetValueToDefault() = 0;
	virtual void Print(std::ostream& os) const;
	virtual bool CheckRange() const = 0;
	virtual mkjson GenerateValues(int x_nbSamples, const mkjson& x_range) const = 0;
	virtual void Export(std::ostream& rx_os) const;

	/// Use this method to mark parameters that must not change value after initialization
	inline void Lock() {m_requiresLock = true;}
	/// Lock the parameter if required
	inline void LockIfRequired() {if(m_requiresLock) m_isLocked = true;}
	inline bool IsLocked() const {return m_isLocked;}
	/// Mark the parameter as hidden (e.g. passwords)
	inline void Hide() {m_isHidden = true;}
	inline bool IsHidden() const {return m_isHidden;}

	// For controllers and actions
	virtual mkjson GetValue() const = 0;
	virtual mkjson GetDefault() const = 0;
	inline mkjson GetRange() const{return m_range;}
	void SetRange(const mkjson& x_range);

protected:
	ParameterConfigType m_confSource = PARAMCONF_UNSET;
	mkjson m_range;

private:
	bool m_isLocked       = false;
	const std::string m_name;
	const std::string m_description;
	bool m_requiresLock   = false;
	bool m_isHidden       = false;

	static log4cxx::LoggerPtr m_logger;
};

#endif
