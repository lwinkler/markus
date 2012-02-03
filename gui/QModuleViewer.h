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

#include <opencv/cv.h>
#include <QPixmap>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QImage>
//#include <QIcon>
#include <QPainter>

class Manager;
class Module;
class Stream;
class Control;
class QComboBox;
class QPainter;
class QGroupBox;
class QPushButton;

class QModuleViewer : public QWidget 
{
	Q_OBJECT
public:
	QModuleViewer(const Manager * x_manager, QWidget *parent = 0);
	virtual ~QModuleViewer();
	static void  ConvertMat2QImage(const cv::Mat *mat, QImage *qim);
	static void  ConvertIplImage2QImage(const IplImage *iplImg, QImage *qim);
private:
	//QLabel *imagelabel;
	QVBoxLayout *layout;
	
	QImage m_image;
	Module * 		m_currentModule;
	const Stream * 		m_currentStream;
	Control*	 	m_currentControl;
	const Manager* 		m_manager;
	
	int m_outputWidth;
	int m_outputHeight;
	int m_offsetX;
	int m_offsetY;
	
	void paintEvent(QPaintEvent *event);
	
	QGroupBox * gbSettings;
	QGroupBox * gbControls;
	QComboBox * comboModules;
	QComboBox * comboStreams;
	QPushButton*  m_buttonUpdateControl;
	virtual void resizeEvent(QResizeEvent * e);

	// Images for format conversion
	cv::Mat * m_img_original;
	cv::Mat * m_img_output;
	cv::Mat * m_img_tmp1;
	cv::Mat * m_img_tmp2;

public slots:
	void updateModule(int x_index);
	void updateStreamOrControl(int x_index);
	void updateModule(Module * x_module);
	void updateStream(Stream * x_outputStream);
	void updateControl(Control * x_control);
	void showDisplayOptions();
	void hideDisplayOptions();
	void applyControl();
}; 

#endif
