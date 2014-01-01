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

#include "Control.h"
#include <cstdio>

using namespace std;

#include <QWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>

#include "QParameterSlider.h"


#define PRECISION_DOUBLE 2

#define CLEAN_DELETE(x) if((x) != NULL){delete((x));(x) = NULL;}

Controller::Controller()
{
	m_widget = NULL;
}


Controller::~Controller()
{
	// if(m_widget != NULL) delete m_widget; // no need to delete
}

/// Set the controlled value (e.g. parameter) to the value on control
void setControlledValueInt(Controller* x_ctr)
{
	ControllerInt* ctr = dynamic_cast<ControllerInt*>(x_ctr);
	assert(ctr != NULL);
	ctr->m_param.SetValue(ctr->m_parameterSlider->GetValue(), PARAMCONF_GUI);
}

/// Display the current value of the controlled object
void getCurrentInt(Controller* x_ctr)
{
	ControllerInt* ctr = dynamic_cast<ControllerInt*>(x_ctr);
	assert(ctr != NULL);
	ctr->m_parameterSlider->SetValue(ctr->m_param.GetValue());
}

/// Display the default value of the controlled object
void getDefaultInt(Controller* x_ctr)
{
	ControllerInt* ctr = dynamic_cast<ControllerInt*>(x_ctr);
	assert(ctr != NULL);
	ctr->m_parameterSlider->SetValue(ctr->m_param.GetDefault());
}

ControllerInt::ControllerInt(ParameterInt& x_param):
	Controller(),
	m_param(x_param)
{
	m_widget = m_parameterSlider = new QParameterSlider(m_param.GetValue(), m_param.GetMin(), m_param.GetMax(), 0);
	// m_actions["Set"] = &ControllerInt::SetControlledValue;
	m_actions.insert(std::make_pair("Set", &setControlledValueInt));
	m_actions.insert(std::make_pair("GetCurrent", &getCurrentInt));
	m_actions.insert(std::make_pair("GetDefault", &getDefaultInt));
}

ControllerInt::~ControllerInt()
{
	// delete(m_parameterSlider);
}


/*------------------------------------------------------------------------------------------------*/
ControllerDouble::ControllerDouble(ParameterDouble& x_param):
	Controller(),
	m_param(x_param)
{
	m_widget = m_parameterSlider = new QParameterSlider(m_param.GetValue(), m_param.GetMin(), m_param.GetMax(), PRECISION_DOUBLE);
}

ControllerDouble::~ControllerDouble()
{
	// delete(m_parameterSlider);
}

void ControllerDouble::SetControlledValue()
{
	m_param.SetValue(m_parameterSlider->GetValue(), PARAMCONF_GUI);
}

void ControllerDouble::GetCurrent()
{
	m_parameterSlider->SetValue(m_param.GetValue());
}

void ControllerDouble::GetDefault()
{
	m_parameterSlider->SetValue(m_param.GetDefault());
}
/*------------------------------------------------------------------------------------------------*/

ControllerBool::ControllerBool(ParameterBool& x_param):
	Controller(),
	m_param(x_param)
{
	m_widget = m_checkBox = new QCheckBox("Enabled");
	GetCurrent();
}

ControllerBool::~ControllerBool()
{
	// delete(m_checkBox);
}

void ControllerBool::SetControlledValue()
{
	m_param.SetValue(m_checkBox->isChecked(), PARAMCONF_GUI);
}

void ControllerBool::GetCurrent()
{
	m_checkBox->setChecked(m_param.GetValue());
}

void ControllerBool::GetDefault()
{
	m_checkBox->setChecked(m_param.GetDefault());
}

/*------------------------------------------------------------------------------------------------*/
ControllerString::ControllerString(ParameterString& x_param):
	Controller(),
	m_param(x_param)
{
	m_widget = m_lineEdit = new QLineEdit();
	m_lineEdit->setStyleSheet("color: black; background-color: white");
	GetCurrent();
}

ControllerString::~ControllerString()
{
	// CLEAN_DELETE(m_lineEdit); // TODO: this causes a crash at closing if the parameter screen is activated.
}

void ControllerString::SetControlledValue()
{
	m_param.SetValue(m_lineEdit->text().toStdString(), PARAMCONF_GUI);
}

void ControllerString::GetCurrent()
{
	m_lineEdit->setText(m_param.GetValue().c_str());
}

void ControllerString::GetDefault()
{
	m_lineEdit->setText(m_param.GetDefault().c_str());
}
/*------------------------------------------------------------------------------------------------*/

ControllerFloat::ControllerFloat(ParameterFloat& x_param):
	Controller(),
	m_param(x_param)
{
	m_widget = m_parameterSlider = new QParameterSlider(m_param.GetValue(), m_param.GetMin(), m_param.GetMax(), PRECISION_DOUBLE);
}

ControllerFloat::~ControllerFloat()
{
	// delete(m_parameterSlider);
}

void ControllerFloat::SetControlledValue()
{
	m_param.SetValue(m_parameterSlider->GetValue(), PARAMCONF_GUI);
}

void ControllerFloat::GetCurrent()
{
	m_parameterSlider->SetValue(m_param.GetValue());
}

void ControllerFloat::GetDefault()
{
	m_parameterSlider->SetValue(m_param.GetDefault());
}


/*------------------------------------------------------------------------------------------------*/

ControllerEnum::ControllerEnum(ParameterEnum& x_param):
	Controller(),
	m_param(x_param)
{
	m_widget = m_comboBox = new QComboBox;
	for(std::map<std::string, int>::const_iterator it = m_param.RefEnum().begin() ; it != m_param.RefEnum().end() ; it++)
	{
		m_comboBox->addItem(it->first.c_str(), it->second);
	}
	GetCurrent();
}

ControllerEnum::~ControllerEnum()
{
	// delete(m_comboBox);
}

void ControllerEnum::SetControlledValue()
{
	m_param.SetValue(m_comboBox->itemData(m_comboBox->currentIndex()).toInt(), PARAMCONF_GUI);
}

void ControllerEnum::GetCurrent()
{
	int index = m_comboBox->findData(m_param.GetValue());
	m_comboBox->setCurrentIndex(index);
}

void ControllerEnum::GetDefault()
{
	int index = m_comboBox->findData(m_param.GetDefault());
	m_comboBox->setCurrentIndex(index);
}


/*------------------------------------------------------------------------------------------------*/
