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
#include <QPushButton>
#include <QLabel>
#include <qevent.h>

#include <QPixmap>
#include <QPainter>
#include <QScrollArea>
#include <QListWidget>

#include "Manager.h"
#include "Module.h"
#include "util.h"
#include "StreamImage.h"
#include "Control.h"

#define CLEAN_DELETE(x) if((x) != NULL){delete((x));(x) = NULL}

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
	m_currentControl 	= NULL;
		
	mp_comboModules 	= new QComboBox();
	mp_comboStreams 	= new QComboBox();
	mp_mainLayout 		= new QBoxLayout(QBoxLayout::TopToBottom);
	mp_gbCombos 		= new QGroupBox(tr("Display options"));
	mp_gbControls		= new QScrollArea;
	mp_gbButtons		= new QGroupBox(tr("Parameter options"));
	mp_widEmpty		= new QWidget();

	QGridLayout * layoutCombos = new QGridLayout;

	// Fill the list of modules
	QLabel* lab1 = new QLabel(tr("Module"));
	layoutCombos->addWidget(lab1,0,0);
	mp_comboModules->clear();
	int ind = 0;
	for(std::vector<Module*>::const_iterator it = x_manager->GetModuleList().begin(); it != x_manager->GetModuleList().end(); it++)
		mp_comboModules->addItem((*it)->GetName().c_str(), ind++);
	layoutCombos->addWidget(mp_comboModules,0,1);
	
	QLabel* lab2 = new QLabel(tr("Out stream"));
	layoutCombos->addWidget(lab2,1,0);
	this->updateModule(*(x_manager->GetModuleList().begin()));
	layoutCombos->addWidget(mp_comboStreams,1,1);

	// Create the group with combo menus
	mp_gbCombos->setLayout(layoutCombos);
	mp_gbCombos->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	
	// Create the group with control buttons (for settings)
	mp_buttonGetCurrentControl  = new QPushButton(tr("Get current"));
	mp_buttonGetDefaultControl  = new QPushButton(tr("Get default"));
	mp_buttonSetControl         = new QPushButton(tr("Set"));
	mp_buttonResetModule        = new QPushButton(tr("Reset module"));
	

	// Create the group with settings buttons	
	QHBoxLayout * buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(mp_buttonGetCurrentControl);
	buttonLayout->addWidget(mp_buttonGetDefaultControl);
	buttonLayout->addWidget(mp_buttonSetControl);
	buttonLayout->addWidget(mp_buttonResetModule);
	mp_gbButtons->setLayout(buttonLayout);

	// add widgets to main layout
	mp_mainLayout->addWidget(mp_gbCombos, 0);
	mp_mainLayout->addWidget(mp_gbControls, 0);
	mp_mainLayout->addWidget(mp_gbButtons, 1);
	mp_mainLayout->addWidget(mp_widEmpty, 2); // this is only for alignment
	
	
	// Set context menus
	QAction * actionShowDisplayMenu = new QAction(tr("Show display options"), this);
	actionShowDisplayMenu->setShortcut(tr("Ctrl+S"));
	QAction * actionHideDisplayMenu = new QAction(tr("Hide display options"), this);
	actionHideDisplayMenu->setShortcut(tr("Ctrl+H"));
	connect(actionShowDisplayMenu, SIGNAL(triggered()), this, SLOT(showDisplayOptions()));
	connect(actionHideDisplayMenu, SIGNAL(triggered()), this, SLOT(hideDisplayOptions()));

	this->addAction(actionShowDisplayMenu);
	this->addAction(actionHideDisplayMenu);
	this->setContextMenuPolicy(Qt::ActionsContextMenu);
	
	setPalette(QPalette(QColor(0, 0, 0)));
	setAutoFillBackground(true);

	//update();
	m_image = QImage(m_outputWidth, m_outputHeight, QImage::Format_RGB32);
	
	// set layout to main
	setLayout(mp_mainLayout);
	
	connect(mp_comboModules, SIGNAL(activated(int)), this, SLOT(updateModuleNb(int) ));
	connect(mp_comboStreams, SIGNAL(activated(int)), this, SLOT(updateStreamOrControlNb(int)));
	connect(mp_buttonGetCurrentControl, SIGNAL(pressed()), this, SLOT(getCurrentControl(void)));
	connect(mp_buttonGetDefaultControl, SIGNAL(pressed()), this, SLOT(getDefaultControl(void)));
	connect(mp_buttonSetControl, SIGNAL(pressed()), this, SLOT(SetControlledValue(void)));
	connect(mp_buttonResetModule, SIGNAL(pressed()), this, SLOT(resetModule(void)));
}

