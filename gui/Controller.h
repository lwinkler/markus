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

#ifndef CONTROLLER_H
#define CONTROLLER_H

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

	// inline QWidget* RefWidget(){return m_widget;}
	virtual QWidget* CreateWidget() = 0;
	const std::string& GetName() {return m_name;};
	const std::map<std::string, const px_action>& GetActions() {return m_actions;}
	
protected:
	// QWidget * m_widget;
	std::map<std::string, const px_action> m_actions;
	std::string m_name;
};
#endif