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

namespace mk {
class Manager;


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
		explicit Parameters(mkconf& x_configReader) : ParameterStructure("window"), config(x_configReader)
		{
			// AddParameter(new ParameterString("module", "", &module, "Module to display"));
			AddParameter(new ParameterInt("nbCols", 1, 1, 4, &nbCols, "Number of columns for display"));
			AddParameter(new ParameterInt("nbRows", 1, 1, 4, &nbRows, "Number of rows for display"));

			m_writeAllParamsToConfig = true;
		}
		int nbCols;
		int nbRows;
		mkjson config;
	};

	MarkusWindow(ParameterStructure& rx_param, Manager& rx_manager);
	~MarkusWindow() override;
	void WriteConfig(mkconf& xr_config, bool x_nonDefaultOnly = false) const override;
private:
	// mkconf& m_configReader;
	Parameters& m_param;

	Manager& mr_manager;

	bool notify(QObject *receiver_, QEvent *event_);
	void resizeEvent(QResizeEvent* e) override;

	void createActionsAndMenus();

	QWidget m_mainWidget;
	QGridLayout m_mainLayout;
	std::vector<QModuleViewer *> m_moduleViewer;
	std::vector<QModuleViewer::Parameters *>    m_paramsViewer;



private slots:
	void about();
	void viewDisplayOptions(bool x_isChecked);
	void refresh();
	void editor();
	void save();
	void view1x1();
	void view1x2();
	void view2x2();
	void view2x3();
	void view3x3();
	void view3x4();
	void view4x4();
	void callManagerCommand();
};


} // namespace mk
#endif // Markus_H
