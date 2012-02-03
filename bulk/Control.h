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
class QScrollBar;

class Controller
{
public:
	Controller();
	~Controller();
	virtual void SetControlledValue() = 0;
	QWidget* RefWidget(){return m_widget;};
	virtual const std::string& GetName() const = 0;
	
protected:
	QWidget * m_widget;
};

class ControllerInt : public Controller
{
public:
	ControllerInt(ParameterInt & x_param);
	~ControllerInt();
	void SetControlledValue();
	inline virtual const std::string& GetName() const {return m_param.GetName();}; 
protected:
	QScrollBar * m_scrollBar;
	ParameterInt & m_param;
};

/// Class to control a module (settings ...)

class Control
{
public:
	Control(const std::string& x_name, const std::string& x_description);
	~Control();
	inline const std::string& GetName() const {return m_name;};
	inline const std::string& GetDescription() const{return m_description;};
	inline std::vector<Controller*>& RefListControllers(){return m_controllers;};
	inline void AddController(Controller * x_ctrr){m_controllers.push_back(x_ctrr);}
	void SetControlledValue();
protected:
	std::string m_name;
	std::string m_description;
	
	std::vector<Controller*> m_controllers;
};

class ParameterControl : public Control
{
public:
	ParameterControl(const std::string& x_name, const std::string& x_description, ParameterStructure& x_param);
	~ParameterControl();
private:
	ParameterStructure& m_param;
};

#endif