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
#include "Module.h"

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


/// Class used to display one module in a widget
class QModuleViewer : public QWidget, public Configurable
{
	Q_OBJECT
public:
	class Parameters : public ParameterStructure
	{
	public:
		Parameters(const std::string& x_name) : ParameterStructure(x_name)
		{
			// AddParameter(new ParameterString("module", "", &module, "Module to display"));
			AddParameter(new ParameterInt("module",   0, -1, 1000, &module,  "Index of the module to display"));
			AddParameter(new ParameterInt("stream",   0, -1, 1000, &stream,  "Index of the stream to display"));
			AddParameter(new ParameterInt("control", -1, -1, 1000, &control, "Index of the control to display"));
			AddParameter(new ParameterBool("display_options", 1, 0, 1, &displayOptions, "Display the options to select the module, stream, ..."));

			m_writeAllParamsToConfig = true;
		}
		int module;
		int stream;
		int control;
		bool displayOptions;
	};

	QModuleViewer(const Manager& x_manager, ParameterStructure& xr_params, QWidget *parent = 0);
	virtual ~QModuleViewer();
	static void  ConvertMat2QImage(const cv::Mat *mat, QImage *qim);
	void mouseDoubleClickEvent(QMouseEvent * event);
private:
	QBoxLayout * mp_mainLayout;

	QImage m_image;
	Module        * m_currentModule;
	const Stream  * m_currentStream;
	const Manager & mr_manager;

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
	Parameters& m_param;

public slots:
	void updateModuleNb(int x_index);
	void updateStreamNb(int x_index);
	void updateControlNb(int x_index = -1);
	void updateModule(Module * x_module);
	void updateStream(Stream * x_outputStream);
	void showDisplayOptions(bool x_isChecked);
};

#endif
