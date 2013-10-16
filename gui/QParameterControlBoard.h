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

#ifndef QPARAMETERCONTROLBOARD_H
#define QPARAMETERCONTROLBOARD_H

#include <QPixmap>
#include <QWidget>
#include <QImage>
#include <QPainter>

// class Manager;
// class Module;
// class Stream;
class ControlBoard;
// class QComboBox;
// class QPainter;
class QGroupBox;
class QPushButton;
// class QBoxLayout;
// class QLabel;
class QScrollArea;
// class QListWidget;
class Module;

class QParameterControlBoard : public QWidget 
{
	Q_OBJECT
public:
	QParameterControlBoard(Module * x_module, QWidget *parent = 0);
	virtual ~QParameterControlBoard();
private:
	
	Module        * m_currentModule;
	ControlBoard  * m_currentControl;

	void paintEvent(QPaintEvent *event);
	virtual void resizeEvent(QResizeEvent * e);

	QScrollArea * mp_gbControls;
	QGroupBox   * mp_gbButtons;
	QPushButton * mp_buttonGetCurrentControl;
	QPushButton * mp_buttonGetDefaultControl;
	QPushButton * mp_buttonSetControl;
	QPushButton * mp_buttonResetModule;


public slots:
	void updateControl(ControlBoard * x_control);
	void getCurrentControl();
	void getDefaultControl();
	void SetControlledValue();
	void resetModule();
};

#endif
