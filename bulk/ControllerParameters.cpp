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
	if(xp_value != NULL)
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
	if(xp_value != NULL)
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

	if(xp_value != NULL)
	{
		m_param.SetValue(*xp_value, PARAMCONF_CMD);
	}
	else
	{
#ifdef MARKUS_NO_GUI
		assert(false);
#else
		m_param.SetValue(GetValueFromWidget(), PARAMCONF_GUI);
#endif
	}
	if(!m_param.CheckRange())
	{
		m_param.SetValue(oldValue, configType);
		throw MkException("Parameter " + m_param.GetName() + " is out of range", LOC);
	}
}

/**
* @brief Command: Display the current value of the controlled object
*
* @param xp_value Output value if the pointer is not null
*/
void ControllerParameter::GetCurrent(string* xp_value)
{
	if(xp_value != NULL)
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
	if(xp_value != NULL)
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
	m_actions.insert(std::make_pair("GetType",    &ControllerParameter::GetParameterType));
	m_actions.insert(std::make_pair("GetRange",   &ControllerParameter::GetRange));
	m_actions.insert(std::make_pair("Set",        &ControllerParameter::SetControlledValue));
	m_actions.insert(std::make_pair("Get",        &ControllerParameter::GetCurrent));
	m_actions.insert(std::make_pair("GetDefault", &ControllerParameter::GetDefault));
}


/*------------------------------------------------------------------------------------------------*/

ControllerInt::ControllerInt(ParameterInt& x_param):
	ControllerParameter(x_param),
	m_param2(x_param)
{
}

QWidget* ControllerInt::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	return m_parameterSlider = new QParameterSlider(m_param2.GetValue(), m_param2.GetMin(), m_param2.GetMax(), 0);
#else
	return NULL;
#endif
}


void ControllerInt::SetWidgetValue(const std::string& x_value)
{
#ifndef MARKUS_NO_GUI
	m_parameterSlider->SetValue(atoi(x_value.c_str()));
#endif
}

std::string ControllerInt::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	stringstream ss;
	ss<<m_parameterSlider->GetValue();
	return ss.str();
#else
	assert(false);
	return "";
#endif
}
/*------------------------------------------------------------------------------------------------*/
ControllerDouble::ControllerDouble(ParameterDouble& x_param):
	ControllerParameter(x_param),
	m_param2(x_param)
{
}
QWidget* ControllerDouble::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	return m_parameterSlider = new QParameterSlider(m_param2.GetValue(), m_param2.GetMin(), m_param2.GetMax(), PRECISION_DOUBLE);
#else
	return NULL;
#endif
}
void ControllerDouble::SetWidgetValue(const std::string& x_value)
{
#ifndef MARKUS_NO_GUI
	m_parameterSlider->SetValue(atof(x_value.c_str()));
#endif
}

std::string ControllerDouble::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	stringstream ss;
	ss<<m_parameterSlider->GetValue();
	return ss.str();
#else
	assert(false);
	return "";
#endif
}
/*------------------------------------------------------------------------------------------------*/

ControllerFloat::ControllerFloat(ParameterFloat& x_param):
	ControllerParameter(x_param),
	m_param2(x_param)
{
}


QWidget* ControllerFloat::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	return m_parameterSlider = new QParameterSlider(m_param2.GetValue(), m_param2.GetMin(), m_param2.GetMax(), PRECISION_DOUBLE);
#else
	return NULL;
#endif
}

void ControllerFloat::SetWidgetValue(const std::string& x_value)
{
#ifndef MARKUS_NO_GUI
	m_parameterSlider->SetValue(atof(x_value.c_str()));
#endif
}

std::string ControllerFloat::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	stringstream ss;
	ss<<m_parameterSlider->GetValue();
	return ss.str();
#else
	assert(false);
	return "";
#endif
}
/*------------------------------------------------------------------------------------------------*/
ControllerBool::ControllerBool(ParameterBool& x_param):
	ControllerParameter(x_param),
	m_param2(x_param)
{
}

QWidget* ControllerBool::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	m_checkBox = new QCheckBox("Enabled");
	m_checkBox->setChecked(m_param2.GetValue());
	return m_checkBox;
#else
	return NULL;
#endif
}

