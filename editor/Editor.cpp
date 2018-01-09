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
#include <sstream>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <assert.h>

#include <QWebElement>
#include <QWebFrame>
#include <QAction>
#include "config.h"
#include "Editor.h"
#include "Manager.h"
#include "QWebPage2.h"
#include "MkException.h"
#include "util.h"

#include <QtWebKit>
//#include <QtWebKitWidgets>
#include <QWebView>

#include <QLabel>
#include <QTextStream>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>
// #include <QStatusBar>

namespace mk {
using namespace std;

Editor::Editor(Manager* xp_manager, const string& x_project, QWidget *parent) :
	m_projectToLoad(x_project)
{
	auto page = new QWebPage2();
	//if(xp_manager != nullptr)
		//page->mainFrame()->addToJavaScriptWindowObject("qmanager", new QManager(*xp_manager));
	m_view.setPage(page);

	setWindowState(Qt::WindowMaximized);
	setCentralWidget(&m_view);
	// m_view.show();
	// setupUi(this);
	stringstream ss;
	char pwd[256];
	char * ret = getcwd(pwd, sizeof(pwd));
	assert(ret != nullptr);
	ss<<"file://"<<pwd<<"/editor/index.html";
	// ss << "http://localhost:3000";
	m_view.load(QUrl(ss.str().c_str()));


	connect(&m_view, SIGNAL(loadFinished(bool)), this, SLOT(adaptDom(bool)));

	CreateActions();
	CreateMenus();
	setWindowTitle(tr("Markus project editor"));
	show();
}

Editor::~Editor()
{
	// delete page§

	delete aboutAct;
	delete loadProjectAct;
	delete saveProjectAct;
	delete saveProjectAsAct;
	delete updateModulesAct;

	delete fileMenu;
	delete viewMenu;
	delete helpMenu;
}

void Editor::LoadProject(const QString& x_fileName)
{
	if (!x_fileName.isEmpty())
	{
		cout<<"Open "<<x_fileName.toStdString()<<endl;
		ifstream ifs(x_fileName.toStdString().c_str());
		stringstream ss;
		if(!ifs.is_open())
		{
			throw MkException("Impossible to open project file " + x_fileName.toStdString(), LOC);
			return;
		}
		string line;
		ss << "setTimeout(function(){window.markusEditor.zone.run(function(){window.markusEditor.component.loadProject('";
		while(getline(ifs, line))
		{
			ss << line;
		}
		ifs.close();
		ss << "');})}, 1000);"; // set a timeout here. No better way for now.
		m_view.page()->mainFrame()->evaluateJavaScript(QString(ss.str().c_str()));
		m_currentProject = x_fileName;
	}
}

/// Adapt the DOM to the Qt environment
void Editor::adaptDom(bool x_loadOk)
{
	assert(x_loadOk);
	QWebFrame *frame = m_view.page()->mainFrame();
	QWebElement document = frame->documentElement();

	// Deactivate the download button // TODO: not working
	QWebElementCollection elements = document.findAll("#downloadProject");

	foreach (QWebElement element, elements)
		element.setAttribute("disabled", "disabled");

	// if a project is select load it
	LoadProject(QString(m_projectToLoad.c_str()));
}

/// Load an JSON project file
void Editor::loadProject()
{
	if(maybeSave())
	{
		QString fileName = QFileDialog::getOpenFileName(this);
		LoadProject(fileName);
	}
}


/// Check if the user wants to save its work
bool Editor::maybeSave()
{
	if(m_view.page()->mainFrame()->evaluateJavaScript("window.markusEditor.hasChanged();").toBool())
	{
		QMessageBox::StandardButton ret;
		ret = QMessageBox::warning(
			this, tr("Application"),
			tr(
				"The project has been modified.\n"
				"Do you want to save your changes?"
			),
			QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
		);
		if (ret == QMessageBox::Save)
			return save();
		else if (ret == QMessageBox::Cancel)
			return false;
	}
	return true;
}

/// Save file
bool Editor::save()
{
	if (m_currentProject.isEmpty())
	{
		return saveAs();
	}
	else
	{
		return saveProject(m_currentProject);
	}
}

/// Save file as
bool Editor::saveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this);
	if (fileName.isEmpty())
		return false;

	return saveProject(fileName);
}

/// Save a project as JSON
bool Editor::saveProject(const QString& x_fileName)
{
	QFile file(x_fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text))
	{
		QMessageBox::warning(this, tr("Application"),
							 tr("Cannot write file %1:\n%2.")
							 .arg(x_fileName)
							 .arg(file.errorString()));
		return false;
	}

	QTextStream out(&file);
#ifndef QT_NO_CURSOR
	QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
	ofstream fout(x_fileName.toStdString().c_str());
	mkjson conf = mkjson::parse(m_view.page()->mainFrame()->evaluateJavaScript("window.markusEditor.dumpProject();").toString().toStdString());
	fout << multiLine(conf) << endl;
	fout.close();
	m_currentProject = x_fileName;
#ifndef QT_NO_CURSOR
	QApplication::restoreOverrideCursor();
#endif
	// statusBar()->showMessage(tr("File saved"), 2000);
	return true;
}

/// Update list of projects
void Editor::updateModules()
{
	if(!maybeSave())
		return;
	SYSTEM("make update_modules_list");
	m_view.reload();
}

/// Print about
void Editor::about()
{
	QMessageBox::about(this, tr("About Markus editor"),
					   tr("<p>The <b>Markus</b> editor  "
						  "<p><b>Author : Laurent Winkler </b></p>"));
}

/// Create actions
void Editor::CreateActions()
{
	aboutAct = new QAction(tr("&About"), this);
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
	loadProjectAct = new QAction(tr("&Load project"), this);
	connect(loadProjectAct, SIGNAL(triggered()), this, SLOT(loadProject()));
	saveProjectAct = new QAction(tr("&Save project"), this);
	connect(saveProjectAct, SIGNAL(triggered()), this, SLOT(save()));
	saveProjectAsAct = new QAction(tr("Save project as"), this);
	connect(saveProjectAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));
	updateModulesAct = new QAction(tr("Update the list of modules"), this);
	connect(updateModulesAct, SIGNAL(triggered()), this, SLOT(updateModules()));
}

/// Create menus
void Editor::CreateMenus()
{
	fileMenu = new QMenu(tr("&File"), this);
	fileMenu->addAction(loadProjectAct);
	fileMenu->addAction(saveProjectAct);
	fileMenu->addAction(saveProjectAsAct);
	fileMenu->addSeparator();
	fileMenu->addAction(updateModulesAct);

	viewMenu = new QMenu(tr("&View"), this);

	helpMenu = new QMenu(tr("&Help"), this);
	helpMenu->addAction(aboutAct);

	menuBar()->addMenu(fileMenu);
	menuBar()->addMenu(viewMenu);
	menuBar()->addMenu(helpMenu);
}

/*
QManager::QManager(Manager& xr_manager)
	: mr_manager(xr_manager)
{
}

/// Create a module given its config
void QManager::CreateModule(QString x_jsonString) // TODO
{
	assert(false);
	// ConfigString config(x_xmlString.toStdString());
	// mr_manager.BuildModule(config);
	// Connect();
	// Reset();
	// Start();
}
*/
}
