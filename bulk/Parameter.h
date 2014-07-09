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
#include "CalibrationByHeight.h"
#include <log4cxx/logger.h>

enum ParameterType
{
	PARAM_INT,
	PARAM_FLOAT,
	PARAM_DOUBLE,
	PARAM_BOOL,
	PARAM_STR,
	PARAM_OBJECT_HEIGHT,
	PARAM_IMAGE_TYPE,
	PARAM_GENERIC
};

enum ParameterConfigType
{
	PARAMCONF_UNSET = 0,
	PARAMCONF_DEF   = 1,
	PARAMCONF_XML,
	PARAMCONF_GUI,
	PARAMCONF_CMD,
	PARAMCONF_UNKNOWN,
	PARAMCONF_SIZE
};

static const char configType[PARAMCONF_SIZE][16] = {"unset", "def", "xml", "gui", "cmd", "unk"};

/// Class representing a generic parameter for use in a module
class Parameter
{
public:
	Parameter(const std::string& x_name, const std::string& x_description):
		m_name(x_name),
		m_confSource(PARAMCONF_UNSET),
		m_description(x_description),
		m_isLocked(false),
		m_isHidden(false){}
	virtual ~Parameter(){}
		
	virtual void SetValue(const std::string& x_value, ParameterConfigType x_confType /*= PARAMCONF_UNKNOWN*/) = 0;
	virtual void SetDefault(const std::string& x_value) = 0;
	inline const std::string& GetName() const {return m_name;}
	virtual const ParameterType& GetType() const = 0;
	virtual const std::string& GetTypeString() const = 0;
	inline const std::string& GetDescription() const {return m_description;}
	inline const ParameterConfigType& GetConfigurationSource() const {return m_confSource;}
	virtual void SetValueToDefault() = 0;
	virtual void Print(std::ostream& os) const 
	{
		os<<m_name<<"=\""<<GetValueString()<<"\" ("<<configType[m_confSource]<<"); ";
	}
	virtual bool CheckRange() const = 0;
	virtual void Export(std::ostream& rx_os, int x_indentation) = 0;
	inline void Lock()
	{
		m_isLocked = true;
	}
	inline bool IsLocked() const {return m_isLocked;}
	inline void Hide() {m_isHidden = true;}
	inline bool IsHidden() const {return m_isHidden;}

	// For controllers and actions
	virtual std::string GetValueString() const = 0;
	virtual std::string GetDefaultString() const = 0;
	virtual std::string GetRange() const = 0;
	virtual void SetRange(const std::string& x_range) = 0;

protected:	
	const std::string m_name;
	ParameterConfigType m_confSource;
	const std::string m_description;
	bool m_isLocked;
	bool m_isHidden;
private:
	static log4cxx::LoggerPtr m_logger;
};

#endif
