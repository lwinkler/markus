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

#include <opencv2/opencv.hpp>
#include <QPixmap>
#include <QWidget>
#include <QImage>
#include <QPainter>
#include "QControlBoard.h"
#include "ParameterStructure.h"
#include "ParameterNum.h"
#include "Module.h"

class Manager;
class Stream;
class ControlBoard;
class QComboBox;
class QPainter;
class QGroupBox;
class QBoxLayout;
class QLabel;
class QListWidget;


/// Class used to display one module in a widget
class QModuleViewer : public QWidget, public Configurable
{
	Q_OBJECT
	class Viewer : public Module 
	{
	public:
		Viewer(ParameterStructure& xr_params) : Module(xr_params){}
		virtual ~Viewer() {}
		MKCLASS("Viewer")
		// MKCATEG("Input")
		MKDESCR("TODO")
		virtual void ProcessFrame() override {}
		virtual void Reset() override {}

	private:
		static log4cxx::LoggerPtr m_logger;
	};
public:
	class Parameters : public ParameterStructure
	{
	public:
		Parameters(const std::string& x_name) : ParameterStructure(x_name)
		{
			// AddParameter(new ParameterString("module", "", &module, "Module to display"));
			AddParameter(new ParameterString("module",   "", &module,  "Name of the module to display"));
			AddParameter(new ParameterInt("stream",   0, -1, 1000, &stream,  "Index of the stream to display"));
			AddParameter(new ParameterInt("control", -1, -1, 1000, &control, "Index of the control to display"));
			AddParameter(new ParameterBool("display_options", 1, 0, 1, &displayOptions, "Display the options to select the module, stream, ..."));

			m_writeAllParamsToConfig = true;
		}
		std::string module;
		int stream;
		int control;
		bool displayOptions;
	};

	QModuleViewer(Manager& x_manager, ParameterStructure& xr_params, QWidget *parent = 0);
	virtual ~QModuleViewer();

	static void  ConvertMat2QImage(const cv::Mat& x_mat, QImage& xr_qim);
	void mouseDoubleClickEvent(QMouseEvent * event);

private:
	void CreateStream();
	void updateModule(const Module& x_module);
	int IndexOfModule(std::string& x_moduleName){
		auto it = std::find(m_moduleNames.begin(), m_moduleNames.end(), x_moduleName);
		if(it == m_moduleNames.end())
			return 0; // throw MkException("Cannot find " + x_moduleName + " in module names");
		return it - m_moduleNames.begin();
	}

	QBoxLayout * mp_mainLayout;

	QImage m_image;
	Manager & mr_manager;

	int m_outputWidth;
	int m_outputHeight;
	int m_offsetX;
	int m_offsetY;

	void paintEvent(QPaintEvent *event);
	virtual void resizeEvent(QResizeEvent * e);

	QGroupBox   * mp_gbCombos;
	QComboBox   * mp_comboModules;
	QComboBox   * mp_comboStreams;
	QWidget     * mp_widEmpty;

	QControlBoard * m_controlBoard;
	Parameters& m_param;

	// Stream and module to handle rendering via Markus
	std::vector<std::string> m_moduleNames;
	std::map<int, std::string> m_outputStreams;
	std::map<int, std::string> m_debugStreams;

	Module::Parameters* mp_viewerParams  = nullptr; //TODO remove
	Viewer*       mp_viewerModule        = nullptr;
	Stream*       mp_stream              = nullptr;
	cv::Mat       m_contentImage;
	Serializable* mp_contentSerializable = nullptr;

public slots:
	void updateModuleNb(int x_index);
	void updateStreamNb(int x_index);
	void updateControlNb(int x_index = -1);
	void showDisplayOptions(bool x_isChecked);
};

#endif
