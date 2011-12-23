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
using namespace cv;

// Constructor
QModuleViewer::QModuleViewer(const Manager* x_manager, QWidget *parent) : QWidget(parent)
{
	m_img_tmp1_c1 = NULL; // Allocated on first conversion
	m_img_tmp1_c3 = NULL;
	m_img_tmp2_c1 = NULL;
	m_img_tmp2_c3 = NULL;
	m_img_output  = NULL;
	m_img_original = NULL;

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_outputWidth  = 0.8 * width();
	m_outputHeight = 0.8 * height();
	
	// Handlers for modules
	assert(x_manager != NULL);
	assert(x_manager->GetModuleList().size() > 0);
	m_manager 		= x_manager;
	m_currentModule 	= *x_manager->GetModuleList().begin();
	m_currentStream 	= *m_currentModule->GetStreamList().begin();
		
	comboModules 		= new QComboBox();
	comboStreams 	= new QComboBox();
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
	
	// Fill the list of modules
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
	vbox->addWidget(comboStreams,1,1);
	
	gbSettings->setLayout(vbox);
	layout->addWidget(gbSettings);
	setLayout(layout);
	//int index = 0;
	
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
	
	
	connect(comboModules, SIGNAL(activated(int)), this, SLOT(updateModule(int) ));
	connect(comboStreams, SIGNAL(activated(int)), this, SLOT(updateStream(int)));
}

QModuleViewer::~QModuleViewer(void) 
{
	if(m_img_original != NULL)  delete(m_img_original); 
	if(m_img_output != NULL)  delete(m_img_output); 
	if(m_img_tmp1_c1 != NULL) delete(m_img_tmp1_c1);
	if(m_img_tmp1_c3 != NULL) delete(m_img_tmp1_c3);
	if(m_img_tmp2_c1 != NULL) delete(m_img_tmp2_c1);
	if(m_img_tmp2_c3 != NULL) delete(m_img_tmp2_c3);
	
	delete comboModules ;
	delete comboStreams;
	delete layout;
	delete gbSettings;
	//vbox;
	//lab1;
	//QLabel* lab2 = new QLabel(tr("Out stream"));
	//QAction * actionShowDisplayMenu = new QAction(tr("Show display options"), this);
	//QAction * actionHideDisplayMenu = new QAction(tr("Hide display options"), this);
	//QResizeEvent* e = new QResizeEvent(QSize(x_width, x_height), size());
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
	double ratio = static_cast<double>(m_currentStream->GetInputHeight()) / m_currentStream->GetInputWidth();
	
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
	
	if(m_img_output != NULL) delete(m_img_output);
	if(m_img_original != NULL)  delete(m_img_original); 
	m_img_output = m_img_original = NULL;
	
	if(m_img_tmp1_c1 != NULL) delete(m_img_tmp1_c1);
	if(m_img_tmp1_c3 != NULL) delete(m_img_tmp1_c3);
	if(m_img_tmp2_c1 != NULL) delete(m_img_tmp2_c1);
	if(m_img_tmp2_c3 != NULL) delete(m_img_tmp2_c3);
	
	m_img_tmp1_c1 = m_img_tmp1_c3 = m_img_tmp2_c1 = m_img_tmp2_c3 = NULL;
}

/*void QModuleViewer::putImage() 
{
	//paintEvent(NULL);
	update();
}*/

