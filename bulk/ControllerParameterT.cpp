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

#include "ControllerParameterT.h"
#include "Polygon.h"
#include "Processable.h"
#include <boost/lexical_cast.hpp>


#ifndef MARKUS_NO_GUI
#include <QWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QComboBox>
#include "QParameterSlider.h"
#endif

namespace mk {
using namespace std;

#define PRECISION_DOUBLE 2



template<> QWidget* ControllerInt::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	return mp_widget = new QParameterSlider(m_param.GetValue().get<int>(), m_param.GetRange().at("min").get<double>(), m_param.GetRange().at("max").get<double>(), 0);
#else
	return nullptr;
#endif
}


template<> void ControllerInt::SetWidgetValue(const string& x_value)
{
#ifndef MARKUS_NO_GUI
	dynamic_cast<QParameterSlider*>(mp_widget)->SetValue(boost::lexical_cast<int>(x_value));
#endif
}

template<> mkjson ControllerInt::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	return dynamic_cast<QParameterSlider*>(mp_widget)->GetValue();
#else
	assert(false);
	return "";
#endif
}

/*------------------------------------------------------------------------------------------------*/

template<> QWidget* ControllerUInt::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	return mp_widget = new QParameterSlider(m_param.GetValue().get<uint>(), m_param.GetRange().at("min").get<double>(), m_param.GetRange().at("max").get<double>(), 0);
#else
	return nullptr;
#endif
}


template<> void ControllerUInt::SetWidgetValue(const string& x_value)
{
#ifndef MARKUS_NO_GUI
	dynamic_cast<QParameterSlider*>(mp_widget)->SetValue(boost::lexical_cast<int>(x_value));
#endif
}

template<> mkjson ControllerUInt::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	return dynamic_cast<QParameterSlider*>(mp_widget)->GetValue();
#else
	assert(false);
	return "";
#endif
}
/*------------------------------------------------------------------------------------------------*/

template<> QWidget* ControllerDouble::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	return mp_widget = new QParameterSlider(m_param.GetValue().get<double>(), m_param.GetRange().at("min").get<double>(), m_param.GetRange().at("max").get<double>(), PRECISION_DOUBLE);
#else
	return nullptr;
#endif
}

template<> void ControllerDouble::SetWidgetValue(const string& x_value)
{
#ifndef MARKUS_NO_GUI
	dynamic_cast<QParameterSlider*>(mp_widget)->SetValue(boost::lexical_cast<double>(x_value));
#endif
}

template<> mkjson ControllerDouble::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	return dynamic_cast<QParameterSlider*>(mp_widget)->GetValue();
#else
	assert(false);
	return "";
#endif
}
/*------------------------------------------------------------------------------------------------*/


template<> QWidget* ControllerFloat::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	return mp_widget = new QParameterSlider(m_param.GetValue().get<float>(), m_param.GetRange().at("min").get<double>(), m_param.GetRange().at("max").get<double>(), PRECISION_DOUBLE);
#else
	return nullptr;
#endif
}

template<> void ControllerFloat::SetWidgetValue(const string& x_value)
{
#ifndef MARKUS_NO_GUI
	dynamic_cast<QParameterSlider*>(mp_widget)->SetValue(boost::lexical_cast<double>(x_value));
#endif
}

template<> mkjson ControllerFloat::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	return dynamic_cast<QParameterSlider*>(mp_widget)->GetValue();
#else
	assert(false);
	return "";
#endif
}
/*------------------------------------------------------------------------------------------------*/
template<> QWidget* ControllerBool::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	mp_widget = new QCheckBox("Enabled");
	dynamic_cast<QCheckBox*>(mp_widget)->setChecked(m_param.GetValue().get<int>());
	return mp_widget;
#else
	return nullptr;
#endif
}

template<> void ControllerBool::SetWidgetValue(const string& x_value)
{
#ifndef MARKUS_NO_GUI
	dynamic_cast<QCheckBox*>(mp_widget)->setChecked(x_value == "1");
#endif
}

template<> mkjson ControllerBool::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	return dynamic_cast<QCheckBox*>(mp_widget)->isChecked();
#else
	assert(false);
	return "";
#endif
}
/*------------------------------------------------------------------------------------------------*/
template<> QWidget* ControllerString::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	QLineEdit* lineEdit = new QLineEdit();
	lineEdit->setStyleSheet("color: black; background-color: white");
	lineEdit->setText(m_param.GetValue().get<string>().c_str());
	mp_widget = lineEdit;
	return lineEdit;
#else
	return nullptr;
#endif
}
template<> void ControllerString::SetWidgetValue(const string& x_value)
{
#ifndef MARKUS_NO_GUI
	dynamic_cast<QLineEdit*>(mp_widget)->setText(x_value.c_str());
#endif
}

template<> mkjson ControllerString::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	return dynamic_cast<QLineEdit*>(mp_widget)->text().toStdString();
#else
	assert(false);
	return "";
#endif
}

/*------------------------------------------------------------------------------------------------*/
QWidget* defaultCreateWidget(Parameter& rx_param, QWidget*& rpx_widget)
{
#ifndef MARKUS_NO_GUI
	QLineEdit* lineEdit = new QLineEdit();
	lineEdit->setStyleSheet("color: black; background-color: white");
	lineEdit->setText(oneLine(rx_param.GetValue()).c_str());
	rpx_widget = lineEdit;
	return lineEdit;
#else
	return nullptr;
#endif
}


void defaultSetWidgetValue(const string& x_value, QWidget*& rpx_widget)
{
#ifndef MARKUS_NO_GUI
	dynamic_cast<QLineEdit*>(rpx_widget)->setText(x_value.c_str());
#else
	assert(false);
	return;
#endif
}

mkjson defaultGetValueFromWidget(QWidget*& rpx_widget)
{
#ifndef MARKUS_NO_GUI
	return mkjson(dynamic_cast<QLineEdit*>(rpx_widget)->text().toStdString());
#else
	assert(false);
	return "";
#endif
}
/*------------------------------------------------------------------------------------------------*/

template<> QWidget* ControllerParameterT<Stream>::CreateWidget()
{
	return nullptr;
}


template<> void ControllerParameterT<Stream>::SetWidgetValue(const string& x_value)
{
	return;
}

template<> mkjson ControllerParameterT<Stream>::GetValueFromWidget()
{
	return "";
}

/*------------------------------------------------------------------------------------------------*/

template<> QWidget* ControllerEnum::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	QComboBox* comboBox = new QComboBox;
	for(const auto & elem : m_param.GetEnum())
	{
		comboBox->addItem(elem.first.c_str(), elem.second);
	}
	int index = comboBox->findData(m_param.GetValue().get<int>());
	comboBox->setCurrentIndex(index);
	mp_widget = comboBox;

	return comboBox;
#else
	return nullptr;
#endif
}

template<> void ControllerEnum::SetWidgetValue(const string& x_value)
{
#ifndef MARKUS_NO_GUI
	QComboBox* comboBox = dynamic_cast<QComboBox*>(mp_widget);
	int index = comboBox->findData(m_param.GetValue().get<int>());
	comboBox->setCurrentIndex(index);
#endif
}

template<> mkjson ControllerEnum::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	QComboBox* comboBox = dynamic_cast<QComboBox*>(mp_widget);
	return m_param.GetReverseEnum().at(comboBox->itemData(comboBox->currentIndex()).toInt());
#else
	assert(false);
	return "";
#endif
}

}
