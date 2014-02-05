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

#ifndef CONTROL_BOARD_H
#define CONTROL_BOARD_H


//#include "Input.h"
#include <QWidget>
#include <QScrollArea>

/// Class to control a module (settings ...)
class Controller;
class Module;
class QGroupBox;
class QHBoxLayout;

class QControlBoard : public QWidget 
{
	Q_OBJECT
public:
	// QControlBoard(const std::string& x_name, const std::string& x_description);
	QControlBoard(Module& x_module, QWidget *parent = 0);
	~QControlBoard();
	// inline const std::string& GetName() const {return m_name;}
	// inline const std::string& GetDescription() const{return m_description;}
	// inline std::vector<Controller*>& RefListControllers(){return m_controllers;}
	// inline void AddController(Controller * x_ctrr){m_controllers.push_back(x_ctrr);}
	virtual void Destroy(){};
	void updateControl(Controller* x_control);

protected:
	// std::string m_name;
	// std::string m_description;
	
	Controller  * mp_currentControl;
	Module      & m_currentModule;

	void paintEvent(QPaintEvent *event);
	virtual void resizeEvent(QResizeEvent * e);

	QScrollArea * mp_gbControls;
	QGroupBox   * mp_gbButtons;
	QHBoxLayout * mp_buttonLayout;

public slots:
	void callAction();
};
#endif
