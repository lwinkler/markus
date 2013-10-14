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

#ifndef QPARAMETERSLIDER_H
#define QPARAMETERSLIDER_H

#include <QWidget>
#include <QGroupBox>

class QScrollBar;
class QLineEdit;
class QBoxLayout;

/// Visual Qt object. This class is used to set a parameter value using a visual slider. It is used for parameter of integer and double type

class QParameterSlider : public QWidget
{
	Q_OBJECT
public:
	QParameterSlider(double x_value, double x_min, double x_max, int x_precision, QWidget *parent = 0);
	virtual ~QParameterSlider();
	inline double GetValue() const {return m_value;}
	void SetValue(double x_value);

private:
	double       m_value;
	int          m_divisionPerUnit;
	char         m_format[16];
	QBoxLayout * m_boxLayout;
	QScrollBar * m_scrollBar;
	QLineEdit  * m_lineEdit;

public slots:
	void SetValueFromSlider();
	void SetValueFromText();
};

#endif
