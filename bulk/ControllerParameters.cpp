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

#include "ControllerParameters.h"
#include "CalibrationByHeight.h"

using namespace std;

#include <QWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QComboBox>

#ifndef MARKUS_NO_GUI
#include "QParameterSlider.h"
#endif


#define PRECISION_DOUBLE 2

log4cxx::LoggerPtr ControllerParameter::m_logger(log4cxx::Logger::getLogger("ControllerParameter"));

/**
* @brief Command: Display the type of the parameter
*
* @param xp_value Output type
*/
void ControllerParameter::GetParameterType(string* xp_value)
{
	if(xp_value != nullptr)
	{
		*xp_value = m_param.GetTypeString();
		return;
	}
#ifdef MARKUS_NO_GUI
	assert(false);
#else
	LOG_INFO(m_logger, "Parameter type is \"" + m_param.GetTypeString() + "\"");
#endif
}

/**
* @brief Command: Display the range string of the parameter
*
* @param xp_value Output range
*/
void ControllerParameter::GetRange(string* xp_value)
{
	if(xp_value != nullptr)
	{
		*xp_value = m_param.GetRange();
		return;
	}
#ifdef MARKUS_NO_GUI
	assert(false);
#else
	LOG_INFO(m_logger, "Parameter range is \"" + m_param.GetRange() + "\"");
#endif
}

/**
* @brief Command: Set the controlled value (e.g. parameter) to the value on control
*
* @param xp_value Output value if the pointer is not null
*/
void ControllerParameter::SetControlledValue(string* xp_value)
{
	string oldValue = m_param.GetValueString();
	ParameterConfigType configType = m_param.GetConfigurationSource();

	if(xp_value != nullptr)
	{
		m_param.SetValue(*xp_value, PARAMCONF_CMD);
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
		catch(...)
		{
			LOG_ERROR(m_logger, "Error setting value of parameter from widget");
		}
#endif
	}
	if(!m_param.CheckRange())
	{
		m_param.SetValue(oldValue, configType);
		throw MkException("Parameter " + m_param.GetName() + "= " + *xp_value + " is out of range " + m_param.GetRange(), LOC);
	}
}

/**
* @brief Command: Display the current value of the controlled object
*
* @param xp_value Output value if the pointer is not null
*/
void ControllerParameter::GetCurrent(string* xp_value)
{
	if(xp_value != nullptr)
	{
		stringstream ss;
		ss<<m_param.GetValueString();
		*xp_value = ss.str();
		return;
	}
#ifdef MARKUS_NO_GUI
	assert(false);
#else
	SetWidgetValue(m_param.GetValueString());
#endif
}

/**
* @brief Command: Display the default value of the controlled object
*
* @param xp_value Output value if the pointer is not null
*/
void ControllerParameter::GetDefault(string* xp_value)
{
	if(xp_value != nullptr)
	{
		*xp_value = m_param.GetDefaultString();
		return;
	}
#ifdef MARKUS_NO_GUI
	assert(false);
#else
	SetWidgetValue(m_param.GetDefaultString());
#endif
}


ControllerParameter::ControllerParameter(Parameter& x_param):
	Controller(x_param.GetName()),
	m_param(x_param)
{
	m_actions.insert(make_pair("GetType",    &ControllerParameter::GetParameterType));
	m_actions.insert(make_pair("GetRange",   &ControllerParameter::GetRange));
	m_actions.insert(make_pair("Set",        &ControllerParameter::SetControlledValue));
	m_actions.insert(make_pair("Get",        &ControllerParameter::GetCurrent));
	m_actions.insert(make_pair("GetDefault", &ControllerParameter::GetDefault));
}


/*------------------------------------------------------------------------------------------------*/

ControllerInt::ControllerInt(Parameter& x_param):
	ControllerParameter(x_param),
	m_param2(dynamic_cast<ParameterInt&>(x_param))
{
	mp_parameterSlider = nullptr;
}

QWidget* ControllerInt::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	return mp_parameterSlider = new QParameterSlider(m_param2.GetValue(), m_param2.GetMin(), m_param2.GetMax(), 0);
#else
	return NULL;
#endif
}


