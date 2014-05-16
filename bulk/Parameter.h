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
#include "ConfigReader.h"
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
	PARAM_IMAGE_TYPE
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
		m_isLocked(false){}
	virtual ~Parameter(){}
		
	virtual void SetValue(const std::string& x_value, ParameterConfigType x_confType /*= PARAMCONF_UNKNOWN*/) = 0;
	//virtual void SetValue(const void* x_value, ParameterConfigType x_confType = PARAMCONF_UNKNOWN) = 0;
	virtual void SetDefault(const std::string& x_value) = 0;
	// virtual const void* GetValue() const = 0;
	inline const std::string& GetName() const {return m_name;}
	virtual const ParameterType& GetType() const = 0;
	virtual const std::string& GetTypeString() const = 0;
	inline const std::string& GetDescription() const {return m_description;}
	inline const ParameterConfigType& GetConfigurationSource() const {return m_confSource;}
	virtual void SetValueToDefault() = 0;
	virtual void Print(std::ostream& os) const = 0;
	virtual bool CheckRange() const = 0;
	virtual void Export(std::ostream& rx_os, int x_indentation) = 0;
	inline void Lock()
	{
		m_isLocked = true;
	}
	inline bool IsLocked() const {return m_isLocked;}

	// For controllers and actions
	virtual std::string GetValueString() const = 0;
	virtual std::string GetDefaultString() const = 0;
	virtual std::string GetRange() const = 0;

protected:	
	const std::string m_name;
	ParameterConfigType m_confSource;
	const std::string m_description;
	bool m_isLocked;
private:
	static log4cxx::LoggerPtr m_logger;
};

/// Template for all parameters with numerical values
template<class T> class ParameterT : public Parameter
{
public:
	ParameterT(const std::string& x_name, T x_default, T x_min, T x_max, T * xp_value, const std::string& x_description) : 
		Parameter(x_name, x_description),
		m_default(x_default),
		m_min(x_min),
		m_max(x_max),
		mp_value(xp_value){}
	inline std::string GetValueString() const {std::stringstream ss; ss<<*mp_value; return ss.str();}
	inline std::string GetDefaultString() const{std::stringstream ss; ss<<m_default; return ss.str();}
	inline std::string GetRange() const{std::stringstream ss; ss<<"["<<m_min<<":"<<m_max<<"]"; return ss.str();}
	inline const ParameterType& GetType() const {return m_type;}
	inline const std::string& GetTypeString() const{return m_typeStr;}
	inline const T GetDefault() const {return m_default;}
	inline const T GetMin() const {return m_min;}
	inline const T GetMax() const{return m_max;}
	
	virtual void SetValue(const std::string& rx_value, ParameterConfigType x_confType/* = PARAMCONF_UNKNOWN*/)
	{
		if(m_isLocked) 
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		std::istringstream istr(rx_value);
		istr >> *mp_value; // atof is sensible to locale format and may use , as a separator
		m_confSource = x_confType;
	}
	inline void SetValue(T x_value, ParameterConfigType x_confType/* = PARAMCONF_UNKNOWN*/)
	{
		if(m_isLocked) 
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		*mp_value = x_value;
		m_confSource = x_confType;
	}
	/*virtual void SetValue(const void * px_value, ParameterConfigType x_confType = PARAMCONF_UNKNOWN)
	{	
		*mp_value = *static_cast<const T*>(px_value);
		m_confSource = x_confType;
	};*/
	virtual void SetDefault(const std::string& rx_value)
	{	
		std::istringstream istr(rx_value);
		istr >> m_default; // atof is sensible to locale format and may use , as a separator
	}
	virtual T GetValue() const
	{
		return *mp_value;
	}
	virtual bool CheckRange() const
	{
		T value = GetValue();
		return (value <= m_max && value >= m_min);
	}
	virtual void Print(std::ostream& os) const 
	{
		os<<m_name<<" = "<<GetValue()<<" ["<<m_min<<":"<<m_max<<"] ("<<configType[m_confSource]<<"); ";
	}
	virtual void SetValueToDefault()
	{
		if(m_isLocked) 
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		*mp_value = m_default;
		m_confSource = PARAMCONF_DEF;
	}
	virtual void Export(std::ostream& rx_os, int x_indentation)
	{
		std::string tabs(x_indentation, '\t');
		rx_os<<tabs<<"<param name=\""<<m_name<<"\">"<<std::endl;
		tabs = std::string(x_indentation + 1, '\t');
		rx_os<<tabs<<"<type>"<<GetTypeString()<<"</type>"<<std::endl;
		rx_os<<tabs<<"<value min=\""<<m_min<<"\" max=\""<<m_max<<"\" default=\""<<m_default<<"\">"<<GetValue()<<"</value>"<<std::endl;
		rx_os<<tabs<<"<description>"<<m_description<<"</description>"<<std::endl;
		tabs = std::string(x_indentation, '\t');
		rx_os<<tabs<<"</param>"<<std::endl;
	}
	
	T m_default;
	const T m_min;
	const T m_max;
private:
	T* mp_value;
	static const ParameterType m_type;
	static const std::string m_typeStr;
};