QModuleViewer::~QModuleViewer(void) 
{
	if(m_img_original != NULL)  delete(m_img_original); 
	if(m_img_output != NULL)  delete(m_img_output); 
	if(m_img_tmp1 != NULL) delete(m_img_tmp1);
	if(m_img_tmp2 != NULL) delete(m_img_tmp2);
	
	delete mp_comboModules ;
	delete mp_comboStreams;
	delete mp_mainLayout;
	delete mp_gbCombos;
	if(mp_gbControls != NULL) delete mp_gbControls;
	if(mp_gbButtons != NULL) delete mp_gbButtons;
	if(mp_widEmpty != NULL) delete mp_widEmpty;
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
		// We paint the image from the stream
		if(m_img_original == NULL)
			m_img_original = new Mat( cvSize(m_currentStream->GetInputWidth(), m_currentStream->GetInputHeight()), CV_8UC3);
		m_img_original->setTo(0);
		if(m_img_output == NULL)
			m_img_output = new Mat( cvSize(m_outputWidth, m_outputHeight), CV_8UC3);
		
		m_currentStream->RenderTo(m_img_original);
		
		adjust(m_img_original, m_img_output, m_img_tmp1, m_img_tmp2);

		ConvertMat2QImage(m_img_output, &m_image);
		
		QPainter painter(this);
		painter.drawImage(QRect(m_offsetX, m_offsetY, m_image.width(), m_image.height()), m_image);
	}
}

void QModuleViewer::updateModule(Module * x_module)
{
	m_currentModule = x_module;
	mp_comboStreams->clear();
	int cpt = 0;
	for(std::vector<Stream*>::const_iterator it = m_currentModule->GetOutputStreamList().begin(); it != m_currentModule->GetOutputStreamList().end(); it++)
	{
		mp_comboStreams->addItem((*it)->GetName().c_str(), cpt++);
	}
	for(std::vector<Stream*>::const_iterator it = m_currentModule->GetDebugStreamList().begin(); it != m_currentModule->GetDebugStreamList().end(); it++)
	{
		mp_comboStreams->addItem((*it)->GetName().c_str(), cpt++);
	}
	// Add a fake stream for control
	for(std::vector<ParameterControl*>::const_iterator it = m_currentModule->GetControlList().begin(); it != m_currentModule->GetControlList().end(); it++)
	{
		mp_comboStreams->addItem((*it)->GetName().c_str(), cpt++);
	}
	
	assert(m_currentModule->GetOutputStreamList().size() > 0);
	updateStream(*(m_currentModule->GetOutputStreamList().begin()));
}

void QModuleViewer::updateModuleNb(int x_index)
{
	std::vector<Module*>::const_iterator it = m_manager->GetModuleList().begin();
	
	while(x_index-- > 0 && it != m_manager->GetModuleList().end())
		it++;
	
	updateModule(*it);
}

void QModuleViewer::updateStreamOrControlNb(int x_index)
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

	//if(mp_gbControls != NULL)
	{
		mp_gbControls->hide();
		mp_gbButtons->hide();
		//mp_widEmpty->show();
	}
}

void QModuleViewer::updateControl(ParameterControl* x_control)
{
	m_currentStream  = NULL;
	m_currentControl = x_control;
	static_cast<ParameterControl*>(m_currentControl)->SetParameterStructure(m_currentModule->RefParameter());

	/// Create new control screen
	mp_gbControls->setWidgetResizable(true);
	QGridLayout * vbox = new QGridLayout;

	// Add controls (= parameters)
	int cpt = 0;
	for(vector<Controller*>::iterator it = m_currentControl->RefListControllers().begin() ; it != m_currentControl->RefListControllers().end() ; it++)
	{
		QLabel * lab = new QLabel((*it)->GetName().c_str());
		vbox->addWidget(lab, cpt, 0);
		vbox->addWidget((*it)->RefWidget(), cpt, 1);
		cpt++;
	}

	QWidget *viewport = new QWidget;
	viewport->setLayout(vbox);
	viewport->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	mp_gbControls->setWidget(viewport);

	//mp_widEmpty->hide();
	mp_gbControls->show();
	mp_gbButtons->show();
}

void QModuleViewer::SetControlledValue()
{
	if(m_currentControl != NULL)
	{
		m_currentControl->SetControlledValue();
	}
}

void QModuleViewer::resetModule()
{
	m_currentModule->Reset();
}

void QModuleViewer::getCurrentControl()
{
	if(m_currentControl != NULL)
	{
		m_currentControl->GetCurrent();
	}
}

void QModuleViewer::getDefaultControl()
{
	if(m_currentControl != NULL)
	{
		m_currentControl->GetDefault();
	}
}

void QModuleViewer::showDisplayOptions()
{
	mp_gbCombos->show();
}

void QModuleViewer::hideDisplayOptions()
{
	mp_gbCombos->hide();
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
