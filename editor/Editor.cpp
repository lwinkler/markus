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
#include <iostream>
#include <unistd.h>
#include <assert.h>

#include <QWebElement>
#include <QWebFrame>
#include "Editor.h"

// #include <QtWebKit>
#include <QtWebKitWidgets/QtWebKitWidgets>
#include <QtWebKitWidgets/QWebView>

using namespace std;

Editor::Editor(QWidget *parent)
    : QWidget(parent)
{
	// setupUi(this);
	stringstream ss;
	char pwd[256];
	assert(getcwd(pwd, sizeof(pwd)) != NULL);
	ss<<"file://"<<pwd<<"/editor.html";
	m_view.load(QUrl(ss.str().c_str()));
	m_view.show();

	connect(&m_view, SIGNAL(loadFinished(bool)), this, SLOT(AdaptDom(bool)));
}

/// Adapt the DOM to the Qt environment
void Editor::AdaptDom(bool x_loadOk)
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
void Editor::LoadProject(QString x_file)
{
}

/// Save a project as XML
void Editor::SaveProject(QString x_file)
{
}
