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
#include <QMessageBox>
#include <qevent.h>

#include <QPixmap>
#include <QPainter>
#include <QListWidget>
#include <QSignalMapper>

#include "Manager.h"
#include "StreamImage.h"
#include "StreamObject.h"
#include "QControlBoard.h"
#include "ControllerParameters.h"
#include "Module.h"
#include "StreamT.h"
#include "StreamNum.h"
#include "Event.h"
#include "Object.h"
#include "util.h"

using namespace cv;
using namespace std;

log4cxx::LoggerPtr QModuleViewer::m_logger(log4cxx::Logger::getLogger("QModuleViewer"));


// template for stream creation
template<typename T>Stream* createStream(T& content, Module& module)
{
	return new StreamT<T>("viewer", content, module, "QModuleViewer stream");
}

// special cases
template<> Stream* createStream<float>(float& content, Module& module){return new StreamNum<float>("viewer", content, module, "QModuleViewer stream");}
template<> Stream* createStream<double>(double& content, Module& module){return new StreamNum<double>("viewer", content, module, "QModuleViewer stream");}
template<> Stream* createStream<int>(int& content, Module& module){return new StreamNum<int>("viewer", content, module, "QModuleViewer stream");}
template<> Stream* createStream<uint>(uint& content, Module& module){return new StreamNum<uint>("viewer", content, module, "QModuleViewer stream");}
template<> Stream* createStream<bool>(bool& content, Module& module){return new StreamNum<bool>("viewer", content, module, "QModuleViewer stream");}
template<> Stream* createStream<vector<Object>>(vector<Object>& content, Module& module){return new StreamObject("viewer", content, module, "QModuleViewer stream");}

class Viewer : public Module
{
public:
	MKCLASS("Viewer")
	MKDESCR("Viewer module used internally by QModuleViewer.")
	Viewer(ParameterStructure& xr_params, QModuleViewer& xr_gui, const string& x_parentModuleName) 
		: Module(xr_params),
		m_param(dynamic_cast<Module::Parameters&>(xr_params)),
		m_image(Size(m_param.width, m_param.height), m_param.type),
		mr_gui(xr_gui),
		m_parentModuleName(x_parentModuleName)
	{
	}
	virtual ~Viewer()
	{
		Processable::WriteLock lock(RefLock());
		if(mp_stream != nullptr)
			mp_stream->Disconnect();
	}
	void Reconnect(Manager& rx_manager, const string& x_moduleName, int x_streamId)
	{
		Processable::WriteLock lock(RefLock());
		mp_stream->Disconnect();
		rx_manager.ConnectExternalInput(*mp_stream, x_moduleName, x_streamId);
		// mp_stream->SetAsConnected(true);
	}
	void RenderTo(QImage& rx_qimage)
	{
		Processable::ReadLock lock(RefLock());
		m_image.setTo(0);
		mp_stream->RenderTo(m_image);
		assert(m_image.cols == rx_qimage.width() && m_image.rows == rx_qimage.height());
		QModuleViewer::ConvertMat2QImage(m_image, rx_qimage);
	}
	virtual void ProcessFrame() override 
	{
		mr_gui.update();
	}
	virtual void Reset() override
	{
		Processable::WriteLock lock(RefLock());
		Module::Reset();
	}
	inline const string& GetParentModuleName() const {return m_parentModuleName;}
	inline void Query(std::ostream& xr_out, const cv::Point& x_pt) const {mp_stream->Query(xr_out, x_pt);}
protected:
	const Module::Parameters& m_param;
	Stream* mp_stream = nullptr;
	Mat m_image;
	QModuleViewer& mr_gui;
	const string m_parentModuleName;
};

// Template for stream creation
template<typename T>class ViewerT : public Viewer
{
public:
	ViewerT(ParameterStructure& xr_params, QModuleViewer& xr_gui, const string& x_parentModuleName) : Viewer(xr_params, xr_gui, x_parentModuleName)
	{
		mp_stream = createStream<T>(content, *this);
		mp_stream->SetBlocking(true);
		mp_stream->SetSynchronized(true);
		AddInputStream(0, mp_stream);
	}
	virtual ~ViewerT(){}

protected:
	T content;
};

// Constructor
QModuleViewer::QModuleViewer(Manager& xr_manager, ParameterStructure& xr_params, QWidget *parent) :
	QWidget(parent),
	Configurable(xr_params),
	mr_manager(xr_manager),
	m_param(dynamic_cast<QModuleViewer::Parameters&>(xr_params))
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	// Handlers for modules
	mr_manager.ListModulesNames(m_moduleNames);
	if(m_moduleNames.empty())
		throw MkException("Module list cannot be empty", LOC);

	mp_comboModules = new QComboBox();
	mp_comboStreams = new QComboBox();
	mp_mainLayout   = new QBoxLayout(QBoxLayout::TopToBottom);
	mp_gbCombos     = new QGroupBox(tr("Display options"));
	mp_widEmpty     = new QWidget();

	auto  layoutCombos = new QGridLayout;

	// Fill the list of modules
	QLabel* lab1 = new QLabel(tr("Module"));
	layoutCombos->addWidget(lab1, 0, 0);
	mp_comboModules->clear();
	int ind = 0;
	
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
	mp_mainLayout->addWidget(mp_widEmpty, 1); // this is only for alignment

	setPalette(QPalette(QColor(20, 20, 20)));
	setAutoFillBackground(true);

	// set layout to main
	setLayout(mp_mainLayout);

	connect(mp_comboModules, SIGNAL(activated(int)), this, SLOT(updateModuleNb(int) ));
	connect(mp_comboStreams, SIGNAL(activated(int)), this, SLOT(updateStreamNb(int)));
}

