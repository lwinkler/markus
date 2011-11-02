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

#include "QModuleViewer.h"
#include <cstdio>

#include <QComboBox>
#include <QGroupBox>
#include <QGridLayout>
#include <qevent.h>

#include <QPixmap>
#include <QPainter>

#include "Manager.h"
#include "Module.h"
#include "util.h"

// Constructor
QModuleViewer::QModuleViewer(const Manager* x_manager, QWidget *parent) : QWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_outputWidth  = 0.8 * width();
	m_outputHeight = 0.8 * height();
	
	// Handlers for modules
	assert(x_manager != NULL);
	assert(x_manager->GetModuleList().size() > 0);
	m_manager 		= x_manager;
	m_currentModule 	= *x_manager->GetModuleList().begin();
	m_currentOutputStream 	= *m_currentModule->GetOutputStreamList().begin();
		
	comboModules 		= new QComboBox();
	comboOutputStreams 	= new QComboBox();
	layout = new QVBoxLayout;
	//QPainter painter(this);
	//imagelabel = new QLabel;
	//imagelabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	//QImage dummy(m_outputWidth, m_outputHeight, QImage::Format_RGB32);
	//m_image = dummy;
	
	//layout->addWidget(imagelabel);
	
	/*for (int x = 0; x < m_outputWidth; x ++) 
	{
		for (int y =0; y < m_outputHeight; y++) 
		{
			image.setPixel(x,y,qRgb(x, y, y));
		}
	}
	
	imagelabel->setPixmap(QPixmap::fromImage(image));
	*/
	gbSettings = new QGroupBox(tr("Display options"));
	gbSettings->setFlat(true);
	QGridLayout * vbox = new QGridLayout;
	//vbox->addStretch(1);
	
	//vbox->addWidget(m_painter);
	
	QLabel* lab1 = new QLabel(tr("Module"));
	vbox->addWidget(lab1,0,0);
	comboModules->clear();
	int ind = 0;
	for(std::vector<Module*>::const_iterator it = x_manager->GetModuleList().begin(); it != x_manager->GetModuleList().end(); it++)
		comboModules->addItem(QString((*it)->GetName().c_str()), ind++);
	vbox->addWidget(comboModules,0,1);
	
	QLabel* lab2 = new QLabel(tr("Out stream"));
	vbox->addWidget(lab2,1,0);
	this->updateModule(*(x_manager->GetModuleList().begin()));
	vbox->addWidget(comboOutputStreams,1,1);
	
	gbSettings->setLayout(vbox);
	layout->addWidget(gbSettings);
	setLayout(layout);
	int index = 0;
	
	setPalette(QPalette(QColor(0, 0, 40)));
	setAutoFillBackground(true);

	//update();
	m_image = QImage(m_outputWidth, m_outputHeight, QImage::Format_RGB32);
	
	//imagelabel->
	//m_painter.setBackground(QPixmap::fromImage(m_image));
	
	m_img_tmp1_c1 = NULL; // Allocated on first conversion
	m_img_tmp1_c3 = NULL;
	m_img_tmp2_c1 = NULL;
	m_img_tmp2_c3 = NULL;
	m_img_output  = NULL;
	
	connect(comboModules, SIGNAL(activated(int)), this, SLOT(updateModule(int) ));
	connect(comboOutputStreams, SIGNAL(activated(int)), this, SLOT(updateOutputStream(int)));
}

QModuleViewer::~QModuleViewer(void) 
{
	if(m_img_output != NULL)  cvReleaseImage(&m_img_output); 
	if(m_img_tmp1_c1 != NULL) cvReleaseImage(&m_img_tmp1_c1);
	if(m_img_tmp1_c3 != NULL) cvReleaseImage(&m_img_tmp1_c3);
	if(m_img_tmp2_c1 != NULL) cvReleaseImage(&m_img_tmp2_c1);
	if(m_img_tmp2_c3 != NULL) cvReleaseImage(&m_img_tmp2_c3);
}

/*void QModuleViewer::Resize(int x_width, int x_height)
{
	QResizeEvent* e = new QResizeEvent(QSize(x_width, x_height), size());
	resizeEvent(e);
	delete(e);
}*/

void QModuleViewer::resizeEvent(QResizeEvent * e)
{
	std::cout<<"resizeEvent "<<e->size().width()<<" "<<e->size().height()<<std::endl;
	m_image =  QImage(e->size().width(), e->size().height(), QImage::Format_RGB32);
	//usleep(100000);
	m_outputWidth  = m_image.width();
	m_outputHeight = m_image.height();
	
	if(m_img_output != NULL) cvReleaseImage(&m_img_output);
	m_img_output = NULL;
	
	if(m_img_output != NULL)  cvReleaseImage(&m_img_output); 
	if(m_img_tmp1_c1 != NULL) cvReleaseImage(&m_img_tmp1_c1);
	if(m_img_tmp1_c3 != NULL) cvReleaseImage(&m_img_tmp1_c3);
	if(m_img_tmp2_c1 != NULL) cvReleaseImage(&m_img_tmp2_c1);
	if(m_img_tmp2_c3 != NULL) cvReleaseImage(&m_img_tmp2_c3);
	
	m_img_tmp1_c1 = m_img_tmp1_c3 = m_img_tmp2_c1 = m_img_tmp2_c3 = NULL;

}