void ControllerInt::SetWidgetValue(const string& x_value)
{
#ifndef MARKUS_NO_GUI
	mp_parameterSlider->SetValue(atoi(x_value.c_str()));
#endif
}

string ControllerInt::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	stringstream ss;
	ss<<mp_parameterSlider->GetValue();
	return ss.str();
#else
	assert(false);
	return "";
#endif
}

/*------------------------------------------------------------------------------------------------*/

ControllerUInt::ControllerUInt(Parameter& x_param):
	ControllerParameter(x_param),
	m_param2(dynamic_cast<ParameterUInt&>(x_param))
{
	mp_parameterSlider = nullptr;
}

QWidget* ControllerUInt::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	return mp_parameterSlider = new QParameterSlider(m_param2.GetValue(), m_param2.GetMin(), m_param2.GetMax(), 0);
#else
	return NULL;
#endif
}


void ControllerUInt::SetWidgetValue(const string& x_value)
{
#ifndef MARKUS_NO_GUI
	mp_parameterSlider->SetValue(atoi(x_value.c_str()));
#endif
}

string ControllerUInt::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	stringstream ss;
	ss<<mp_parameterSlider->GetValue();
	return ss.str();
#else
	assert(false);
	return "";
#endif
}
/*------------------------------------------------------------------------------------------------*/
ControllerDouble::ControllerDouble(Parameter& x_param):
	ControllerParameter(x_param),
	m_param2(dynamic_cast<ParameterDouble&>(x_param))
{
	mp_parameterSlider = nullptr;
}
QWidget* ControllerDouble::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	return mp_parameterSlider = new QParameterSlider(m_param2.GetValue(), m_param2.GetMin(), m_param2.GetMax(), PRECISION_DOUBLE);
#else
	return NULL;
#endif
}
void ControllerDouble::SetWidgetValue(const string& x_value)
{
#ifndef MARKUS_NO_GUI
	mp_parameterSlider->SetValue(atof(x_value.c_str()));
#endif
}

string ControllerDouble::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	stringstream ss;
	ss<<mp_parameterSlider->GetValue();
	return ss.str();
#else
	assert(false);
	return "";
#endif
}
/*------------------------------------------------------------------------------------------------*/

ControllerFloat::ControllerFloat(Parameter& x_param):
	ControllerParameter(x_param),
	m_param2(dynamic_cast<ParameterFloat&>(x_param))
{
	mp_parameterSlider = nullptr;
}


QWidget* ControllerFloat::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	return mp_parameterSlider = new QParameterSlider(m_param2.GetValue(), m_param2.GetMin(), m_param2.GetMax(), PRECISION_DOUBLE);
#else
	return NULL;
#endif
}

void ControllerFloat::SetWidgetValue(const string& x_value)
{
#ifndef MARKUS_NO_GUI
	mp_parameterSlider->SetValue(atof(x_value.c_str()));
#endif
}

string ControllerFloat::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	stringstream ss;
	ss<<mp_parameterSlider->GetValue();
	return ss.str();
#else
	assert(false);
	return "";
#endif
}
/*------------------------------------------------------------------------------------------------*/
ControllerBool::ControllerBool(Parameter& x_param):
	ControllerParameter(x_param),
	m_param2(dynamic_cast<ParameterBool&>(x_param))
{
	mp_checkBox = nullptr;
}

QWidget* ControllerBool::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	mp_checkBox = new QCheckBox("Enabled");
	mp_checkBox->setChecked(m_param2.GetValue());
	return mp_checkBox;
#else
	return NULL;
#endif
}

void ControllerBool::SetWidgetValue(const string& x_value)
{
#ifndef MARKUS_NO_GUI
	mp_checkBox->setChecked(x_value == "1");
#endif
}

string ControllerBool::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	return mp_checkBox->isChecked() ? "1" : "0";
#else
	assert(false);
	return "";
#endif
}
/*------------------------------------------------------------------------------------------------*/
ControllerString::ControllerString(Parameter& x_param):
	ControllerParameter(x_param),
	m_param2(dynamic_cast<ParameterString&>(x_param))
{
	mp_lineEdit = nullptr;
}

