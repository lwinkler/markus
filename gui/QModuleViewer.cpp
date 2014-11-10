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
#include <QLabel>
#include <qevent.h>

#include <QPixmap>
#include <QPainter>
#include <QListWidget>
#include <QSignalMapper>

#include "Manager.h"
#include "Module.h"
#include "util.h"
#include "StreamImage.h"
#include "QControlBoard.h"
#include "ControllerParameters.h"

using namespace cv;
using namespace std;

// Constructor
QModuleViewer::QModuleViewer(const Manager* x_manager, ConfigReader& x_configReader, QWidget *parent) : 
	QWidget(parent),
	Configurable(x_configReader),
 	m_param(x_configReader)
{
	m_img_tmp1              = NULL; // Allocated on first conversion
	m_img_tmp2              = NULL;
	m_img_output            = NULL;
	m_img_original          = NULL;
	m_controlBoard          = NULL;

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_outputWidth  = 0.8 * width();
	m_outputHeight = 0.8 * height();
	
	// Handlers for modules
	assert(x_manager != NULL);
	if(x_manager->GetModules().size() == 0)
		throw MkException("Module list cannot be empty", LOC);
	m_manager 		= x_manager;
	m_currentModule 	= *x_manager->GetModules().begin();
	m_currentStream 	= m_currentModule->GetOutputStreamList().begin()->second;
		
	mp_comboModules 	= new QComboBox();
	mp_comboStreams 	= new QComboBox();
	mp_mainLayout 		= new QBoxLayout(QBoxLayout::TopToBottom);
	mp_gbCombos 		= new QGroupBox(tr("Display options"));
	mp_widEmpty		= new QWidget();

	QGridLayout * layoutCombos = new QGridLayout;

	// Fill the list of modules
	QLabel* lab1 = new QLabel(tr("Module"));
	layoutCombos->addWidget(lab1, 0, 0);
	mp_comboModules->clear();
	int ind = 0;
	for(std::vector<Module*>::const_iterator it = x_manager->GetModules().begin(); it != x_manager->GetModules().end(); ++it)
		mp_comboModules->addItem((*it)->GetName().c_str(), ind++);
	layoutCombos->addWidget(mp_comboModules, 0, 1);
	if(m_param.module > 0 && m_param.module < mp_comboModules->count())
		mp_comboModules->setCurrentIndex(m_param.module);
	
	QLabel* lab2 = new QLabel(tr("Out stream"));
	layoutCombos->addWidget(lab2, 1, 0);
	this->updateModuleNb(m_param.module);
	layoutCombos->addWidget(mp_comboStreams, 1, 1);

	// Create the group with combo menus
	mp_gbCombos->setLayout(layoutCombos);
	mp_gbCombos->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	
	// add widgets to main layout
	mp_mainLayout->addWidget(mp_gbCombos, 0);
	// mp_mainLayout->addWidget(mp_gbButtons, 1);
	mp_mainLayout->addWidget(mp_widEmpty, 1); // this is only for alignment
	
	setPalette(QPalette(QColor(20, 20, 20)));
	setAutoFillBackground(true);

	//update();
	m_image = QImage(m_outputWidth, m_outputHeight, QImage::Format_RGB32);
	
	// set layout to main
	setLayout(mp_mainLayout);
	
	connect(mp_comboModules, SIGNAL(activated(int)), this, SLOT(updateModuleNb(int) ));
	connect(mp_comboStreams, SIGNAL(activated(int)), this, SLOT(updateStreamNb(int)));
}

QModuleViewer::~QModuleViewer(void) 
{
	CLEAN_DELETE(m_img_original);
	CLEAN_DELETE(m_img_output);
	CLEAN_DELETE(m_img_tmp1);
	CLEAN_DELETE(m_img_tmp2);

	delete mp_comboModules ;
	delete mp_comboStreams;
	delete mp_mainLayout;
	delete mp_gbCombos;
}

