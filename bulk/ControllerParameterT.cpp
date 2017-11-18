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
#include "Processable.h"
#include <boost/lexical_cast.hpp>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

using namespace std;

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

typedef ControllerParameterT<ParameterInt> ControllerInt;
typedef ControllerParameterT<ParameterUInt> ControllerUInt;
typedef ControllerParameterT<ParameterDouble> ControllerDouble;
typedef ControllerParameterT<ParameterFloat> ControllerFloat;
typedef ControllerParameterT<ParameterBool> ControllerBool;
typedef ControllerParameterT<ParameterString> ControllerString;
typedef ControllerParameterT<ParameterSerializable> ControllerSerializable;
typedef ControllerParameterT<ParameterEnum> ControllerEnum;

// Explicit template instanciation
template class ControllerParameterT<ParameterInt>;
template class ControllerParameterT<ParameterUInt>;
template class ControllerParameterT<ParameterDouble>;
template class ControllerParameterT<ParameterFloat>;
template class ControllerParameterT<ParameterBool>;
template class ControllerParameterT<ParameterString>;
template class ControllerParameterT<ParameterSerializable>;
template class ControllerParameterT<ParameterEnum>;

#define PRECISION_DOUBLE 2

/**
* @brief Command: Display the type of the parameter
*
* @param xp_value Output type
*/
template<class T> void ControllerParameterT<T>::GetType(string* xp_value)
{
	Processable::ReadLock lock(m_module.RefLock());
	if(xp_value != nullptr)
	{
		*xp_value = m_param.GetType();
		return;
	}
#ifdef MARKUS_NO_GUI
	assert(false);
#else
	LOG_INFO(m_logger, "Parameter type is \"" + m_param.GetType() + "\"");
#endif
}

/**
* @brief Command: Display the range string of the parameter
*
* @param xp_value Output range
*/
template<class T> void ControllerParameterT<T>::GetRange(string* xp_value)
{
	Processable::ReadLock lock(m_module.RefLock());
	if(xp_value != nullptr)
	{
		*xp_value = jsonToString(m_param.GetRange());
		return;
	}
#ifdef MARKUS_NO_GUI
	assert(false);
#else
	LOG_INFO(m_logger, "Parameter range is \"" + jsonToString(m_param.GetRange()) + "\"");
#endif
}

/**
* @brief Command: Set the controlled value (e.g. parameter) to the value on control
*
* @param xp_value Output value if the pointer is not null
*/
template<class T> void ControllerParameterT<T>::SetControlledValue(string* xp_value)
{
	Processable::WriteLock lock(m_module.RefLock());
	const auto oldValue = m_param.GetValue();
	ParameterConfigType configType = m_param.GetConfigurationSource();

	if(xp_value != nullptr)
	{
		m_param.SetValue(stringToJson(*xp_value), PARAMCONF_CMD);
	}
	else
	{
#ifdef MARKUS_NO_GUI
		assert(false);
#else
		try
		{
			m_param.SetValue(GetValueFromWidget(), PARAMCONF_GUI);
		}
		catch(exception& e)
		{
			LOG_ERROR(m_logger, "Error setting value of parameter from widget: " + string(e.what()));
		}
#endif
	}
	if(!m_param.CheckRange())
	{
		m_param.SetValue(oldValue, configType);
		throw MkException("Parameter " + m_param.GetName() + "=" + *xp_value + " is out of range " + jsonToString(m_param.GetRange()), LOC);
	}
}

/**
* @brief Command: Display the current value of the controlled object
*
* @param xp_value Output value if the pointer is not null
*/
template<class T> void ControllerParameterT<T>::GetCurrent(string* xp_value)
{
	Processable::ReadLock lock(m_module.RefLock());
	stringstream ss;
	ss << m_param.GetValue();
	if(xp_value != nullptr)
	{
		*xp_value = ss.str();
		return;
	}
#ifdef MARKUS_NO_GUI
	assert(false);
#else
	SetWidgetValue(ss.str());
#endif
}

/**
* @brief Command: Display the default value of the controlled object
*
* @param xp_value Output value if the pointer is not null
*/
template<class T> void ControllerParameterT<T>::GetDefault(string* xp_value)
{
	Processable::ReadLock lock(m_module.RefLock());
	stringstream ss;
	ss<<m_param.GetDefault();
	if(xp_value != nullptr)
	{
		*xp_value = ss.str();
		return;
	}
#ifdef MARKUS_NO_GUI
	assert(false);
#else
	SetWidgetValue(ss.str());
#endif
}



/*------------------------------------------------------------------------------------------------*/

template<> QWidget* ControllerInt::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	return mp_widget = new QParameterSlider(m_param.GetValue().asInt(), m_param.GetRange()["min"].asDouble(), m_param.GetRange()["max"].asDouble(), 0);
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

template<> Json::Value ControllerInt::GetValueFromWidget()
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
	return mp_widget = new QParameterSlider(m_param.GetValue().asUInt(), m_param.GetRange()["min"].asDouble(), m_param.GetRange()["max"].asDouble(), 0);
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

template<> Json::Value ControllerUInt::GetValueFromWidget()
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
	return mp_widget = new QParameterSlider(m_param.GetValue().asDouble(), m_param.GetRange()["min"].asDouble(), m_param.GetRange()["max"].asDouble(), PRECISION_DOUBLE);
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

template<> Json::Value ControllerDouble::GetValueFromWidget()
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
	return mp_widget = new QParameterSlider(m_param.GetValue().asFloat(), m_param.GetRange()["min"].asDouble(), m_param.GetRange()["max"].asDouble(), PRECISION_DOUBLE);
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

