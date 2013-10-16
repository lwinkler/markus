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
#include "QParameterControlBoard.h"

class Manager;
class Module;
class Stream;
class ControlBoard;
class QComboBox;
class QPainter;
// class QGroupBox;
// class QPushButton;
class QBoxLayout;
class QLabel;
// class QScrollArea;
class QListWidget;

class QModuleViewer : public QWidget 
{
	Q_OBJECT
public:
	QModuleViewer(const Manager * x_manager, QWidget *parent = 0);
	virtual ~QModuleViewer();
	static void  ConvertMat2QImage(const cv::Mat *mat, QImage *qim);
	static void  ConvertIplImage2QImage(const IplImage *iplImg, QImage *qim);
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

	QParameterControlBoard * m_parameterControlBoard;

public slots:
	void updateModuleNb(int x_index);
	void updateStreamOrControlNb(int x_index);
	void updateControlNb(int x_index);
	void updateModule(Module * x_module);
	void updateStream(Stream * x_outputStream);
	void showDisplayOptions();
	void hideDisplayOptions();
};

#endif
