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

#ifndef CONTROLLER_PARAMETERS_H
#define CONTROLLER_PARAMETERS_H

#include "Parameter.h"
#include "ParameterNum.h"
#include "ParameterEnum.h"
#include "ParameterString.h"
#include "ParameterSerializable.h"
#include "Controller.h"


class QParameterSlider;
class QCheckBox;
class QLineEdit;
class QTextEdit;
class QComboBox;
class QLabel;

/// Control class for any parameter
class ControllerParameter : public Controller
{
public:
	ControllerParameter(Parameter& x_param);
	MKCLASS("ControllerParameter")

	virtual QWidget* CreateWidget() = 0;
	virtual void SetWidgetValue(const std::string& x_value) = 0;
	virtual std::string GetValueFromWidget() = 0;

	// Controllers
	void GetParameterType(std::string* xp_value);
	void GetRange(std::string* xp_value);
	void SetControlledValue(std::string* xp_value);
	void GetCurrent(std::string* xp_value);
	void GetDefault(std::string* xp_value);

	typedef void (ControllerParameter::*action)(std::string*);
	DECLARE_CALL_ACTION(action);
	DECLARE_LIST_ACTION(action);

protected:
	std::map<std::string, const action> m_actions;
	Parameter& m_param;
private:
	static log4cxx::LoggerPtr m_logger;
};

/// Control class for an integer parameter
class ControllerInt : public ControllerParameter
{
public:
	ControllerInt(Parameter& x_param);
	virtual QWidget* CreateWidget();
	virtual void SetWidgetValue(const std::string& x_value);
	virtual std::string GetValueFromWidget();

protected:
	QParameterSlider * mp_parameterSlider;
	ParameterInt& m_param2;
};

/// Control class for an integer parameter
class ControllerUInt : public ControllerParameter
{
public:
	ControllerUInt(Parameter& x_param);
	virtual QWidget* CreateWidget();
	virtual void SetWidgetValue(const std::string& x_value);
	virtual std::string GetValueFromWidget();

protected:
	QParameterSlider * mp_parameterSlider;
	ParameterUInt& m_param2;
};


/// Control class for a double parameter
class ControllerDouble : public ControllerParameter
{
public:
	ControllerDouble(Parameter& x_param);
	virtual QWidget* CreateWidget();
	virtual void SetWidgetValue(const std::string& x_value);
	virtual std::string GetValueFromWidget();

protected:
	QParameterSlider * mp_parameterSlider;
	ParameterDouble  & m_param2;
};

/// Control class for a float parameter
class ControllerFloat : public ControllerParameter
{
public:
	ControllerFloat(Parameter& x_param);
	virtual QWidget* CreateWidget();
	virtual void SetWidgetValue(const std::string& x_value);
	virtual std::string GetValueFromWidget();

protected:
	QParameterSlider * mp_parameterSlider;
	ParameterFloat   & m_param2;
};

/// Control class for a boolean parameter
class ControllerBool : public ControllerParameter
{
public:
	ControllerBool(Parameter& x_param);
	virtual QWidget* CreateWidget();
	virtual void SetWidgetValue(const std::string& x_value);
	virtual std::string GetValueFromWidget();

protected:
	QCheckBox     * mp_checkBox;
	ParameterBool & m_param2;
};

/// Control class for a string parameter
class ControllerString : public ControllerParameter
{
public:
	ControllerString(Parameter& x_param);
	virtual QWidget* CreateWidget();
	virtual void SetWidgetValue(const std::string& x_value);
	virtual std::string GetValueFromWidget();

protected:
	QLineEdit       * mp_lineEdit;
	ParameterString & m_param2;
};

/// Control class for a object height parameter
class ControllerSerializable : public ControllerParameter
{
public:
	ControllerSerializable(Parameter& x_param);
	virtual QWidget* CreateWidget();
	virtual void SetWidgetValue(const std::string& x_value);
	virtual std::string GetValueFromWidget();

protected:
	QLineEdit             * mp_lineEdit;
	ParameterSerializable & m_param2;
};

/// Control class for a object height parameter
class ControllerCalibrationByHeight : public ControllerParameter
{
public:
	ControllerCalibrationByHeight(Parameter& x_param);
	virtual QWidget* CreateWidget();
	virtual void SetWidgetValue(const std::string& x_value);
	virtual std::string GetValueFromWidget();

protected:
	//QPainter * parameterLine;
	ParameterSerializable & m_param2;
	QWidget * mp_widget;
	QParameterSlider * mp_sliderX;
	QParameterSlider * mp_sliderY;
	QParameterSlider * mp_sliderHeight;
	QLabel* mp_labX;
	QLabel* mp_labY;
	QLabel* mp_labHeight ;
};


/// Control class for a enum parameter
class ControllerEnum : public ControllerParameter
{
public:
	ControllerEnum(Parameter& x_param);
	virtual QWidget* CreateWidget();
	virtual void SetWidgetValue(const std::string& x_value);
	virtual std::string GetValueFromWidget();

protected:
	QComboBox     * mp_comboBox;
	ParameterEnum & m_param2;
};


/// Control class for a json parameter
class ControllerText : public ControllerParameter
{
public:
	ControllerText(Parameter& x_param);
	virtual QWidget* CreateWidget();
	virtual void SetWidgetValue(const std::string& x_value);
	virtual std::string GetValueFromWidget();

protected:
	QTextEdit * mp_textEdit;
	Parameter & m_param2;
};

#endif