template<> Json::Value ControllerFloat::GetValueFromWidget()
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
	dynamic_cast<QCheckBox*>(mp_widget)->setChecked(m_param.GetValue().asInt());
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

template<> Json::Value ControllerBool::GetValueFromWidget()
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
	lineEdit->setText(m_param.GetValue().asString().c_str());
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

template<> Json::Value ControllerString::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	return dynamic_cast<QLineEdit*>(mp_widget)->text().toStdString();
#else
	assert(false);
	return "";
#endif
}

/*------------------------------------------------------------------------------------------------*/

template<> QWidget* ControllerSerializable::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	QLineEdit* lineEdit = new QLineEdit();
	lineEdit->setStyleSheet("color: black; background-color: white");
	lineEdit->setText(jsonToString(m_param.GetValue()).c_str());
	mp_widget = lineEdit;
	return lineEdit;
#else
	return nullptr;
#endif
}


template<> void ControllerSerializable::SetWidgetValue(const string& x_value)
{
#ifndef MARKUS_NO_GUI
	dynamic_cast<QLineEdit*>(mp_widget)->setText(x_value.c_str());
#else
	assert(false);
	return;
#endif
}

template<> Json::Value ControllerSerializable::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	return stringToJson(dynamic_cast<QLineEdit*>(mp_widget)->text().toStdString());
#else
	assert(false);
	return "";
#endif
}
/*------------------------------------------------------------------------------------------------*/

QWidget* ControllerVoid::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	// QLineEdit* lineEdit = new QLineEdit();
	// lineEdit->setStyleSheet("color: black; background-color: white");
	// // lineEdit->setText(jsonToString(m_param.GetValue()).c_str());
	// mp_widget = lineEdit;
	return nullptr;
#else
	return nullptr;
#endif
}


void ControllerVoid::SetWidgetValue(const string& x_value)
{
#ifndef MARKUS_NO_GUI
	// dynamic_cast<QLineEdit*>(mp_widget)->setText(x_value.c_str());
#else
	assert(false);
	return;
#endif
}

Json::Value ControllerVoid::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	return ""; // dynamic_cast<QLineEdit*>(mp_widget)->text().toStdString();
#else
	assert(false);
	return "";
#endif
}
/*------------------------------------------------------------------------------------------------*/
/*
ControllerCalibrationByHeight::ControllerCalibrationByHeight(Parameter& x_param, Processable& x_module):
	Controller(x_param.GetName()),
	m_param(dynamic_cast<ParameterSerializable&>(x_param)),
	// m_param(x_param),
	m_module(x_module)
{
	mp_widget = nullptr;
	mp_sliderX = nullptr;
	mp_sliderY = nullptr;
	mp_sliderHeight = nullptr;
	mp_labX = nullptr;
	mp_labY = nullptr;
	mp_labHeight = nullptr;
}

QWidget* ControllerCalibrationByHeight::CreateWidget()
{
#ifndef MARKUS_NO_GUI

	auto   mainLayout = new QGridLayout();
	mp_widget = new QWidget();
	CalibrationByHeight calib;
	stringstream ss;
	ss << m_param.GetValue();
	calib.Deserialize(ss);

	mp_sliderX = new QParameterSlider(calib.x, 0, 1, PRECISION_DOUBLE,mp_widget);
	mp_sliderY = new QParameterSlider(calib.y, 0, 1, PRECISION_DOUBLE,mp_widget);
	mp_sliderHeight = new QParameterSlider(calib.height, 0, 1, PRECISION_DOUBLE,mp_widget);
	mp_labX = new QLabel("x");
	mp_labY = new QLabel("y");
	mp_labHeight = new QLabel("height");

	mainLayout->addWidget(mp_labX, 0, 0);
	mainLayout->addWidget(mp_sliderX,0,1);
	mainLayout->addWidget(mp_labY, 1, 0);
	mainLayout->addWidget(mp_sliderY,1,1);
	mainLayout->addWidget(mp_labHeight, 2, 0);
	mainLayout->addWidget(mp_sliderHeight,2,1);
	mp_widget->setLayout(mainLayout);

	return mp_widget;
#else
	return nullptr;
#endif
}


void ControllerCalibrationByHeight::SetWidgetValue(const string& x_value)
{
#ifndef MARKUS_NO_GUI
	CalibrationByHeight object;
	istringstream str(x_value);
	object.Deserialize(str);
	mp_sliderX->SetValue(object.x);
	mp_sliderY->SetValue(object.y);
	mp_sliderHeight->SetValue(object.height);
#endif
}

Json::Value ControllerCalibrationByHeight::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	stringstream ss;
	CalibrationByHeight object;
	object.x = mp_sliderX->GetValue();
	object.y = mp_sliderY->GetValue();
	object.height = mp_sliderHeight->GetValue();
	object.Serialize(ss);

	return stringToJson(ss.str());
#else
	assert(false);
	return "";
#endif
}
*/

/*------------------------------------------------------------------------------------------------*/

template<> QWidget* ControllerEnum::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	QComboBox* comboBox = new QComboBox;
	for(const auto & elem : m_param.GetEnum())
	{
		comboBox->addItem(elem.first.c_str(), elem.second);
	}
	int index = comboBox->findData(m_param.GetValue().asInt());
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
	int index = comboBox->findData(m_param.GetValue().asInt());
	comboBox->setCurrentIndex(index);
#endif
}

template<> Json::Value ControllerEnum::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	QComboBox* comboBox = dynamic_cast<QComboBox*>(mp_widget);
	return m_param.GetReverseEnum().at(comboBox->itemData(comboBox->currentIndex()).toInt());
#else
	assert(false);
	return "";
#endif
}