void QModuleViewer::paintEvent(QPaintEvent * e) 
{
	//int cvIndex, cvLineStart;
	
	
	if(m_img_original == NULL)
		m_img_original = new Mat( cvSize(m_currentStream->GetInputWidth(), m_currentStream->GetInputHeight()), CV_8UC3);
	m_img_original->setTo(cvScalar(0,0,0));
	if(m_img_output == NULL)
		m_img_output = new Mat( cvSize(m_outputWidth, m_outputHeight), CV_8UC3);
	// Write output to screen
	// TODO : Copy below
	//cout<<"Render "<<m_currentStream->GetInputWidth()<<" to "<<m_img_original->width<<endl;
	m_currentStream->Render(m_img_original);
	if(m_img_output->channels() == 3)
		adjust(m_img_original, m_img_output, m_img_tmp1_c3, m_img_tmp2_c3);
	else
		adjust(m_img_original, m_img_output, m_img_tmp1_c1, m_img_tmp2_c1);
	
	// switch between bit depths
	/*switch (m_img_output->depth()) 
	{
		case CV_8U:
			switch (m_img_output->channels()) 
			{
				case 3:
				{
					cvIndex = 0; cvLineStart = 0;
					for (int y = 0; y < m_img_output->rows; y++) 
					{
						unsigned char red,green,blue;
						cvIndex = cvLineStart;
						for (int x = 0; x < m_img_output->cols; x++) 
						{
							// DO it
							red = m_img_output->at<Vec3w>(y, x)[1];// ->data[cvIndex+2];
							green = m_img_output->at<Vec3w>(y, x)[2];// ->data[cvIndex+2];
							blue = m_img_output->at<Vec3w>(y, x)[0];// ->data[cvIndex+2];
							//green = m_img_output->data[cvIndex+1];
							//blue = m_img_output->data[cvIndex+0];
							
							m_image.setPixel(x,y,qRgb(red, green, blue));
							cvIndex += 3;
						}
						cvLineStart += m_img_output->cols;                        
					}
				}
				break;
				default:
					throw("This number of channels is not supported\n");
					break;
			}
			break;
		default:
		{
			//cout<<m_img_output->depth()<<endl;
			throw("This type of Mat is not implemented in QModuleViewer\n");
		}
		break;
	}*/
	//imagelabel->setPixmap(QPixmap::fromImage(m_image));
	ConvertMat2QImage(m_img_output, &m_image);
	
	QPainter painter(this);
	painter.drawImage(QRect(m_offsetX, m_offsetY, m_image.width(), m_image.height()), m_image);
	
}

void QModuleViewer::updateModule(const Module * x_module)
{
	m_currentModule = x_module;
	comboStreams->clear();
	int cpt = 0;
	for(std::vector<Stream*>::const_iterator it = m_currentModule->GetStreamList().begin(); it != m_currentModule->GetStreamList().end(); it++)
	{
		comboStreams->addItem(QString((*it)->GetName().c_str()), cpt++);
	}
	assert(m_currentModule->GetStreamList().size() > 0);
	updateStream(*(m_currentModule->GetStreamList().begin()));
}

void QModuleViewer::updateStream(const Stream * x_outputStream)
{
	m_currentStream = x_outputStream;
	if(m_img_original != NULL)
	{
		delete(m_img_original);
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

void QModuleViewer::updateStream(int x_index)
{
	std::vector<Stream*>::const_iterator it = m_currentModule->GetStreamList().begin();
	
	while(x_index-- > 0 && it != m_currentModule->GetStreamList().end())
		it++;
	
	updateStream((*it));
}

void QModuleViewer::showDisplayOptions()
{
	gbSettings->show();
}

void QModuleViewer::hideDisplayOptions()
{
	gbSettings->hide();
}

QImage*  QModuleViewer::ConvertMat2QImage(const Mat *mat, QImage *qim)
{
	IplImage iplim = *mat;
	return ConvertIplImage2QImage(&iplim, qim);
}

// Based on http://umanga.wordpress.com/2010/04/19/how-to-covert-qt-qimage-into-opencv-iplimage-and-wise-versa/
QImage*  QModuleViewer::ConvertIplImage2QImage(const IplImage *iplImg, QImage *qimg)
{
	int h = iplImg->height;
	int w = iplImg->width;
	int channels = iplImg->nChannels;
	char *data = iplImg->imageData;
	
	for (int y = 0; y < h; y++, data += iplImg->widthStep)
	{
		for (int x = 0; x < w; x++)
		{
			char r, g, b, a = 0;
			if (channels == 1)
			{
				r = data[x * channels];
				g = data[x * channels];
				b = data[x * channels];
			}
			else if (channels == 3 || channels == 4)
			{
				r = data[x * channels + 2];
				g = data[x * channels + 1];
				b = data[x * channels];
			}
			
			if (channels == 4)
			{
				a = data[x * channels + 3];
				qimg->setPixel(x, y, qRgba(r, g, b, a));
			}
			else
			{
				qimg->setPixel(x, y, qRgb(r, g, b));
			}
		}
	}
	return qimg;
	
}

#include "QModuleViewer.moc"