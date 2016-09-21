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
#include "StreamObject.h"
#include "QControlBoard.h"
#include "ControllerParameters.h"

using namespace cv;
using namespace std;

// Constructor
QModuleViewer::QModuleViewer(Manager& xr_manager, ParameterStructure& xr_params, QWidget *parent) :
	QWidget(parent),
	Configurable(xr_params),
	mr_manager(xr_manager),
	m_param(dynamic_cast<QModuleViewer::Parameters&>(xr_params))
{
	m_controlBoard          = nullptr;

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_outputWidth  = 0.8 * width();
	m_outputHeight = 0.8 * height();

	// Handlers for modules
	mr_manager.ListModulesNames(m_moduleNames);
	if(m_moduleNames.size() == 0)
		throw MkException("Module list cannot be empty", LOC);

	mp_comboModules 	= new QComboBox();
	mp_comboStreams 	= new QComboBox();
	mp_mainLayout 		= new QBoxLayout(QBoxLayout::TopToBottom);
	mp_gbCombos 		= new QGroupBox(tr("Display options"));
	mp_widEmpty		= new QWidget();

	auto  layoutCombos = new QGridLayout;

	// Fill the list of modules
	QLabel* lab1 = new QLabel(tr("Module"));
	layoutCombos->addWidget(lab1, 0, 0);
	mp_comboModules->clear();
	int ind = 0;
	assert(!m_moduleNames.empty());
	for(const auto & elem : m_moduleNames)
		mp_comboModules->addItem(elem.c_str(), ind++);
	layoutCombos->addWidget(mp_comboModules, 0, 1);
	int index = IndexOfModule(m_param.module);
	if(index > 0 && index < mp_comboModules->count())
		mp_comboModules->setCurrentIndex(index);

	QLabel* lab2 = new QLabel(tr("Out stream"));
	layoutCombos->addWidget(lab2, 1, 0);
	this->updateModuleNb(index);
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

QModuleViewer::~QModuleViewer()
{
	delete mp_comboModules ;
	delete mp_comboStreams;
	delete mp_mainLayout;
	delete mp_gbCombos;
}

void QModuleViewer::CreateStream()
{
	// (mr_manager.GetModuleByName(m_param.module).GetOutputStreamById(m_param.stream).GetType());
	cout << "ici " << __LINE__ << endl;
	if(mp_stream != nullptr)
		mp_stream->Disconnect();
	cout << "ici " << __LINE__ << endl;
	CLEAN_DELETE(mp_viewerParams);
	cout << "ici " << __LINE__ << endl;
	CLEAN_DELETE(mp_viewerModule);
	cout << "ici " << __LINE__ << endl;
	CLEAN_DELETE(mp_stream);
	cout << "ici " << __LINE__ << endl;
	CLEAN_DELETE(mp_contentSerializable);
	cout << "ici " << __LINE__ << endl;
	mp_viewerParams = new Module::Parameters("fake");
	mp_viewerParams->width  = m_outputWidth;
	mp_viewerParams->height = m_outputHeight;
	cout << "ici " << __LINE__ << endl;
	mp_viewerParams->autoProcess = false;
	cout << "ici " << __LINE__ << endl;
	mp_viewerModule = new Viewer(*mp_viewerParams);
	cout << "ici " << __LINE__ << endl;
	mp_stream = new StreamImage("test", m_contentImage, *mp_viewerModule, "Fake stream");
	mr_manager.ConnectExternalInput(*mp_stream, m_param.module, m_param.stream);
	cout << "ici " << __LINE__ << endl;
	mp_viewerModule->Reset();
	cout << "ici " << __LINE__ << endl;
}

void QModuleViewer::resizeEvent(QResizeEvent * e)
{
	CreateStream();

	// Keep proportionality
	double ratio = static_cast<double>(mp_stream->GetHeight()) / mp_stream->GetWidth();

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

	m_image = QImage(m_outputWidth, m_outputHeight, QImage::Format_RGB32);
}

void QModuleViewer::paintEvent(QPaintEvent * e)
{
	if(mp_viewerModule != nullptr)
	{
		{
			// note: do we lock the module before reading ? this may be dangerous and may lead to
			//       corrupted images, but avoids two problems:
			//        - slow interaction with the GUI
			//        - no image is displayed if we only try to lock and the module is busy
			//       A solution could be to display asynchronously
			// TODO Processable::ReadLock lock(m_currentModule->RefLock(), boost::try_to_lock);
			// We paint the image from the stream
			mp_stream->ConvertInput();
			mp_stream->RenderTo(m_contentImage);
		}

		ConvertMat2QImage(m_contentImage, m_image);

		QPainter painter(this);
		painter.drawImage(QRect(m_offsetX, m_offsetY, m_image.width(), m_image.height()), m_image);
	}
}

/// Change the module being currently displayed
void QModuleViewer::updateModule(const Module& x_module)
{
	// TODO: Lock module for safety
	// m_currentModule = x_module;
	mp_comboStreams->clear();
	CLEAN_DELETE(m_controlBoard);

	int cpt = 0;
	for(const auto & elem : x_module.GetOutputStreamList())
	{
		QString str;
		mp_comboStreams->addItem(elem.second->GetName().c_str(), str);
	}
	cout << "ici " << __LINE__ << endl;
	for(const auto & elem : x_module.GetDebugStreamList())
	{
		mp_comboStreams->addItem(elem.second->GetName().c_str(), cpt++);
	}

	cout << "ici " << __LINE__ << endl;
	// Update the stream
	updateStreamNb(m_param.stream);
	if(m_param.stream > 0 && m_param.stream < mp_comboStreams->count())
		mp_comboStreams->setCurrentIndex(m_param.stream);

	cout << "ici " << __LINE__ << endl;
	// Empty the action menu (different for each module)
	QList<QAction *> actions = this->actions();
	for(auto & action : actions)
		this->removeAction(action);

	cout << "ici " << __LINE__ << endl;
	// Set context menus
	QAction * actionShowDisplayMenu = new QAction(tr("Show display options"), this);
	actionShowDisplayMenu->setCheckable(true);
	connect(actionShowDisplayMenu, SIGNAL(triggered(bool)), this, SLOT(showDisplayOptions(bool)));
	actionShowDisplayMenu->setChecked(m_param.displayOptions);
	actionShowDisplayMenu->setShortcut(tr("Ctrl+S"));
	this->addAction(actionShowDisplayMenu);

	// Show control board for parameters
	const map<string, Controller*>& ctrs = x_module.GetControllersList();
	cpt = 0;

	// Add option to right-click: controls
	for(const auto & ctr : ctrs)
	{
		// Add an action for each control associated with the module
		string name = string("Control ") + (ctr.second)->GetName();
		QAction * actionShowControl = new QAction(tr(name.c_str()), this);
		auto  signalMapper = new QSignalMapper(this);
		signalMapper->setMapping(actionShowControl, cpt);
		//actionShowControl->setShortcut(tr("Ctrl+C"));
		connect(actionShowControl, SIGNAL(triggered()), signalMapper, SLOT(map()));
		connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(updateControlNb(int)));
		this->addAction(actionShowControl);
		cpt++;
	}

	if(cpt != 0)
	{
		QAction * actionShowControl = new QAction("Hide all controls", this);
		auto  signalMapper = new QSignalMapper(this);
		signalMapper->setMapping(actionShowControl, cpt);
		connect(actionShowControl, SIGNAL(triggered()), this, SLOT(updateControlNb()));
		this->addAction(actionShowControl);
		cpt++;
	}
	this->setContextMenuPolicy(Qt::ActionsContextMenu);

	// actionShowDisplayMenu->setChecked(m_param.displayOptions);
	showDisplayOptions(m_param.displayOptions);
	updateControlNb(m_param.control);
}

