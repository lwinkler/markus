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


#include <QWidget>
#include <QScrollArea>

class QGroupBox;
class QHBoxLayout;

namespace mk {

/// Class to control a module (settings ...)
class Controller;
class Manager;

/// QT widget to display one controller
class QControlBoard : public QWidget
{
	Q_OBJECT
public:
	QControlBoard(Manager& rx_manager, const std::string& x_moduleName, const std::string& x_controllerName, QWidget *xp_parent);
	virtual ~QControlBoard();
	virtual void Destroy() {}

protected:
	void paintEvent(QPaintEvent *event);
	virtual void resizeEvent(QResizeEvent * e);

	QScrollArea * mp_gbControls;
	QGroupBox   * mp_gbButtons;
	QHBoxLayout * mp_buttonLayout;
	Manager&      mr_manager;
	std::string   m_moduleName;
	std::string   m_controllerName;

public slots:
	void callAction();
};
}
#endif
