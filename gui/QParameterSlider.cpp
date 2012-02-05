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

#include "QParameterSlider.h"
#include <cstdio>

#include <QBoxLayout>
#include <QScrollBar>
#include <QLineEdit>
#include <QSlider>
#include <QLabel>


using namespace std;

QParameterSlider::QParameterSlider(double x_value, double x_min, double x_max, int x_precision, QWidget *parent):
	m_value(x_value)
{
	m_divisionPerUnit = 1;
	for(int tmp = x_precision ; tmp ; tmp--)
		m_divisionPerUnit *= 10;

	m_boxLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	char str[16];
	sprintf(m_format, "%%.%dlf", x_precision);

	m_lineEdit      = new QLineEdit();
	sprintf(str, m_format, m_value);
	m_lineEdit->setText(str);
	m_lineEdit->setStyleSheet("color: black; background-color: white");
	m_boxLayout->addWidget(m_lineEdit);

	sprintf(str, m_format, x_min);
	m_boxLayout->addWidget(new QLabel(str));

	m_scrollBar 	= new QScrollBar(Qt::Horizontal);
	m_scrollBar->setMinimum(x_min * m_divisionPerUnit);
	m_scrollBar->setMaximum(x_max * m_divisionPerUnit);
	m_scrollBar->setValue(m_value * m_divisionPerUnit);
	m_scrollBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	m_boxLayout->addWidget(m_scrollBar);

	sprintf(str, m_format, x_max);
	m_boxLayout->addWidget(new QLabel(str));

	setLayout(m_boxLayout);

	connect(m_scrollBar, SIGNAL(valueChanged(int)), this, SLOT(SetValueFromSlider()));
	connect(m_lineEdit, SIGNAL(textEdited(const QString &)), this, SLOT(SetValueFromText()));
}

QParameterSlider::~QParameterSlider()
{
	delete(m_boxLayout);
	delete(m_lineEdit);
	delete(m_scrollBar);
}

void QParameterSlider::SetValueFromSlider()
{
	m_value = static_cast<double>(m_scrollBar->value()) / m_divisionPerUnit;
	char str[16];
	sprintf(str, m_format, m_value);
	m_lineEdit->setText(str);
}

void QParameterSlider::SetValueFromText()
{
	m_value = m_lineEdit->text().toDouble();
	m_scrollBar->setValue(m_value * m_divisionPerUnit);
}

#include "QParameterSlider.moc"
