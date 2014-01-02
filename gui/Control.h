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

#ifndef CONTROL_H
#define CONTROL_H

#include <string>
#include <vector>
#include "Parameter.h"

class Module;
class QWidget;
class QParameterSlider;
class QCheckBox;
class QLineEdit;
class QComboBox;

// Pointer to a void method
class Controller;
typedef void (*px_action)(Controller*);

class Controller
{
public:
	Controller(std::string x_name);
	virtual ~Controller();

	inline QWidget* RefWidget(){return m_widget;}
	const std::string& GetName() {return m_name;};
	const std::map<std::string, const px_action>& GetActions() {return m_actions;}
	
protected:
	QWidget * m_widget;
	std::map<std::string, const px_action> m_actions;
	std::string m_name;
};

/// Control class for an integer parameter
class ControllerInt : public Controller
{
public:
	ControllerInt(ParameterInt & x_param);
	~ControllerInt();
	// void SetControlledValue();

// protected:
	QParameterSlider * parameterSlider;
	ParameterInt     & param;
};


/// Control class for a double parameter
class ControllerDouble : public Controller
{
public:
	ControllerDouble(ParameterDouble & x_param);
	~ControllerDouble();

	QParameterSlider * parameterSlider;
	ParameterDouble  & param;
};

/// Control class for a float parameter
class ControllerFloat : public Controller
{
public:
	ControllerFloat(ParameterFloat & x_param);
	~ControllerFloat();

	QParameterSlider * parameterSlider;
	ParameterFloat   & param;
};

/// Control class for a boolean parameter
class ControllerBool : public Controller
{
public:
	ControllerBool(ParameterBool & x_param);
	~ControllerBool();

	QCheckBox     * checkBox;
	ParameterBool & param;
};

/// Control class for a string parameter
class ControllerString : public Controller
{
public:
	ControllerString(ParameterString & x_param);
	~ControllerString();

// protected:
	QLineEdit  * lineEdit;
	ParameterString & param;
};


/// Control class for a enum parameter
class ControllerEnum : public Controller
{
public:
	ControllerEnum(ParameterEnum & x_param);
	~ControllerEnum();

// protected:
	QComboBox  * comboBox;
	ParameterEnum & param;
};


#endif