class ParameterCalibrationByHeight : public Parameter
{
public:
	static CalibrationByHeight DefaultFg;
	static CalibrationByHeight DefaultBg;


public:
	ParameterCalibrationByHeight(const std::string& x_name, const CalibrationByHeight& x_default, CalibrationByHeight * xp_value, const std::string& x_description) :
		Parameter(x_name, x_description),
		m_default(x_default),
		mp_value(xp_value)
	{
	}
	inline std::string GetValueString() const {
		if (mp_value->x == 0 && mp_value->y == 0 && mp_value->heigth == 0)
			return "";
		std::stringstream ss;
		mp_value->Serialize(ss,"");
		return ss.str();
	}
	inline std::string GetDefaultString() const{std::stringstream ss; m_default.Serialize(ss,""); return ss.str();}
	inline std::string GetRange() const{/*std::stringstream ss; ss<<"["<<m_min<<":"<<m_max<<"]"; return ss.str();*/ return "";}	
	inline const ParameterType& GetType() const {const static ParameterType s = PARAM_OBJECT_HEIGHT; return s;}
	inline const std::string& GetTypeString() const {const static std::string s = "objectHeigth"; return s;}
	inline const CalibrationByHeight& GetDefault() const {
		return m_default;}

	inline void SetValue(const CalibrationByHeight& x_value, ParameterConfigType x_confType)
	{
		if(m_isLocked)
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		*mp_value = x_value;
		m_confSource = x_confType;
	}

	virtual void SetValue(const std::string& rx_value, ParameterConfigType x_confType /*= PARAMCONF_UNKNOWN*/)
	{
		if(m_isLocked)
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		std::istringstream istr(rx_value);
		if(rx_value == "")	// This case happens with unit testing
		{
			mp_value->x = 0;
			mp_value->y = 0;
			mp_value->heigth = 0;
			return;
		}

		mp_value->Deserialize(istr, ""); // atof is sensible to locale format and may use , as a separator
		m_confSource = x_confType;
	}
	virtual void SetDefault(const std::string& rx_value)
	{
		std::istringstream istr(rx_value);
		m_default.Deserialize(istr); // atof is sensible to locale format and may use , as a separator
	}
	const CalibrationByHeight& GetValue() const
	{
		return *mp_value;
	}
	virtual bool CheckRange() const
	{
		CalibrationByHeight object = GetValue();
		return ((object.x <= 1 && object.x >= 0) &&
		(object.y <= 1 && object.y >= 0) &&
		(object.heigth <= 1 && object.heigth >= 0));
	}
	virtual void Print(std::ostream& os) const
	{
		os<<m_name<<" : x = "<< mp_value->x
				  <<", y = "<< mp_value->y
				  <<", heigth = "<< mp_value->heigth
				  <<" ("<<configType[m_confSource]<<"); ";
	}
	virtual void SetValueToDefault()
	{
		if(m_isLocked)
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		*mp_value = m_default;
		m_confSource = PARAMCONF_DEF;
	}
	virtual void Export(std::ostream& rx_os, int x_indentation)
	{
		std::string tabs(x_indentation, '\t');
		rx_os<<tabs<<"<param name=\""<<m_name<<"\">"<<std::endl;
		tabs = std::string(x_indentation + 1, '\t');
		rx_os<<tabs<<"<type>"<<GetTypeString()<<"</type>"<<std::endl;
		rx_os<<tabs<<"<value default=\'"<<m_default.SerializeToString()<<"\'>";
		rx_os<<mp_value->SerializeToString()<<"</value>"<<std::endl;
		rx_os<<tabs<<"<description>"<<m_description<<"</description>"<<std::endl;
		tabs = std::string(x_indentation, '\t');
		rx_os<<tabs<<"</param>"<<std::endl;
	}


private:
	CalibrationByHeight m_default;
	CalibrationByHeight* mp_value;
};

/// Parameter of type string
class ParameterString : public Parameter
{
public:
	ParameterString(const std::string& x_name, std::string x_default, std::string * xp_value, const std::string& x_description) : 
		Parameter(x_name, x_description),
		m_default(x_default),
		mp_value(xp_value){}
	virtual void SetValue(const std::string& rx_value, ParameterConfigType x_confType /*= PARAMCONF_UNKNOWN*/)
	{
		if(m_isLocked) 
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		*mp_value = rx_value;
		m_confSource = x_confType;
	}
	virtual void SetDefault(const std::string& x_value)
	{	
		m_default = x_value;
	}
	virtual std::string GetValue() const
	{
		return *mp_value;
	}
	inline std::string GetValueString() const {return *mp_value;}
	inline std::string GetDefaultString() const {return m_default;}
	inline std::string GetRange() const{return "";}
	inline virtual bool CheckRange() const
	{
		return true;
	}
	virtual void Print(std::ostream& os) const 
	{
		os<<m_name<<" = "<<GetValue()<<" ("<<configType[m_confSource]<<"); ";
	}
	virtual void SetValueToDefault()
	{
		if(m_isLocked) 
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		*mp_value = m_default;
		m_confSource = PARAMCONF_DEF;
	}
	virtual void Export(std::ostream& rx_os, int x_tabs)
	{
		std::string tabs(x_tabs, '\t');
		rx_os<<tabs<<"<param name=\""<<m_name<<"\">"<<std::endl;
		tabs = std::string(x_tabs + 1, '\t');
		rx_os<<tabs<<"<type>"<<GetTypeString()<<"</type>"<<std::endl;
		rx_os<<tabs<<"<value default=\""<<m_default<<"\">"<<GetValue()<<"</value>"<<std::endl;
		rx_os<<tabs<<"<description>"<<m_description<<"</description>"<<std::endl;
		tabs = std::string(x_tabs, '\t');
		rx_os<<tabs<<"</param>"<<std::endl;
	}
	inline const ParameterType& GetType() const {const static ParameterType s = PARAM_STR; return s;}
	inline const std::string& GetTypeString() const {const static std::string s = "string"; return s;}

