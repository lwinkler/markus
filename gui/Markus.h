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

#ifndef MARKUS_H
#define MARKUS_H

#include "QModuleViewer.h"

#include <QMainWindow>
#include <QScrollArea>
#include <QTimer>
#include <QGridLayout>

#include <vector>

#define MARKUS_TIMER_S 0.01 // Interval for the main timer

class Manager;
class ConfigReader;
class QModuleTimer;

class MarkusParameterStructure : public ParameterStructure
{
public:
	MarkusParameterStructure(const ConfigReader& x_confReader) : ParameterStructure(x_confReader)
	{
		// m_list.push_back(new ParameterString("module", "", &module, "Module to display"));
		m_list.push_back(new ParameterInt("nb_cols", 1, 1, 4, &nbCols, "Number of columns for display"));
		m_list.push_back(new ParameterInt("nb_rows", 1, 1, 4, &nbRows, "Number of rows for display"));

		Init();
		m_writeAllParamsToConfig = true;
	}
	int nbCols;
	int nbRows;
};

class Markus : public QMainWindow, public Configurable
{
Q_OBJECT
public:
	Markus(ConfigReader& rx_configReader, Manager& rx_manager);
	virtual ~Markus();
        void UpdateConfig();
private:
	// ConfigReader& m_configReader;
	MarkusParameterStructure m_param;
        inline const MarkusParameterStructure& GetParameters() const{return m_param;}

	Manager& m_manager;
	
	//std::vector<QModuleTimer *> m_moduleTimer;
	
	bool notify(QObject *receiver_, QEvent *event_);
	void timerEvent(QTimerEvent*);
	void resizeEvent(QResizeEvent*);
	
	void createActionsAndMenus();
	
	QLabel *createLabel(const QString &text);
	
	QWidget m_mainWidget;
	QGridLayout m_mainLayout;
	std::vector<QModuleViewer *> m_moduleViewer;
	
	
private slots:
	void about();
	void viewDisplayOptions(bool x_isChecked);
	void view1x1();
	void view1x2();
	void view2x2();
	void view2x3();
	void view3x3();
	void view3x4();
	void view4x4();
	void callManagerCommand();

};


#endif // Markus_H
