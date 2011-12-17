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
#include <QMenu>
#include <qevent.h>

#include <QPixmap>
#include <QPainter>

#include "Manager.h"
#include "Module.h"
#include "util.h"
#include "StreamImage.h"

using namespace std;

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
	
	//set context menu
	QAction * actionShowDisplayMenu = new QAction(tr("Show display options"), this);
	actionShowDisplayMenu->setShortcut(tr("Ctrl+S"));
	QAction * actionHideDisplayMenu = new QAction(tr("Hide display options"), this);
	actionHideDisplayMenu->setShortcut(tr("Ctrl+H"));
	connect(actionShowDisplayMenu, SIGNAL(triggered()), this, SLOT(showDisplayOptions()));
	connect(actionHideDisplayMenu, SIGNAL(triggered()), this, SLOT(hideDisplayOptions()));

	this->addAction(actionShowDisplayMenu);
	this->addAction(actionHideDisplayMenu);
	this->setContextMenuPolicy(Qt::ActionsContextMenu);
	
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
	m_img_original = NULL;
	
	connect(comboModules, SIGNAL(activated(int)), this, SLOT(updateModule(int) ));
	connect(comboOutputStreams, SIGNAL(activated(int)), this, SLOT(updateOutputStream(int)));
}

QModuleViewer::~QModuleViewer(void) 
{
	if(m_img_original != NULL)  cvReleaseImage(&m_img_original); 
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
	// Keep proportionality
	double ratio = static_cast<double>(m_currentOutputStream->GetHeight()) / m_currentOutputStream->GetWidth();
	
	m_outputWidth  = e->size().width();
	m_outputHeight = e->size().height();
	
	if(m_outputHeight >= m_outputWidth * ratio)
	{
		m_outputHeight = m_outputWidth * ratio;
		m_offsetX = 0;
		m_offsetY = (e->size().height() - m_outputHeight) / 2;
	}
	else 
	{
		m_outputWidth = m_outputHeight / ratio;
		m_offsetX = (e->size().width() - m_outputWidth) / 2;
		m_offsetY = 0;
	}
	
	m_image =  QImage(m_outputWidth, m_outputHeight, QImage::Format_RGB32);
	
	if(m_img_output != NULL) cvReleaseImage(&m_img_output);
	if(m_img_original != NULL)  cvReleaseImage(&m_img_original); 
	m_img_output = m_img_original = NULL;
	
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
	
	
	if(m_img_original == NULL)
		m_img_original = cvCreateImage( cvSize(m_currentOutputStream->GetWidth(), m_currentOutputStream->GetHeight()), IPL_DEPTH_8U, 3);
	cvSet(m_img_original, cvScalar(0,0,0));
	if(m_img_output == NULL)
		m_img_output = cvCreateImage( cvSize(m_outputWidth, m_outputHeight), IPL_DEPTH_8U, 3);
	// Write output to screen
	// TODO : Copy below
	//cout<<"Render "<<m_currentOutputStream->GetWidth()<<" to "<<m_img_original->width<<endl;
	m_currentOutputStream->Render(m_img_original);
	if(m_img_output->nChannels == 3)
		adjust(m_img_original, m_img_output, m_img_tmp1_c3, m_img_tmp2_c3);
	else
		adjust(m_img_original, m_img_output, m_img_tmp1_c1, m_img_tmp2_c1);
	
	// switch between bit depths
	switch (m_img_output->depth) 
	{
		case IPL_DEPTH_8U:
			switch (m_img_output->nChannels) 
			{
				case 3:
					cvIndex = 0; cvLineStart = 0;
					for (int y = 0; y < m_img_output->height; y++) 
					{
						unsigned char red,green,blue;
						cvIndex = cvLineStart;
						for (int x = 0; x < m_img_output->width; x++) 
						{
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
	painter.drawImage(QRect(m_offsetX, m_offsetY, m_image.width(), m_image.height()), m_image);
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
	assert(m_currentModule->GetOutputStreamList().size() > 0);
	updateOutputStream(*(m_currentModule->GetOutputStreamList().begin()));
}

void QModuleViewer::updateOutputStream(const OutputStream * x_outputStream)
{
	m_currentOutputStream = x_outputStream;
	if(m_img_original != NULL)
	{
		cvReleaseImage(&m_img_original);
		m_img_original = NULL;
	}
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

void QModuleViewer::showDisplayOptions()
{
	gbSettings->show();
}

void QModuleViewer::hideDisplayOptions()
{
	gbSettings->hide();
}

#include "QModuleViewer.moc"