QWidget* ControllerString::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	mp_lineEdit = new QLineEdit();
	mp_lineEdit->setStyleSheet("color: black; background-color: white");
	mp_lineEdit->setText(m_param2.GetValue().c_str());
	return mp_lineEdit;
#else
	return NULL;
#endif
}
void ControllerString::SetWidgetValue(const string& x_value)
{
#ifndef MARKUS_NO_GUI
	mp_lineEdit->setText(x_value.c_str());
#endif
}

string ControllerString::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	return mp_lineEdit->text().toStdString();
#else
	assert(false);
	return "";
#endif
}

/*------------------------------------------------------------------------------------------------*/
ControllerSerializable::ControllerSerializable(Parameter& x_param):
	ControllerParameter(x_param),
	m_param2(dynamic_cast<ParameterSerializable&>(x_param))
{
	mp_lineEdit = nullptr;
}

QWidget* ControllerSerializable::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	mp_lineEdit = new QLineEdit();
	mp_lineEdit->setStyleSheet("color: black; background-color: white");
	mp_lineEdit->setText(m_param2.GetValueString().c_str());
	return mp_lineEdit;
#else
	return NULL;
#endif
}


void ControllerSerializable::SetWidgetValue(const string& x_value)
{
#ifndef MARKUS_NO_GUI
	mp_lineEdit->setText(x_value.c_str());
#else
	assert(false);
	return;
#endif
}

string ControllerSerializable::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	return mp_lineEdit->text().toStdString();
#else
	assert(false);
	return "";
#endif
}
/*------------------------------------------------------------------------------------------------*/
ControllerCalibrationByHeight::ControllerCalibrationByHeight(Parameter& x_param):
	ControllerParameter(x_param),
	m_param2(dynamic_cast<ParameterSerializable&>(x_param))
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
	ss << m_param2.GetValue();
	calib.Deserialize(ss, "");

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
	return NULL;
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

string ControllerCalibrationByHeight::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	stringstream ss;
	CalibrationByHeight object;
	object.x = mp_sliderX->GetValue();
	object.y = mp_sliderY->GetValue();
	object.height = mp_sliderHeight->GetValue();
	object.Serialize(ss);

	return ss.str();
#else
	assert(false);
	return "";
#endif
}

/*------------------------------------------------------------------------------------------------*/
ControllerEnum::ControllerEnum(Parameter& x_param):
	ControllerParameter(x_param),
	m_param2(dynamic_cast<ParameterEnum&>(x_param))
{
	mp_comboBox = nullptr;
}


QWidget* ControllerEnum::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	mp_comboBox = new QComboBox;
	for(const auto & elem : m_param2.GetEnum())
	{
		mp_comboBox->addItem(elem.first.c_str(), elem.second);
	}
	int index = mp_comboBox->findData(m_param2.GetValue());
	mp_comboBox->setCurrentIndex(index);

	return mp_comboBox;
#else
	return NULL;
#endif
}

void ControllerEnum::SetWidgetValue(const string& x_value)
{
#ifndef MARKUS_NO_GUI
	int index = mp_comboBox->findData(m_param2.GetValue());
	mp_comboBox->setCurrentIndex(index);
#endif
}

string ControllerEnum::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	return m_param2.GetReverseEnum().at(mp_comboBox->itemData(mp_comboBox->currentIndex()).toInt());
#else
	assert(false);
	return "";
#endif
}

/*------------------------------------------------------------------------------------------------*/
ControllerText::ControllerText(Parameter& x_param):
	ControllerParameter(x_param),
	m_param2(x_param)
{
	mp_textEdit = nullptr;
}

QWidget* ControllerText::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	mp_textEdit = new QTextEdit();
	mp_textEdit->setStyleSheet("color: black; background-color: white");
	mp_textEdit->setText(m_param2.GetValueString().c_str());
	return mp_textEdit;
#else
	return NULL;
#endif
}
void ControllerText::SetWidgetValue(const string& x_value)
{
#ifndef MARKUS_NO_GUI
	mp_textEdit->setText(x_value.c_str());
#endif
}

string ControllerText::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	return mp_textEdit->toPlainText().toStdString();
#else
	assert(false);
	return "";
#endif
}