QModuleViewer::~QModuleViewer()
{
	try
	{
		mr_manager.CallModuleMethod(m_param.module, [=] (Module* pmod) {pmod->RemoveDependingModule(*mp_viewerModule);});
	}
	catch(MkException &e)
	{
		LOG_WARN(m_logger, "Exception while removing module from depending: " << e.what());
	}
	delete mp_viewerModule;
	delete mp_viewerParams;
	delete mp_controlBoard;
	delete mp_comboModules;
	delete mp_comboStreams;
	delete mp_mainLayout;
	delete mp_gbCombos;
	delete mp_widEmpty;
}

void QModuleViewer::CreateInputStream(int x_outputWidth, int x_outputHeight)
{
	Processable::WriteLock lock1(mr_manager.LockModuleByName(m_param.module));
	if(mp_viewerModule != nullptr)
	{
		try
		{
			mr_manager.CallModuleMethod(mp_viewerModule->GetParentModuleName(), [=] (Module* pmod) {pmod->RemoveDependingModule(*mp_viewerModule);});
		}
		catch(MkException &e)
		{
			LOG_WARN(m_logger, "Exception while removing module from depending: " << e.what());
		}
		LOG_DEBUG(m_logger, "Delete " << mp_viewerModule);
		CLEAN_DELETE(mp_viewerModule);
	}
	CLEAN_DELETE(mp_viewerParams);
	if(x_outputWidth == 0 || x_outputHeight == 0)
		return;
	mp_viewerParams = new Module::Parameters("viewer");
	mp_viewerParams->width       = x_outputWidth;
	mp_viewerParams->height      = x_outputHeight;
	mp_viewerParams->type        = CV_8UC3;
	mp_viewerParams->autoProcess = false;

	const string type = mr_manager.GetModuleByName(m_param.module).GetOutputStreamById(m_param.stream).GetType();

	if(type == "Image")
		mp_viewerModule = new ViewerT<Mat>(*mp_viewerParams, *this, m_param.module);
	else if(type == "Objects")
		mp_viewerModule = new ViewerT<vector<Object>>(*mp_viewerParams, *this, m_param.module);
	else if(type == "Event")
		mp_viewerModule = new ViewerT<Event>(*mp_viewerParams, *this, m_param.module);
	else if(type == "NumBool")
		mp_viewerModule = new ViewerT<bool>(*mp_viewerParams, *this, m_param.module);
	else if(type == "NumInt")
		mp_viewerModule = new ViewerT<uint>(*mp_viewerParams, *this, m_param.module);
	else if(type == "NumUIt")
		mp_viewerModule = new ViewerT<int>(*mp_viewerParams, *this, m_param.module);
	else if(type == "NumFloat")
		mp_viewerModule = new ViewerT<float>(*mp_viewerParams, *this, m_param.module);
	else if(type == "NumDouble")
		mp_viewerModule = new ViewerT<double>(*mp_viewerParams, *this, m_param.module);
	else throw MkException("Unknown stream type " + type, LOC);

	LOG_DEBUG(m_logger, "Add " << mp_viewerModule << " to depending of " << m_param.module);
	mr_manager.CallModuleMethod(m_param.module, [=] (Module* pmod) {pmod->AddDependingModule(*mp_viewerModule);});

	m_qimage = QImage(x_outputWidth, x_outputHeight, QImage::Format_RGB32);
	Reconnect();
}

void QModuleViewer::Reconnect()
{
	if(mp_viewerModule == nullptr)
		return;
	mp_viewerModule->Reconnect(mr_manager, m_param.module, m_param.stream);
	mp_viewerModule->Reset();
	// process once to display the current frame
	mp_viewerModule->Process();
}

void QModuleViewer::resizeEvent(QResizeEvent * e)
{
	// Keep proportionality
	m_outputWidth  = e->size().width();
	m_outputHeight = e->size().height();
	Size outputSize;
	outputSize = mr_manager.GetModuleByName(m_param.module).GetOutputStreamById(m_param.stream).GetSize();

	double ratio = aspectRatio(outputSize);

	if(m_outputHeight >= m_outputWidth * ratio)
	{
		m_outputHeight = m_outputWidth * ratio;
		m_offsetX      = 0;
		m_offsetY      = (e->size().height() - m_outputHeight) / 2;
	}
	else
	{
		m_outputWidth = m_outputHeight / ratio;
		m_offsetX     = (e->size().width() - m_outputWidth) / 2;
		m_offsetY     = 0;
	}
	CreateInputStream(m_outputWidth, m_outputHeight);
}

