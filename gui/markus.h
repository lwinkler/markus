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

#ifndef markus_H
#define markus_H

#include "QModuleViewer.h"

#include <QMainWindow>
#include <QScrollArea>
#include <QTimer>
#include <QGridLayout>

#include <vector>


class Manager;
class ConfigReader;
class InputTimer;
class QTimerEvent;
class Input;

class InputTimer : public QTimer
{
public:
	InputTimer(Input & x_input);
	virtual void timerEvent(QTimerEvent *);
	
	Input & m_input;
};

class markus : public QMainWindow
{
Q_OBJECT
public:
	markus(ConfigReader& rx_configReader, Manager& rx_manager);
	virtual ~markus();
private:
	ConfigReader& m_configReader;
	Manager& m_manager;
	
	std::vector<InputTimer *> m_inputTimer;
	
	void timerEvent(QTimerEvent*);
	void resizeEvent(QResizeEvent*);
	
	int nbCols;
	int nbLines;
	
	void createActions();
	void createMenus();
	
	QLabel *createLabel(const QString &text);
	
	QWidget m_mainWidget;
	QGridLayout m_mainLayout;
	std::vector<QModuleViewer *> m_moduleViewer;
	
	QAction *exitAct;
	QAction *viewDisplayOptions1Act;
	QAction *viewDisplayOptions0Act;
	QAction *view1x1Act;
	QAction *view1x2Act;
	QAction *view2x2Act;
	QAction *view2x3Act;
	QAction *view3x3Act;
	QAction *view3x4Act;
	QAction *view4x4Act;
	QAction *aboutAct;
	QAction *aboutQtAct;
	
	QMenu *fileMenu;
	QMenu *viewMenu;
	QMenu *helpMenu;
private slots:
	void about();
	void viewDisplayOptions1();
	void viewDisplayOptions0();
	void view1x1();
	void view1x2();
	void view2x2();
	void view2x3();
	void view3x3();
	void view3x4();
	void view4x4();

};


#endif // markus_H
