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


/**
* @brief The QT window of the main window of the GUI
*/
class MarkusWindow : public QMainWindow, public Configurable
{
	Q_OBJECT
public:
	class Parameters : public ParameterStructure
	{
	public:
		Parameters(ConfigReader& x_configReader) : ParameterStructure(x_configReader.GetAttribute("name")), config(x_configReader)
		{
			// AddParameter(new ParameterString("module", "", &module, "Module to display"));
			AddParameter(new ParameterInt("nb_cols", 1, 1, 4, &nbCols, "Number of columns for display"));
			AddParameter(new ParameterInt("nb_rows", 1, 1, 4, &nbRows, "Number of rows for display"));

			m_writeAllParamsToConfig = true;
		}
		int nbCols;
		int nbRows;
		ConfigReader& config;
	};

	MarkusWindow(ParameterStructure& rx_param, Manager& rx_manager);
	void WriteConfig(ConfigReader xr_config) const override;
private:
	// ConfigReader& m_configReader;
	Parameters& m_param;

	Manager& m_manager;

	bool notify(QObject *receiver_, QEvent *event_);
	void resizeEvent(QResizeEvent* e) override;

	void createActionsAndMenus();

	QLabel *createLabel(const QString &text);

	QWidget m_mainWidget;
	QGridLayout m_mainLayout;
	std::vector<QModuleViewer *> m_moduleViewer;
	std::vector<QModuleViewer::Parameters *>    m_paramsViewer;



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