void QModuleViewer::paintEvent(QPaintEvent * e)
{
	if(mp_viewerModule == nullptr)
	{
		return;
	}
	{
		try
		{
			// mp_viewerModule->Process();
		}
		catch(MkException &e)
		{
			LOG_ERROR(m_logger, "Error while processing in viewer " << e.what());
		}
	}
	mp_viewerModule->RenderTo(m_qimage);
	QPainter painter(this);
	painter.drawImage(QRect(m_offsetX, m_offsetY, m_qimage.width(), m_qimage.height()), m_qimage);
}

/// Change the module being currently displayed
void QModuleViewer::updateModule(const Module& x_module)
{
	mp_comboStreams->clear();
	m_streamIds.clear();
	m_controllerNames.clear();
	CLEAN_DELETE(mp_controlBoard);

	for(const auto & elem : x_module.GetOutputStreamList())
	{
		QString str;
		mp_comboStreams->addItem(elem.second->GetName().c_str(), elem.first);
		m_streamIds.push_back(elem.first);
	}

	for(const auto & elem : x_module.GetControllersList())
	{
		m_controllerNames.push_back(elem.first);
	}


	// Update the stream
	// if(m_param.stream > 0 && m_param.stream < mp_comboStreams->count())
	int index = IndexOfStream(m_param.stream);
	if(index > 0 && index < mp_comboStreams->count())
		mp_comboStreams->setCurrentIndex(index);
	updateStreamNb(index);

	// Empty the action menu (different for each module)
	QList<QAction *> actions = this->actions();
	for(auto & action : actions)
		this->removeAction(action);

	// Set context menus
	QAction * actionShowDisplayMenu = new QAction(tr("Show display options"), this);
	actionShowDisplayMenu->setCheckable(true);
	connect(actionShowDisplayMenu, SIGNAL(triggered(bool)), this, SLOT(showDisplayOptions(bool)));
	actionShowDisplayMenu->setChecked(m_param.displayOptions);
	actionShowDisplayMenu->setShortcut(tr("Ctrl+S"));
	this->addAction(actionShowDisplayMenu);

	// Show control board for parameters
	const map<string, Controller*>& ctrs = x_module.GetControllersList();
	int cpt = 0;

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
	if(m_moduleNames.empty())
	{
		m_param.module = "";
		return;
	}
	if(x_index < 0 || x_index >= static_cast<int>(m_moduleNames.size()))
	{
		m_param.module = m_moduleNames.at(0);
	}
	else
	{
		m_param.module = m_moduleNames.at(x_index);
	}
	updateModule(mr_manager.GetModuleByName(m_param.module));
}

/// Change the stream being currently displayed (by index)
void QModuleViewer::updateStreamNb(int x_index)
{
	if(m_streamIds.empty() || x_index < 0)
	{
		m_param.stream = -1;
		return;
	}
	if(x_index >= static_cast<int>(m_streamIds.size()))
	{
		m_param.stream = m_streamIds.at(0);
	}
	else
	{
		m_param.stream = m_streamIds.at(x_index);
	}
	CreateInputStream(m_outputWidth, m_outputHeight);
}

/// display the control with the given index
void QModuleViewer::updateControlNb(int x_index)
{
	m_param.control = x_index;
	CLEAN_DELETE(mp_controlBoard);
	if(x_index < 0 || m_controllerNames.empty())
	{
		m_param.control = -1;
		return;
	}

	if(x_index >= 0 && x_index < static_cast<int>(m_controllerNames.size()))
	{
		try
		{
			mp_controlBoard = new QControlBoard(mr_manager, m_param.module, m_controllerNames.at(x_index), this);
			mp_mainLayout->addWidget(mp_controlBoard, 0);
		}
		catch(MkException &e)
		{
			LOG_ERROR(m_logger, "Error while setting controller: " << e.what());
			m_param.control = -1;
		}
	}
	else m_param.control = -1;
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
	assert(x_mat.cols == xr_qimg.width() && x_mat.rows == xr_qimg.height());

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
	if(mp_viewerModule == nullptr)
		return;
	QPoint cursor = event->pos();
	assert(mp_viewerModule->GetWidth() == m_qimage.width());
	assert(mp_viewerModule->GetHeight() == m_qimage.height());
	Point pt(cursor.x() - m_offsetX, cursor.y() - m_offsetY); // * mp_viewerModule->GetWidth() / m_qimage.width();
	stringstream ss1;
	stringstream ss2;
	ss1 << "Query stream at " << pt;
	mp_viewerModule->Query(ss2, pt);

	QMessageBox::about(this, QString(ss1.str().c_str()), QString(ss2.str().c_str()));
}