void ControllerBool::SetWidgetValue(const std::string& x_value)
{
#ifndef MARKUS_NO_GUI
	m_checkBox->setChecked(x_value == "1");
#endif
}

std::string ControllerBool::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	return m_checkBox->isChecked() ? "1" : "0";
#else
	assert(false);
	return "";
#endif
}
/*------------------------------------------------------------------------------------------------*/
ControllerString::ControllerString(ParameterString& x_param):
	ControllerParameter(x_param),
	m_param2(x_param)
{
}

QWidget* ControllerString::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	m_lineEdit = new QLineEdit();
	m_lineEdit->setStyleSheet("color: black; background-color: white");
	m_lineEdit->setText(m_param2.GetValue().c_str());
	return m_lineEdit;
#else
	return NULL;
#endif
}
void ControllerString::SetWidgetValue(const std::string& x_value)
{
#ifndef MARKUS_NO_GUI
	m_lineEdit->setText(x_value.c_str());
#endif
}

std::string ControllerString::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	return m_lineEdit->text().toStdString();
#else
	assert(false);
	return "";
#endif
}
/*------------------------------------------------------------------------------------------------*/
ControllerCalibrationByHeight::ControllerCalibrationByHeight(ParameterCalibrationByHeight& x_param):
	ControllerParameter(x_param),
	m_param2(x_param)
{
}

QWidget* ControllerCalibrationByHeight::CreateWidget()
{
#ifndef MARKUS_NO_GUI

	QGridLayout * mainLayout = new QGridLayout();
	widget = new QWidget();

	mp_sliderX = new QParameterSlider(m_param2.GetValue().x, 0, 1, PRECISION_DOUBLE,widget);
	mp_sliderY = new QParameterSlider(m_param2.GetValue().y, 0, 1, PRECISION_DOUBLE,widget);
	mp_sliderHeight = new QParameterSlider(m_param2.GetValue().height, 0, 1, PRECISION_DOUBLE,widget);
	mp_labX = new QLabel("x");
	mp_labY = new QLabel("y");
	mp_labHeight = new QLabel("height");

	mainLayout->addWidget(mp_labX, 0, 0);
	mainLayout->addWidget(mp_sliderX,0,1);
	mainLayout->addWidget(mp_labY, 1, 0);
	mainLayout->addWidget(mp_sliderY,1,1);
	mainLayout->addWidget(mp_labHeight, 2, 0);
	mainLayout->addWidget(mp_sliderHeight,2,1);
	widget->setLayout(mainLayout);

	return widget;
#else
	return NULL;
#endif
}


void ControllerCalibrationByHeight::SetWidgetValue(const std::string& x_value)
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

std::string ControllerCalibrationByHeight::GetValueFromWidget()
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
ControllerEnum::ControllerEnum(ParameterEnum& x_param):
	ControllerParameter(x_param),
	m_param2(x_param)
{
}


QWidget* ControllerEnum::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	comboBox = new QComboBox;
	for(std::map<std::string, int>::const_iterator it = m_param2.GetEnum().begin() ; it != m_param2.GetEnum().end() ; it++)
	{
		comboBox->addItem(it->first.c_str(), it->second);
	}
	int index = comboBox->findData(m_param2.GetValue());
	comboBox->setCurrentIndex(index);

	return comboBox;
#else
	return NULL;
#endif
}

void ControllerEnum::SetWidgetValue(const std::string& x_value)
{
#ifndef MARKUS_NO_GUI
	int index = comboBox->findData(m_param2.GetValue());
	comboBox->setCurrentIndex(index);
#endif
}

std::string ControllerEnum::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	return m_param2.GetReverseEnum().at(comboBox->itemData(comboBox->currentIndex()).toInt());
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
}

QWidget* ControllerText::CreateWidget()
{
#ifndef MARKUS_NO_GUI
	m_textEdit = new QTextEdit();
	m_textEdit->setStyleSheet("color: black; background-color: white");
	m_textEdit->setText(m_param2.GetValueString().c_str());
	return m_textEdit;
#else
	return NULL;
#endif
}
void ControllerText::SetWidgetValue(const std::string& x_value)
{
#ifndef MARKUS_NO_GUI
	m_textEdit->setText(x_value.c_str());
#endif
}

std::string ControllerText::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	return m_textEdit->toPlainText().toStdString();
#else
	assert(false);
	return "";
#endif
}
