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

// #include <string>
// #include <vector>
#include "Parameter.h"
#include "Controller.h"


class QParameterSlider;
class QCheckBox;
class QLineEdit;
class QComboBox;

/// Control class for an integer parameter
class ControllerInt : public Controller
{
public:
	ControllerInt(ParameterInt & x_param);
	~ControllerInt();
	virtual QWidget* CreateWidget();

	QParameterSlider * parameterSlider;
	ParameterInt     & param;
};


/// Control class for a double parameter
class ControllerDouble : public Controller
{
public:
	ControllerDouble(ParameterDouble & x_param);
	~ControllerDouble();
	virtual QWidget* CreateWidget();

	QParameterSlider * parameterSlider;
	ParameterDouble  & param;
};

/// Control class for a float parameter
class ControllerFloat : public Controller
{
public:
	ControllerFloat(ParameterFloat & x_param);
	~ControllerFloat();
	virtual QWidget* CreateWidget();

	QParameterSlider * parameterSlider;
	ParameterFloat   & param;
};

/// Control class for a boolean parameter
class ControllerBool : public Controller
{
public:
	ControllerBool(ParameterBool & x_param);
	~ControllerBool();
	virtual QWidget* CreateWidget();

	QCheckBox     * checkBox;
	ParameterBool & param;
};

/// Control class for a string parameter
class ControllerString : public Controller
{
public:
	ControllerString(ParameterString & x_param);
	~ControllerString();
	virtual QWidget* CreateWidget();

	QLineEdit  * lineEdit;
	ParameterString & param;
};


/// Control class for a enum parameter
class ControllerEnum : public Controller
{
public:
	ControllerEnum(ParameterEnum & x_param);
	~ControllerEnum();
	virtual QWidget* CreateWidget();

	QComboBox  * comboBox;
	ParameterEnum & param;
};


#endif