/// change the module being currently displayed (by index)
void QModuleViewer::updateModuleNb(int x_index)
{
	// Module* mod = nullptr;

	cout << "ici " << __LINE__ << endl;
	cout << "ici " << __LINE__ << endl;
	if(x_index < 0 || x_index >= static_cast<int>(m_moduleNames.size()))
	{
		m_param.module = m_moduleNames.at(0);
	}
	else
	{
		m_param.module = m_moduleNames.at(x_index);
	}
	cout << "ici " << __LINE__ << endl;
	// TODO: lock manager
	updateModule(mr_manager.GetModuleByName(m_param.module));
	cout << "ici " << __LINE__ << endl;
}

/// Change the stream being currently displayed (by index)
void QModuleViewer::updateStreamNb(int x_index)
{
	unsigned int cpt = static_cast<unsigned int>(x_index);
	cout << "ici " << __LINE__ << endl;

	const Module& module(mr_manager.GetModuleByName(m_param.module));
	cout << "ici " << __LINE__ << endl;

	if(x_index >= 0 && cpt < module.GetOutputStreamList().size())
	{
		// Pick an output stream
		// stream = module.GetOutputStreamList().at(cpt);
		m_param.stream = x_index;
	}
	else
	{
		// Pick a debug stream
		cpt -= module.GetOutputStreamList().size();
		if(cpt < module.GetDebugStreamList().size())
		{
			m_param.stream = x_index;
		}
		else
		{
			m_param.stream = 0;
		}
	}
	cout << "ici " << __LINE__ << endl;
	CreateStream();
	cout << "ici " << __LINE__ << endl;
}

/// display the control with the given index
void QModuleViewer::updateControlNb(int x_index)
{
	/*
	m_param.control = x_index;
	CLEAN_DELETE(m_controlBoard);
	if(x_index < 0)
		return;
	m_controlBoard = new QControlBoard(this);
	mp_mainLayout->addWidget(m_controlBoard, 0);
	auto it = m_currentModule->GetControllersList().begin();
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
	*/
}

void QModuleViewer::showDisplayOptions(bool x_isChecked)
{
	if(x_isChecked)
		mp_gbCombos->show();
	else
		mp_gbCombos->hide();
	m_param.displayOptions = x_isChecked;
}

void QModuleViewer::ConvertMat2QImage(const Mat& x_mat, QImage& xr_qimg)
{

	// So far only char images are supported
	if(x_mat.type() != CV_8UC1 && x_mat.type() != CV_8UC3)
	{
		printf("Warning: unsupported image type to be displayed");
		return;
	}

	const int & h = x_mat.rows;
	const int & w = x_mat.cols;
	const int & channels = x_mat.channels();
	const char *data = reinterpret_cast<const char*>(x_mat.data);

	for (int y = 0; y < h; y++, data += x_mat.cols * x_mat.channels())
	{
		for (int x = 0; x < w; x++)
		{
			char r = 0, g = 0, b = 0;
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
				char a = data[x * channels + 3];
				xr_qimg.setPixel(x, y, qRgba(r, g, b, a));
			}
			else
			{
				xr_qimg.setPixel(x, y, qRgb(r, g, b));
			}
		}
	}
}

// Display some info on stream (and position of cursor)
void QModuleViewer::mouseDoubleClickEvent(QMouseEvent * event)
{
	/*
	if(m_currentStream == nullptr)
		return;
	QPoint cursor = event->pos();
	int x = (cursor.x() - m_offsetX) * m_currentStream->GetWidth() / m_image.width();
	int y = (cursor.y() - m_offsetY) * m_currentStream->GetHeight() / m_image.height();
	m_currentStream->Query(x, y);
	*/
}

// #include "QModuleViewer.moc"
