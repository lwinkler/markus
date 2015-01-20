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


#include "QWebPage2.h"

#include <QDebug>
#include <QTextStream>


/// This class is simply used to log of the webpage directly to the console output

QWebPage2::QWebPage2(QObject *parent): QWebPage(parent) {}

void QWebPage2::javaScriptConsoleMessage(const QString& message, int lineNumber, const QString& sourceID)
{
	QString logEntry = message +" on line:"+ QString::number(lineNumber) +" Source:"+ sourceID;
	qDebug()<<logEntry;
}
