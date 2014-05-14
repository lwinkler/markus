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
#include "Controller.h"


class QParameterSlider;
class QCheckBox;
class QLineEdit;
class QComboBox;

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
};

/// Control class for an integer parameter
class ControllerInt : public ControllerParameter
{
public:
	ControllerInt(ParameterInt & x_param);
	virtual QWidget* CreateWidget();
	virtual void SetWidgetValue(const std::string& x_value);
	virtual std::string GetValueFromWidget();

protected:
	QParameterSlider * m_parameterSlider;
	ParameterInt& m_param2;
};


/// Control class for a double parameter
class ControllerDouble : public ControllerParameter
{
public:
	ControllerDouble(ParameterDouble & x_param);
	virtual QWidget* CreateWidget();
	virtual void SetWidgetValue(const std::string& x_value);
	virtual std::string GetValueFromWidget();

protected:
	QParameterSlider * m_parameterSlider;
	ParameterDouble& m_param2;
};

/// Control class for a float parameter
class ControllerFloat : public ControllerParameter
{
public:
	ControllerFloat(ParameterFloat & x_param);
	virtual QWidget* CreateWidget();
	virtual void SetWidgetValue(const std::string& x_value);
	virtual std::string GetValueFromWidget();

protected:
	QParameterSlider * m_parameterSlider;
	ParameterFloat   & m_param2;
};

/// Control class for a boolean parameter
class ControllerBool : public ControllerParameter
{
public:
	ControllerBool(ParameterBool & x_param);
	virtual QWidget* CreateWidget();
	virtual void SetWidgetValue(const std::string& x_value);
	virtual std::string GetValueFromWidget();

protected:
	QCheckBox     * m_checkBox;
	ParameterBool & m_param2;
};

/// Control class for a string parameter
class ControllerString : public ControllerParameter
{
public:
	ControllerString(ParameterString & x_param);
	virtual QWidget* CreateWidget();
	virtual void SetWidgetValue(const std::string& x_value);
	virtual std::string GetValueFromWidget();

protected:
	QLineEdit       * m_lineEdit;
	ParameterString & m_param2;
};

/// Control class for a object height parameter
class ControllerObjectHeight : public ControllerParameter
{
public:
    ControllerObjectHeight(ParameterObjectHeigth & x_param);
	virtual QWidget* CreateWidget();
	virtual void SetWidgetValue(const std::string& x_value);
	virtual std::string GetValueFromWidget();

protected:
    //QPainter * parameterLine;
    ParameterObjectHeigth & m_param2;
    QWidget * widget;
	QParameterSlider * slider_xb;
	QParameterSlider * slider_yb;
	QParameterSlider * slider_heigthb;
	QParameterSlider * slider_xf;
	QParameterSlider * slider_yf;
	QParameterSlider * slider_heigthf;
};


/// Control class for a enum parameter
class ControllerEnum : public ControllerParameter
{
public:
	ControllerEnum(ParameterEnum & x_param);
	virtual QWidget* CreateWidget();
	virtual void SetWidgetValue(const std::string& x_value);
	virtual std::string GetValueFromWidget();

protected:
	QComboBox     * comboBox;
	ParameterEnum & m_param2;
};


#endif