void QModuleViewer::resizeEvent(QResizeEvent * e)
{
	if(m_currentStream != NULL)
	{
		// Keep proportionality
		double ratio = static_cast<double>(m_currentStream->GetHeight()) / m_currentStream->GetWidth();
	
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
	
	CLEAN_DELETE(m_img_output);
	CLEAN_DELETE(m_img_original);
	CLEAN_DELETE(m_img_tmp1);
	CLEAN_DELETE(m_img_tmp2);
}

void QModuleViewer::paintEvent(QPaintEvent * e) 
{
	if(m_currentStream != NULL)
	{
		// We paint the image from the stream
		if(m_img_original == NULL)
			m_img_original = new Mat( Size(m_currentStream->GetWidth(), m_currentStream->GetHeight()), CV_8UC3);
		m_img_original->setTo(0);
		if(m_img_output == NULL)
			m_img_output = new Mat( Size(m_outputWidth, m_outputHeight), CV_8UC3);
		
		m_currentModule->LockForRead();
		m_currentStream->RenderTo(*m_img_original);
		m_currentModule->Unlock();
		
		adjust(*m_img_original, *m_img_output, m_img_tmp1, m_img_tmp2);

		ConvertMat2QImage(m_img_output, &m_image);
		
		QPainter painter(this);
		painter.drawImage(QRect(m_offsetX, m_offsetY, m_image.width(), m_image.height()), m_image);
	}
}

/// Change the module being currently displayed
void QModuleViewer::updateModule(Module * x_module)
{
	m_currentModule = x_module;
	mp_comboStreams->clear();
	CLEAN_DELETE(m_controlBoard);

	int cpt = 0;
	for(map<int, Stream*>::const_iterator it = m_currentModule->GetOutputStreamList().begin(); it != m_currentModule->GetOutputStreamList().end(); ++it)
	{
		mp_comboStreams->addItem(it->second->GetName().c_str(), cpt++);
	}
	for(map<int, Stream*>::const_iterator it = m_currentModule->GetDebugStreamList().begin(); it != m_currentModule->GetDebugStreamList().end(); ++it)
	{
		mp_comboStreams->addItem(it->second->GetName().c_str(), cpt++);
	}
	
	// Update the stream
	updateStreamNb(m_param.stream);
	if(m_param.stream > 0 && m_param.stream < mp_comboStreams->count())
		mp_comboStreams->setCurrentIndex(m_param.stream);

	// Empty the action menu (different for each module)
	QList<QAction *> actions = this->actions();
	for(QList<QAction*>::iterator it = actions.begin() ; it != actions.end() ; ++it)
		this->removeAction(*it);

	// Set context menus
	QAction * actionShowDisplayMenu = new QAction(tr("Show display options"), this);
	actionShowDisplayMenu->setCheckable(true);
	connect(actionShowDisplayMenu, SIGNAL(triggered(bool)), this, SLOT(showDisplayOptions(bool)));
	actionShowDisplayMenu->setChecked(m_param.displayOptions);
	actionShowDisplayMenu->setShortcut(tr("Ctrl+S"));
	this->addAction(actionShowDisplayMenu);

	// Show control board for parameters
	const map<string, Controller*>& ctrs = m_currentModule->GetControllersList();
	cpt = 0;

	for(map<string, Controller*>::const_iterator it = ctrs.begin() ; it != ctrs.end() ; ++it)
	{
		// Add an action for each control associated with the module
		string name = string("Control ") + (it->second)->GetName();
		QAction * actionShowControl = new QAction(tr(name.c_str()), this);
		QSignalMapper * signalMapper = new QSignalMapper(this);
		signalMapper->setMapping(actionShowControl, cpt);
		//actionShowControl->setShortcut(tr("Ctrl+C"));
		connect(actionShowControl, SIGNAL(triggered()), signalMapper, SLOT(map()));
		connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(updateControlNb(int)));
		this->addAction(actionShowControl);
		cpt++;
	}
	if(ctrs.size() > 0)
	{
		QAction * actionShowControl = new QAction("Hide all controls", this);
		QSignalMapper * signalMapper = new QSignalMapper(this);
		signalMapper->setMapping(actionShowControl, cpt);
		connect(actionShowControl, SIGNAL(triggered()), this, SLOT(updateControlNb()));
		this->addAction(actionShowControl);
	}
	this->setContextMenuPolicy(Qt::ActionsContextMenu);

	// actionShowDisplayMenu->setChecked(m_param.displayOptions);
	showDisplayOptions(m_param.displayOptions);
	updateControlNb(m_param.control);
}

