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
#include <QScrollBar>
#include <qevent.h>

#include <QPixmap>
#include <QPainter>

#include "Manager.h"
#include "Module.h"
#include "util.h"
#include "StreamImage.h"
#include "Control.h"

using namespace std;
using namespace cv;

// Constructor
QModuleViewer::QModuleViewer(const Manager* x_manager, QWidget *parent) : QWidget(parent)
{
	m_img_tmp1 = NULL; // Allocated on first conversion
	m_img_tmp2 = NULL;
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
	m_currentStream 	= *m_currentModule->GetOutputStreamList().begin();
	m_currentControl	= NULL;
		
	comboModules 		= new QComboBox();
	comboStreams 		= new QComboBox();
	layout 			= new QVBoxLayout;
	gbSettings 		= new QGroupBox(tr("Display options"));
	//gbSettings->setFlat(true);
	gbSettings->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	QGridLayout * vbox 	= new QGridLayout;
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
	gbControls = NULL;
	
	connect(comboModules, SIGNAL(activated(int)), this, SLOT(updateModule(int) ));
	connect(comboStreams, SIGNAL(activated(int)), this, SLOT(updateStreamOrControl(int)));
}

QModuleViewer::~QModuleViewer(void) 
{
	if(m_img_original != NULL)  delete(m_img_original); 
	if(m_img_output != NULL)  delete(m_img_output); 
	if(m_img_tmp1 != NULL) delete(m_img_tmp1);
	if(m_img_tmp2 != NULL) delete(m_img_tmp2);
	
	delete comboModules ;
	delete comboStreams;
	delete layout;
	delete gbSettings;
	if(gbControls != NULL) delete gbControls;
}

void QModuleViewer::resizeEvent(QResizeEvent * e)
{
	if(m_currentStream != NULL)
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
	}
	
	if(m_img_output != NULL) delete(m_img_output);
	if(m_img_original != NULL)  delete(m_img_original); 
	m_img_output = m_img_original = NULL;
	
	if(m_img_tmp1 != NULL) delete(m_img_tmp1);
	if(m_img_tmp2 != NULL) delete(m_img_tmp2);
	
	m_img_tmp1 = m_img_tmp2 = NULL;
}

void QModuleViewer::paintEvent(QPaintEvent * e) 
{
	if(m_currentStream != NULL)
	{
		if(gbControls != NULL) gbControls->hide(); // TODO : move this so it is lessed called
		
		// We paint the image from the stream
		if(m_img_original == NULL)
			m_img_original = new Mat( cvSize(m_currentStream->GetInputWidth(), m_currentStream->GetInputHeight()), CV_8UC3);
		m_img_original->setTo(cvScalar(0,0,0));
		if(m_img_output == NULL)
			m_img_output = new Mat( cvSize(m_outputWidth, m_outputHeight), CV_8UC3);
		// Write output to screen
		// TODO : Copy below
		//cout<<"Render "<<m_currentStream->GetInputWidth()<<" to "<<m_img_original->width<<endl;
		
		m_currentStream->RenderTo(m_img_original);
		
		adjust(m_img_original, m_img_output, m_img_tmp1, m_img_tmp2);

		ConvertMat2QImage(m_img_output, &m_image);
		
		QPainter painter(this);
		painter.drawImage(QRect(m_offsetX, m_offsetY, m_image.width(), m_image.height()), m_image);
	}
	else	
	{
		if(gbControls == NULL)
		{
			gbSettings->hide();

			// Create new control screen
			string str = m_currentModule->GetName() + ", " + m_currentControl->GetName();
			gbControls = new QGroupBox(str.c_str());
			QGridLayout * vbox = new QGridLayout;
			
			if(m_currentControl != NULL) delete m_currentControl;
			m_currentControl = new ParameterControl(m_currentModule->GetName(), m_currentModule->GetDescription(), m_currentModule->RefParameter());
			
			int cpt = 0;
			for(vector<Controller*>::iterator it = m_currentControl->RefListControllers().begin() ; it != m_currentControl->RefListControllers().end() ; it++)
			{
				QLabel * lab = new QLabel((*it)->GetName().c_str());
				vbox->addWidget(lab, cpt, 0);
				vbox->addWidget((*it)->RefWidget(), cpt, 1);
				cpt++;
			}
			vbox->setColumnStretch(1, 2);
			
			gbControls->setLayout(vbox);
			layout->addWidget(gbControls);
			setLayout(layout);
		}
	}
}

void QModuleViewer::updateModule(Module * x_module)
{
	m_currentModule = x_module;
	comboStreams->clear();
	int cpt = 0;
	for(std::vector<Stream*>::const_iterator it = m_currentModule->GetOutputStreamList().begin(); it != m_currentModule->GetOutputStreamList().end(); it++)
	{
		comboStreams->addItem(QString((*it)->GetName().c_str()), cpt++);
	}
	for(std::vector<Stream*>::const_iterator it = m_currentModule->GetDebugStreamList().begin(); it != m_currentModule->GetDebugStreamList().end(); it++)
	{
		comboStreams->addItem(QString((*it)->GetName().c_str()), cpt++);
	}
	// Add a fake streams for control
	for(std::vector<Control*>::const_iterator it = m_currentModule->GetControlList().begin(); it != m_currentModule->GetControlList().end(); it++)
	{
		comboStreams->addItem(QString((*it)->GetName().c_str()), cpt++);
	}
	
	assert(m_currentModule->GetOutputStreamList().size() > 0);
	updateStream(*(m_currentModule->GetOutputStreamList().begin()));
}

void QModuleViewer::updateModule(int x_index)
{
	std::vector<Module*>::const_iterator it = m_manager->GetModuleList().begin();
	
	while(x_index-- > 0 && it != m_manager->GetModuleList().end())
		it++;
	
	updateModule(*it);
}

void QModuleViewer::updateStreamOrControl(int x_index)
{
	unsigned int cpt = static_cast<unsigned int>(x_index);
	if(cpt < m_currentModule->GetOutputStreamList().size())
	{
		updateStream(m_currentModule->GetOutputStreamList()[cpt]);
		return;
	}

	cpt -= m_currentModule->GetOutputStreamList().size();
	if(cpt < m_currentModule->GetDebugStreamList().size())
	{
		updateStream(m_currentModule->GetDebugStreamList()[cpt]);
		return;
	}
	cpt -= m_currentModule->GetDebugStreamList().size();
	if(cpt < m_currentModule->GetControlList().size())
	{
		updateControl(m_currentModule->GetControlList()[cpt]);
		return;
	}
	
	
	assert(false);
}


void QModuleViewer::updateStream(Stream * x_outputStream)
{
	m_currentStream  = x_outputStream;
	m_currentControl = NULL;
	if(m_img_original != NULL)
	{
		delete(m_img_original);
		m_img_original = NULL;
	}
}

void QModuleViewer::updateControl(Control* x_control)
{
	m_currentStream  = NULL;
	m_currentControl = x_control;
}

void QModuleViewer::showDisplayOptions()
{
	gbSettings->show();
}

void QModuleViewer::hideDisplayOptions()
{
	gbSettings->hide();
}

void QModuleViewer::ConvertMat2QImage(const Mat *mat, QImage *qim)
{
	const IplImage iplim = *mat;
	ConvertIplImage2QImage(&iplim, qim);
}

// Based on http://umanga.wordpress.com/2010/04/19/how-to-covert-qt-qimage-into-opencv-iplimage-and-wise-versa/
void  QModuleViewer::ConvertIplImage2QImage(const IplImage *iplImg, QImage *qimg)
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
}

#include "QModuleViewer.moc"
