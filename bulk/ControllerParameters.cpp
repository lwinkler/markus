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

#ifndef MARKUS_NO_GUI
#include "QParameterSlider.h"
#endif


#define PRECISION_DOUBLE 2

/// Display the type of the parameter
void getType(Controller* xp_ctr, string* xp_value)
{
	ControllerParameter* ctr = dynamic_cast<ControllerParameter*>(xp_ctr);
	assert(ctr != NULL);
	if(xp_value != NULL)
	{
		*xp_value = ctr->param.GetTypeString();
		return;
	}
#ifdef MARKUS_NO_GUI
	assert(false);
#else
	// TODO ctr->SetWidgetValue(ctr->param.GetDefaultString());
#endif
}

/// Display the range string of the parameter
void getRange(Controller* xp_ctr, string* xp_value)
{
	ControllerParameter* ctr = dynamic_cast<ControllerParameter*>(xp_ctr);
	assert(ctr != NULL);
	if(xp_value != NULL)
	{
		*xp_value = ctr->param.GetRange();
		return;
	}
#ifdef MARKUS_NO_GUI
	assert(false);
#else
	// TODO ctr->SetWidgetValue(ctr->param.GetDefaultString());
#endif
}

/// Set the controlled value (e.g. parameter) to the value on control
void setControlledValue(Controller* xp_ctr, string* xp_value)
{
	ControllerParameter* ctr = dynamic_cast<ControllerParameter*>(xp_ctr);
	assert(ctr != NULL);
	if(xp_value != NULL)
	{
		ctr->param.SetValue(*xp_value, PARAMCONF_CMD);
	}
	else
	{
#ifdef MARKUS_NO_GUI
		assert(false);
#else
		ctr->param.SetValue(ctr->GetValueFromWidget(), PARAMCONF_GUI);
#endif
	}
	assert(ctr->param.CheckRange()); // TODO throw
}

/// Display the current value of the controlled object
void getCurrent(Controller* xp_ctr, string* xp_value)
{
	ControllerParameter* ctr = dynamic_cast<ControllerParameter*>(xp_ctr);
	assert(ctr != NULL);
	if(xp_value != NULL)
	{
		stringstream ss;
		ss<<ctr->param.GetValueString();
		*xp_value = ss.str();
		return;
	}
#ifdef MARKUS_NO_GUI
	assert(false);
#else
	ctr->SetWidgetValue(ctr->param.GetValueString());
#endif
}

/// Display the default value of the controlled object
void getDefault(Controller* xp_ctr, string* xp_value)
{
	ControllerParameter* ctr = dynamic_cast<ControllerParameter*>(xp_ctr);
	assert(ctr != NULL);
	if(xp_value != NULL)
	{
		*xp_value = ctr->param.GetDefaultString();
		return;
	}
#ifdef MARKUS_NO_GUI
	assert(false);
#else
	ctr->SetWidgetValue(ctr->param.GetDefaultString());
#endif
}


ControllerParameter::ControllerParameter(Parameter& x_param):
	Controller(x_param.GetName(), "parameter"),
	param(x_param)
{
	m_actions.insert(std::make_pair("GetType", &getType));
	m_actions.insert(std::make_pair("GetRange", &getRange));
	m_actions.insert(std::make_pair("Set", &setControlledValue));
	m_actions.insert(std::make_pair("Get", &getCurrent));
	m_actions.insert(std::make_pair("GetDefault", &getDefault));
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
	return parameterSlider = new QParameterSlider(m_param2.GetValue(), m_param2.GetMin(), m_param2.GetMax(), 0);
#else
	return NULL;
#endif
}


void ControllerInt::SetWidgetValue(const std::string& x_value)
{
#ifndef MARKUS_NO_GUI
	parameterSlider->SetValue(atoi(x_value.c_str()));
#endif
}

std::string ControllerInt::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	stringstream ss;
	ss<<parameterSlider->GetValue();
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
	return parameterSlider = new QParameterSlider(m_param2.GetValue(), m_param2.GetMin(), m_param2.GetMax(), PRECISION_DOUBLE);
#else
	return NULL;
#endif
}
void ControllerDouble::SetWidgetValue(const std::string& x_value)
{
#ifndef MARKUS_NO_GUI
	parameterSlider->SetValue(atof(x_value.c_str()));
#endif
}

std::string ControllerDouble::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	stringstream ss;
	ss<<parameterSlider->GetValue();
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
	return parameterSlider = new QParameterSlider(m_param2.GetValue(), m_param2.GetMin(), m_param2.GetMax(), PRECISION_DOUBLE);
#else
	return NULL;
#endif
}

void ControllerFloat::SetWidgetValue(const std::string& x_value)
{
#ifndef MARKUS_NO_GUI
	parameterSlider->SetValue(atof(x_value.c_str()));
#endif
}

std::string ControllerFloat::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	stringstream ss;
	ss<<parameterSlider->GetValue();
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
	checkBox = new QCheckBox("Enabled");
	checkBox->setChecked(m_param2.GetValue());
	return checkBox;
#else
	return NULL;
#endif
}

void ControllerBool::SetWidgetValue(const std::string& x_value)
{
#ifndef MARKUS_NO_GUI
	checkBox->setChecked(x_value == "1");
#endif
}

std::string ControllerBool::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	stringstream ss;
	checkBox->setChecked(m_param2.GetValue());
	return ss.str();
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
	lineEdit = new QLineEdit();
	lineEdit->setStyleSheet("color: black; background-color: white");
	lineEdit->setText(m_param2.GetValue().c_str());
	return lineEdit;
#else
	return NULL;
#endif
}
void ControllerString::SetWidgetValue(const std::string& x_value)
{
#ifndef MARKUS_NO_GUI
	lineEdit->setText(x_value.c_str());
#endif
}

std::string ControllerString::GetValueFromWidget()
{
#ifndef MARKUS_NO_GUI
	return lineEdit->text().toStdString();
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
	for(std::map<std::string, int>::const_iterator it = m_param2.RefEnum().begin() ; it != m_param2.RefEnum().end() ; it++)
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
	return m_param2.Int2Str(comboBox->itemData(comboBox->currentIndex()).toInt());
#else
	assert(false);
	return "";
#endif
}

/*------------------------------------------------------------------------------------------------*/