	inline const std::string& GetDefault(){return m_default;}
private:
	std::string m_default;
	std::string* mp_value;
};

/// Parameter of type enum
class ParameterEnum : public Parameter
{
public:
	ParameterEnum(const std::string& x_name, int x_default, int * xp_value, const std::string x_description):
		Parameter(x_name, x_description),
		m_default(x_default),
		mp_value(xp_value){}
	void SetValue(const std::string& rx_value, ParameterConfigType x_confType/* = PARAMCONF_UNKNOWN*/);
	void SetValue(int rx_value, ParameterConfigType x_confType/* = PARAMCONF_UNKNOWN*/);
	void SetDefault(const std::string& rx_value);
	inline int GetDefault() const {return m_default;}
	inline int GetValue() const{return *mp_value;}
	inline std::string GetValueString() const {return GetReverseEnum().at(*mp_value);}
	inline std::string GetDefaultString() const{return GetReverseEnum().at(m_default);}
	inline std::string GetRange() const
	{
		std::stringstream ss; 
		for(std::map<std::string,int>::const_iterator it = GetEnum().begin() ; it != GetEnum().end() ; it++)
			ss<<it->first<<",";
		return ss.str();
	}
	virtual bool CheckRange() const;
	virtual void Print(std::ostream& os) const;
	virtual void SetValueToDefault()
	{
		if(m_isLocked) 
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		*mp_value = m_default;
		m_confSource = PARAMCONF_DEF;
	}
	inline const ParameterType& GetType() const {const static ParameterType s = PARAM_IMAGE_TYPE; return s;}
	virtual const std::string& GetTypeString() const = 0;
	virtual void Export(std::ostream& rx_os, int x_indentation);
	virtual const std::map<std::string, int>& GetEnum() const = 0;
	virtual const std::map<int, std::string>& GetReverseEnum() const = 0;
	
protected:

	int m_default;
	int* mp_value;
};

/// Parameter for the type of an OpenCV image
class ParameterImageType : public ParameterEnum
{
public:
	ParameterImageType(const std::string& x_name, int x_default, int * xp_value, const std::string x_description);
	~ParameterImageType(){}
	// void Export(std::ostream& rx_os, int x_indentation);

	// Conversion methods
	inline const std::string& GetTypeString() const {const static std::string s = "imageType"; return s;}
	const std::map<std::string, int> & GetEnum() const {return Enum;}
	const std::map<int, std::string> & GetReverseEnum() const {return ReverseEnum;}
	static std::map<std::string, int> CreateMap();
	static std::map<int, std::string> CreateReverseMap();

	// static attributes
	const static std::map<std::string, int> Enum;
	const static std::map<int, std::string> ReverseEnum;
};

/// Represents a set of parameters for a configurable objects
class ParameterStructure
{
public:
	ParameterStructure(const ConfigReader& x_configReader);
	~ParameterStructure();
	void Init();
	void SetFromConfig();
	void UpdateConfig() const;
	void SetValueToDefault();
	void CheckRange(bool x_checkRelated) const;
	void PrintParameters() const;
	//void SetValueByName(const std::string& x_name, const std::string& x_value, ParameterConfigType x_configType = PARAMCONF_UNKNOWN);
	const Parameter & GetParameterByName(const std::string& x_name) const;
	const std::vector<Parameter*>& GetList() const {return m_list;}
	
protected:
	std::vector<Parameter*> m_list;
	const ConfigReader m_configReader; // Warning this still contains reference to the tinyxml config!
	std::string m_moduleName;
	bool m_writeAllParamsToConfig;
	Parameter & RefParameterByName(const std::string& x_name);
private:
	static log4cxx::LoggerPtr m_logger;
};

/// Parameter of type integer
typedef ParameterT<int> 	ParameterInt;
/// Parameter of type double
typedef ParameterT<double> 	ParameterDouble;
/// Parameter of type float
typedef ParameterT<float> 	ParameterFloat;
/// Parameter of type boolean
typedef ParameterT<bool> 	ParameterBool;

#endif