/*void QModuleViewer::putImage() 
{
	//paintEvent(NULL);
	update();
}*/

void QModuleViewer::paintEvent(QPaintEvent * e) 
{
	int cvIndex, cvLineStart;
	const IplImage * cvimage = m_currentOutputStream->GetImageRef();
	/*
	if(m_image.width() - 0.8 * width() > 10 || m_image.height() - 0.8 * height() > 10)
	{
		// Resize the image
		m_image =  QImage(width() * 0.8, height() * 0.8, QImage::Format_RGB32);
		usleep(100000);
		std::cout<<"Reiszing "<<std::endl;
	}*/
	m_outputWidth  = m_image.width();
	m_outputHeight = m_image.height();
	
	if(m_img_output == NULL)
		m_img_output = cvCreateImage( cvSize(m_outputWidth, m_outputHeight), IPL_DEPTH_8U, 3);
	// Write output to screen
	// TODO : Copy below
	if(cvimage->nChannels == 3)
		adjust(cvimage, m_img_output, m_img_tmp1_c3, m_img_tmp2_c3);
	else
		adjust(cvimage, m_img_output, m_img_tmp1_c1, m_img_tmp2_c1);
	
	// switch between bit depths
	switch (m_img_output->depth) {
		case IPL_DEPTH_8U:
			switch (m_img_output->nChannels) {
				case 3:
					if ( (m_img_output->width != m_image.width()) || (m_img_output->height != m_image.height()) ) 
					{
						assert(false);
						break;
						std::cout<<"Resizing"<<std::endl;
						QImage temp(m_img_output->width, m_img_output->height, QImage::Format_RGB32);
						m_image = temp;
					}
					cvIndex = 0; cvLineStart = 0;
					for (int y = 0; y < m_img_output->height; y++) 
					{
						unsigned char red,green,blue;
						cvIndex = cvLineStart;
						for (int x = 0; x < m_img_output->width; x++) {
							// DO it
							red = m_img_output->imageData[cvIndex+2];
							green = m_img_output->imageData[cvIndex+1];
							blue = m_img_output->imageData[cvIndex+0];
							
							m_image.setPixel(x,y,qRgb(red, green, blue));
							cvIndex += 3;
						}
						cvLineStart += m_img_output->widthStep;                        
					}
					break;
				default:
					throw("This number of channels is not supported\n");
					break;
			}
			break;
				default:
					throw("This type of IplImage is not implemented in QModuleViewer\n");
					break;
	}
	//imagelabel->setPixmap(QPixmap::fromImage(m_image));
	
	QPainter painter(this);
	painter.drawImage(QRect(0, 0, m_outputWidth, m_outputHeight), m_image);
}

void QModuleViewer::updateModule(const Module * x_module)
{
	m_currentModule = x_module;
	comboOutputStreams->clear();
	int cpt = 0;
	for(std::vector<OutputStream*>::const_iterator it = m_currentModule->GetOutputStreamList().begin(); it != m_currentModule->GetOutputStreamList().end(); it++)
	{
		comboOutputStreams->addItem(QString((*it)->GetName().c_str()), cpt++);
	}

	updateOutputStream(*(m_currentModule->GetOutputStreamList().begin()));
}

void QModuleViewer::updateOutputStream(const OutputStream * x_outputStream)
{
	m_currentOutputStream = x_outputStream;
}

void QModuleViewer::updateModule(int x_index)
{
	std::vector<Module*>::const_iterator it = m_manager->GetModuleList().begin();
	
	while(x_index-- > 0 && it != m_manager->GetModuleList().end())
		it++;
	
	updateModule(*it);
}

void QModuleViewer::updateOutputStream(int x_index)
{
	std::vector<OutputStream*>::const_iterator it = m_currentModule->GetOutputStreamList().begin();
	
	while(x_index-- > 0 && it != m_currentModule->GetOutputStreamList().end())
		it++;
	
	updateOutputStream((*it));
}

void QModuleViewer::toggleDisplayOptions(int choice)
{
	if(choice == -1)
	{
		if(gbSettings->isHidden())
			choice = 1;
		else choice = 0;
	}
	if(choice == 0)
	{
		gbSettings->hide();
	}
	else
	{
		gbSettings->show();
	}
}


#include "QModuleViewer.moc"