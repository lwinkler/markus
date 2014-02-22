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
#include "Parameter.h"

class Manager;
class Module;
class Stream;
class ControlBoard;
class QComboBox;
class QPainter;
class QGroupBox;
class QBoxLayout;
class QLabel;
class QListWidget;

class QModuleViewerParameterStructure : public ParameterStructure
{
public:
	QModuleViewerParameterStructure(const ConfigReader& x_confReader) : ParameterStructure(x_confReader)
	{
		// m_list.push_back(new ParameterString("module", "", &module, "Module to display"));
		m_list.push_back(new ParameterInt("module",  -1, -1, 1000, &module,  "Index of the module to display"));
		m_list.push_back(new ParameterInt("stream",  -1, -1, 1000, &stream,  "Index of the stream to display"));
		m_list.push_back(new ParameterInt("control", -1, -1, 1000, &control, "Index of the control to display"));
		m_list.push_back(new ParameterBool("display_options", 0, 0, 1, &displayOptions, "Display the options to select the module, stream, ..."));

		Init();
		m_writeAllParamsToConfig = true;
	}
	int module;
	int stream;
	int control;
	bool displayOptions;
};

class QModuleViewer : public QWidget, public Configurable
{
	Q_OBJECT
public:
	QModuleViewer(const Manager * x_manager, const ConfigReader& x_configReader, QWidget *parent = 0);
	virtual ~QModuleViewer();
	static void  ConvertMat2QImage(const cv::Mat *mat, QImage *qim);
private:
	QBoxLayout * mp_mainLayout;
	
	QImage m_image;
	Module        * m_currentModule;
	const Stream  * m_currentStream;
	const Manager * m_manager;
	
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

	// Images for format conversion
	cv::Mat * m_img_original;
	cv::Mat * m_img_output;
	cv::Mat * m_img_tmp1;
	cv::Mat * m_img_tmp2;

	QControlBoard * m_controlBoard;
	QModuleViewerParameterStructure m_param;
        inline const QModuleViewerParameterStructure& GetParameters() const{return m_param;}

public slots:
	void updateModuleNb(int x_index);
	void updateStreamOrControlNb(int x_index);
	void updateControlNb(int x_index = -1);
	void updateModule(Module * x_module);
	void updateStream(Stream * x_outputStream);
	void showDisplayOptions(bool x_isChecked);
};

#endif
