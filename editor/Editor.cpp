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
#include "Editor.h"

#include <QtWebKit>
//#include <QtWebKitWidgets>
#include <QWebView>

#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>
// #include <QStatusBar>

using namespace std;

Editor::Editor(QWidget *parent)
{
	setWindowState(Qt::WindowMaximized);
	setCentralWidget(&m_view);
	// m_view.show();
	// setupUi(this);
	stringstream ss;
	char pwd[256];
	assert(getcwd(pwd, sizeof(pwd)) != NULL);
	ss<<"file://"<<pwd<<"/editor.html";
	m_view.load(QUrl(ss.str().c_str()));

	connect(&m_view, SIGNAL(loadFinished(bool)), this, SLOT(adaptDom(bool)));

	CreateActions();
	CreateMenus();
	setWindowTitle(tr("Markus project editor"));
	show();
}

/// Adapt the DOM to the Qt environment
void Editor::adaptDom(bool x_loadOk)
{
	assert(x_loadOk); // TODO Exception
	QWebFrame *frame = m_view.page()->mainFrame();
	QWebElement document = frame->documentElement();

	// Deactivate the download button
	QWebElementCollection elements = document.findAll("#downloadProject");

	foreach (QWebElement element, elements)
		element.setAttribute("disabled", "disabled");
}

/// Load an XML project file
void Editor::loadProject()
{
	if(maybeSave())
	{
		QString fileName = QFileDialog::getOpenFileName(this);
		if (!fileName.isEmpty())
		{
			m_view.page()->mainFrame()->evaluateJavaScript("window.markusEditor.loadProjectFile(\"" + fileName + "\"); null");
			m_currentProject = fileName;
		}
	}
}


/// Check if the user wants to save its work
bool Editor::maybeSave()
{
	if (true)// textEdit->document()->isModified())
	{
		QMessageBox::StandardButton ret;
		ret = QMessageBox::warning(this, tr("Application"),
				tr("The project has been modified.\n"
					"Do you want to save your changes?"),
				QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
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

/// Save a project as XML
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
	std::ofstream fout(x_fileName.toStdString().c_str());
	fout<<m_view.page()->mainFrame()->evaluateJavaScript("window.markusEditor.saveProject();").toString().toStdString()<<endl;
	fout.close();
	m_currentProject = x_fileName;
#ifndef QT_NO_CURSOR
	QApplication::restoreOverrideCursor();
#endif
	// statusBar()->showMessage(tr("File saved"), 2000);
	return true;
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
}

/// Create menus
void Editor::CreateMenus()
{
	fileMenu = new QMenu(tr("&File"), this);
	fileMenu->addAction(loadProjectAct);
	fileMenu->addAction(saveProjectAct);
	fileMenu->addAction(saveProjectAsAct);
	
	
	viewMenu = new QMenu(tr("&View"), this);
	// viewMenu->addAction(viewDisplayOptionsAct);
	// viewMenu->addAction(view1x1Act);
	// viewMenu->addAction(view1x2Act);
	// viewMenu->addAction(view2x2Act);
	// viewMenu->addAction(view2x3Act);
	// viewMenu->addAction(view3x3Act);
	// viewMenu->addAction(view3x4Act);
	// viewMenu->addAction(view4x4Act);
	
	helpMenu = new QMenu(tr("&Help"), this);
	helpMenu->addAction(aboutAct);
	
	menuBar()->addMenu(fileMenu);
	menuBar()->addMenu(viewMenu);
	menuBar()->addMenu(helpMenu);
}
