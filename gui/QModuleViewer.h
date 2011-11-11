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
class OutputStream;
class QComboBox;
class QPainter;
class QGroupBox;

class QModuleViewer : public QWidget 
{
	Q_OBJECT
public:
	QModuleViewer(const Manager * x_manager, QWidget *parent = 0);
	virtual ~QModuleViewer();
private:
	//QLabel *imagelabel;
	QVBoxLayout *layout;
	
	QImage m_image;
	const Module * 		m_currentModule;
	const OutputStream * 	m_currentOutputStream;
	const Manager* 		m_manager;
	
	int m_outputWidth;
	int m_outputHeight;
	int m_offsetX;
	int m_offsetY;
	
	//QSize size;
	//QIcon icon;
	void paintEvent(QPaintEvent *event);
	//void putImage();
	
	QGroupBox * gbSettings;
	QComboBox * comboModules;
	QComboBox * comboOutputStreams;
	virtual void resizeEvent(QResizeEvent * e);

	IplImage * m_img_output;
	// Images for format conversion
	IplImage* m_img_tmp1_c1;
	IplImage* m_img_tmp2_c1;
	IplImage* m_img_tmp1_c3;
	IplImage* m_img_tmp2_c3;

public slots:
	//void Resize(int x_width, int x_height);
	void updateModule(const Module * x_module);
	void updateOutputStream(const OutputStream * x_outputStream);
	void updateModule(int x_index);
	void updateOutputStream(int x_index);
	void showDisplayOptions();
	void hideDisplayOptions();
}; 

#endif
