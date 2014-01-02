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
	Controller();
	virtual ~Controller();

	inline QWidget* RefWidget(){return m_widget;}
	virtual const std::string& GetName() const = 0;
	const std::map<std::string, const px_action>& GetActions() {return m_actions;}
	
protected:
	QWidget * m_widget;
	std::map<std::string, const px_action> m_actions;
};

/// Control class for an integer parameter
class ControllerInt : public Controller
{
public:
	ControllerInt(ParameterInt & x_param);
	~ControllerInt();
	// void SetControlledValue();
	inline virtual const std::string& GetName() const {return param.GetName();}

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
	inline virtual const std::string& GetName() const {return param.GetName();}

	QParameterSlider * parameterSlider;
	ParameterDouble  & param;
};

/// Control class for a float parameter
class ControllerFloat : public Controller
{
public:
	ControllerFloat(ParameterFloat & x_param);
	~ControllerFloat();
	inline virtual const std::string& GetName() const {return param.GetName();}

	QParameterSlider * parameterSlider;
	ParameterFloat   & param;
};

/// Control class for a boolean parameter
class ControllerBool : public Controller
{
public:
	ControllerBool(ParameterBool & x_param);
	~ControllerBool();
	inline virtual const std::string& GetName() const {return param.GetName();}

	QCheckBox     * checkBox;
	ParameterBool & param;
};

/// Control class for a string parameter
class ControllerString : public Controller
{
public:
	ControllerString(ParameterString & x_param);
	~ControllerString();
	void SetControlledValue();
	void GetCurrent();
	void GetDefault();
	inline virtual const std::string& GetName() const {return m_param.GetName();}

protected:
	QLineEdit  * m_lineEdit;
	ParameterString & m_param;
};


/// Control class for a enum parameter
class ControllerEnum : public Controller
{
public:
	ControllerEnum(ParameterEnum & x_param);
	~ControllerEnum();
	void SetControlledValue();
	void GetCurrent();
	void GetDefault();
	inline virtual const std::string& GetName() const {return m_param.GetName();}

protected:
	QComboBox  * m_comboBox;
	ParameterEnum & m_param;
};

/// Class to control a module (settings ...)

/*class ControlBoard
{
public:
	ControlBoard(const std::string& x_name, const std::string& x_description);
	virtual ~ControlBoard();
	inline const std::string& GetName() const {return m_name;}
	inline const std::string& GetDescription() const{return m_description;}
	inline std::vector<Controller*>& RefListControllers(){return m_controllers;}
	inline void AddController(Controller * x_ctrr){m_controllers.push_back(x_ctrr);}
	void SetControlledValue();
	void GetCurrent();
	void GetDefault();
	virtual void Destroy(){};

protected:
	std::string m_name;
	std::string m_description;
	
	std::vector<Controller*> m_controllers;
};
*/

#endif