/// change the module being currently displayed (by index)
void QModuleViewer::updateModuleNb(int x_index)
{
	Module* mod = NULL;

	if(x_index < 0 || x_index >= static_cast<int>(m_manager->GetModules().size()))
	{
		mod = m_manager->GetModules().at(0);
		m_param.module = 0;
	}
	else
	{
		mod = m_manager->GetModules().at(x_index);
		m_param.module = x_index;
	}
	updateModule(mod);
}

/// Change the stream being currently displayed (by index)
void QModuleViewer::updateStreamNb(int x_index)
{
	unsigned int cpt = static_cast<unsigned int>(x_index);
	Stream* stream = NULL;

	if(x_index >= 0 && cpt < m_currentModule->GetOutputStreamList().size())
	{
		// Pick an output stream
		stream = m_currentModule->GetOutputStreamList().at(cpt);
		m_param.stream = x_index;
	}
	else
	{
		// Pick a debug stream
		cpt -= m_currentModule->GetOutputStreamList().size();
		if(cpt < m_currentModule->GetDebugStreamList().size())
		{
			stream = m_currentModule->GetDebugStreamList().at(cpt);
			m_param.stream = x_index;
		}
		else
		{
			// stream = m_currentModule->GetOutputStreamList().at(0);
			m_param.stream = 0;
		}
	}
	updateStream(stream);
}

/// display the control with the given index
void QModuleViewer::updateControlNb(int x_index)
{
	m_param.control = x_index;
	CLEAN_DELETE(m_controlBoard);
	if(x_index < 0)
		return;
	m_controlBoard = new QControlBoard(*m_currentModule, this);
	mp_mainLayout->addWidget(m_controlBoard, 0);
	map<string, Controller*>::const_iterator it = m_currentModule->GetControllersList().begin();
	unsigned int cpt = static_cast<unsigned int>(x_index);

	if(x_index >= 0 && cpt < m_currentModule->GetControllersList().size())
	{
		try
		{
			advance(it, x_index);
			m_controlBoard->updateControl(it->second);
		}
		catch(...)
		{
			m_param.control = -1;
		}
	}
	else m_param.control = -1;
}

void QModuleViewer::updateStream(Stream * x_outputStream)
{
	m_currentStream = x_outputStream;
	CLEAN_DELETE(m_img_original);
	CLEAN_DELETE(m_img_output);
}


void QModuleViewer::showDisplayOptions(bool x_isChecked)
{
	if(x_isChecked)
		mp_gbCombos->show();
	else
		mp_gbCombos->hide();
	m_param.displayOptions = x_isChecked;
}

void QModuleViewer::ConvertMat2QImage(const Mat *mat, QImage *qimg)
{

	// So far only char images are supported
	if(mat->type() != CV_8UC3 && mat->type() != CV_8UC3)
		return;

	const int & h = mat->rows;
	const int & w = mat->cols;
	const int & channels = mat->channels();
	const char *data = reinterpret_cast<const char*>(mat->data);

	for (int y = 0; y < h; y++, data += mat->cols * mat->channels())
	{
		for (int x = 0; x < w; x++)
		{
			char r = 0, g = 0, b = 0, a = 0;
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

// #include "QModuleViewer.moc"
