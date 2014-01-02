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
	ctr->param.SetValue(ctr->parameterSlider->GetValue(), PARAMCONF_GUI);
}

/// Display the current value of the controlled object
void getCurrentInt(Controller* x_ctr)
{
	ControllerInt* ctr = dynamic_cast<ControllerInt*>(x_ctr);
	assert(ctr != NULL);
	ctr->parameterSlider->SetValue(ctr->param.GetValue());
}

/// Display the default value of the controlled object
void getDefaultInt(Controller* x_ctr)
{
	ControllerInt* ctr = dynamic_cast<ControllerInt*>(x_ctr);
	assert(ctr != NULL);
	ctr->parameterSlider->SetValue(ctr->param.GetDefault());
}

ControllerInt::ControllerInt(ParameterInt& x_param):
	Controller(),
	param(x_param)
{
	m_widget = parameterSlider = new QParameterSlider(param.GetValue(), param.GetMin(), param.GetMax(), 0);
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
void setControlledValueDouble(Controller* x_ctr)
{
	ControllerDouble* ctr = dynamic_cast<ControllerDouble*>(x_ctr);
	assert(ctr != NULL);
	ctr->param.SetValue(ctr->parameterSlider->GetValue(), PARAMCONF_GUI);
}

void getCurrentDouble(Controller* x_ctr)
{
	ControllerDouble* ctr = dynamic_cast<ControllerDouble*>(x_ctr);
	assert(ctr != NULL);
	ctr->parameterSlider->SetValue(ctr->param.GetValue());
}

void getDefaultDouble(Controller* x_ctr)
{
	ControllerDouble* ctr = dynamic_cast<ControllerDouble*>(x_ctr);
	assert(ctr != NULL);
	ctr->parameterSlider->SetValue(ctr->param.GetDefault());
}

ControllerDouble::ControllerDouble(ParameterDouble& x_param):
	Controller(),
	param(x_param)
{
	m_widget = parameterSlider = new QParameterSlider(param.GetValue(), param.GetMin(), param.GetMax(), PRECISION_DOUBLE);
	m_actions.insert(std::make_pair("Set", &setControlledValueDouble));
	m_actions.insert(std::make_pair("GetCurrent", &getCurrentDouble));
	m_actions.insert(std::make_pair("GetDefault", &getDefaultDouble));
}

ControllerDouble::~ControllerDouble()
{
	// delete(m_parameterSlider);
}

/*------------------------------------------------------------------------------------------------*/

void setControlledValueBool(Controller* x_ctr)
{
	ControllerBool* ctr = dynamic_cast<ControllerBool*>(x_ctr);
	assert(ctr != NULL);
	ctr->param.SetValue(ctr->checkBox->isChecked(), PARAMCONF_GUI);
}

void getCurrentBool(Controller* x_ctr)
{
	ControllerBool* ctr = dynamic_cast<ControllerBool*>(x_ctr);
	assert(ctr != NULL);
	ctr->checkBox->setChecked(ctr->param.GetValue());
}

void getDefaultBool(Controller* x_ctr)
{
	ControllerBool* ctr = dynamic_cast<ControllerBool*>(x_ctr);
	assert(ctr != NULL);
	ctr->checkBox->setChecked(ctr->param.GetDefault());
}

ControllerBool::ControllerBool(ParameterBool& x_param):
	Controller(),
	param(x_param)
{
	m_widget = checkBox = new QCheckBox("Enabled");
	(*getCurrentBool)(this);
	m_actions.insert(std::make_pair("Set", &setControlledValueBool));
	m_actions.insert(std::make_pair("GetCurrent", &getCurrentBool));
	m_actions.insert(std::make_pair("GetDefault", &getDefaultBool));
}

ControllerBool::~ControllerBool()
{
	// delete(m_checkBox);
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

void setControlledValueFloat(Controller* x_ctr)
{
	ControllerFloat* ctr = dynamic_cast<ControllerFloat*>(x_ctr);
	assert(ctr != NULL);
	ctr->param.SetValue(ctr->parameterSlider->GetValue(), PARAMCONF_GUI);
}

void getCurrentFloat(Controller* x_ctr)
{
	ControllerFloat* ctr = dynamic_cast<ControllerFloat*>(x_ctr);
	assert(ctr != NULL);
	ctr->parameterSlider->SetValue(ctr->param.GetValue());
}

void getDefaultFloat(Controller* x_ctr)
{
	ControllerFloat* ctr = dynamic_cast<ControllerFloat*>(x_ctr);
	assert(ctr != NULL);
	ctr->parameterSlider->SetValue(ctr->param.GetDefault());
}

ControllerFloat::ControllerFloat(ParameterFloat& x_param):
	Controller(),
	param(x_param)
{
	m_widget = parameterSlider = new QParameterSlider(param.GetValue(), param.GetMin(), param.GetMax(), PRECISION_DOUBLE);
	m_actions.insert(std::make_pair("Set", &setControlledValueFloat));
	m_actions.insert(std::make_pair("GetCurrent", &getCurrentFloat));
	m_actions.insert(std::make_pair("GetDefault", &getDefaultFloat));
}

ControllerFloat::~ControllerFloat()
{
	// delete(m_parameterSlider);
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
