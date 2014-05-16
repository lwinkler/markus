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
#include <QCheckBox>
#include <QComboBox>
#include <iostream>

#ifndef MARKUS_NO_GUI
#include "QParameterSlider.h"
#endif


#define PRECISION_DOUBLE 2

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
	// TODO ctr->SetWidgetValue(ctr->param.GetDefaultString());
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
	// TODO ctr->SetWidgetValue(ctr->param.GetDefaultString());
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
ControllerObjectHeight::ControllerObjectHeight(ParameterObjectHeigth& x_param):
	ControllerParameter(x_param),
	m_param2(x_param)
{
}

QWidget* ControllerObjectHeight::CreateWidget()
{
#ifndef MARKUS_NO_GUI

	QBoxLayout * mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	widget = new QWidget();

	slider_x =new QParameterSlider(m_param2.GetValue().x, 0, 1, PRECISION_DOUBLE,widget);
	slider_y = new QParameterSlider(m_param2.GetValue().y, 0, 1, PRECISION_DOUBLE,widget);
	slider_heigth = new QParameterSlider(m_param2.GetValue().heigth, 0, 1, PRECISION_DOUBLE,widget);

	mainLayout->addWidget(slider_x,0);
	mainLayout->addWidget(slider_y,1);
	mainLayout->addWidget(slider_heigth,2);
	widget->setLayout(mainLayout);

	return widget;
#else
	return NULL;
#endif
}


void ControllerObjectHeight::SetWidgetValue(const std::string& x_value)
{
#ifndef MARKUS_NO_GUI
	CalibrationByHeigth object;
	istringstream str(x_value);
	object.Deserialize(str);
	slider_x->SetValue(object.x);
	slider_y->SetValue(object.y);
	slider_heigth->SetValue(object.heigth);
#endif
}

std::string ControllerObjectHeight::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	stringstream ss;
	CalibrationByHeigth object;
	object.x = slider_x->GetValue();
	object.y = slider_y->GetValue();
	object.heigth = slider_heigth->GetValue();
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
