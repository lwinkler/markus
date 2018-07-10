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

#ifndef QMODULEVIEWER_H
#define QMODULEVIEWER_H

#include <QPixmap>
#include <QWidget>
#include <QImage>
#include <QPainter>
#include <opencv2/opencv.hpp>
#include "QControlBoard.h"
#include "Module.h"
#include "ParameterStructure.h"
#include "ParameterT.h"

class QComboBox;
class QPainter;
class QGroupBox;
class QBoxLayout;
class QLabel;
class QListWidget;

namespace mk {

class Manager;
class Stream;
class ControlBoard;
class Viewer;

/// Class used to display one module in a widget
class QModuleViewer : public QWidget, public Configurable
{
	Q_OBJECT
public:
	class Parameters : public ParameterStructure
	{
	public:
		explicit Parameters(const std::string& x_name) : ParameterStructure(x_name)
		{
			// AddParameter(new ParameterString("module", "", &module, "Module to display"));
			AddParameter(new ParameterString("module",  "", &module,  "Name of the module to display"));
			AddParameter(new ParameterString("stream",  "", &stream,  "Index of the stream to display"));
			AddParameter(new ParameterInt("control", -1, -1, 2000, &control, "Index of the control to display"));
			AddParameter(new ParameterBool("displayOptions", true, &displayOptions, "Display the options to select the module, stream, ..."));

			m_writeAllParamsToConfig = true;
		}
		std::string module;
		std::string stream;
		int control;
		bool displayOptions;
	};

	QModuleViewer(Manager& x_manager, ParameterStructure& xr_params, QWidget *parent = 0);
	~QModuleViewer() override;

	static void  ConvertMat2QImage(const cv::Mat& x_mat, QImage& xr_qim);
	void mouseDoubleClickEvent(QMouseEvent * event) override;

protected:
	void CreateInputStream(int x_outputWidth, int x_outputHeight);
	void Reconnect();
	void updateModule(const Module& x_module);
	int IndexOfModule(std::string& x_moduleName)
	{
		auto it = std::find(m_moduleNames.begin(), m_moduleNames.end(), x_moduleName);
		if(it == m_moduleNames.end())
			return 0; // throw MkException("Cannot find " + x_moduleName + " in module names");
		return it - m_moduleNames.begin();
	}
	int IndexOfStream(const std::string& x_streamName)
	{
		auto it = std::find(m_streamNames.begin(), m_streamNames.end(), x_streamName);
		if(it == m_streamNames.end())
			return 0; // throw MkException("Cannot find " + x_moduleName + " in module names");
		return it - m_streamNames.begin();
	}

	QBoxLayout * mp_mainLayout;

	QImage m_qimage;
	Manager & mr_manager;

	int m_outputWidth = 0;
	int m_outputHeight = 0;
	int m_offsetX = 0;
	int m_offsetY = 0;

	void paintEvent(QPaintEvent *event) override;
	void resizeEvent(QResizeEvent * e) override;

	QGroupBox     * mp_gbCombos     = nullptr;
	QComboBox     * mp_comboModules = nullptr;
	QComboBox     * mp_comboStreams = nullptr;
	QWidget       * mp_widEmpty     = nullptr;
	QControlBoard * mp_controlBoard = nullptr;

	// Stream and module to handle rendering via Markus
	std::vector<std::string> m_moduleNames;
	std::vector<std::string> m_streamNames;
	std::vector<std::string> m_controllerNames;

	Module::Parameters* mp_viewerParams = nullptr;
	Viewer*             mp_viewer       = nullptr;

private:
	Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

public slots:
	void updateModuleNb(int x_index);
	void updateStreamNb(int x_index);
	void updateControlNb(int x_index = -1);
	void showDisplayOptions(bool x_isChecked);
};

} // namespace mk
#endif
