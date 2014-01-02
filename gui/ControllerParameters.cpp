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
	Controller(x_param.GetName()),
	param(x_param)
{
	m_actions.insert(std::make_pair("Set", &setControlledValueInt));
	m_actions.insert(std::make_pair("GetCurrent", &getCurrentInt));
	m_actions.insert(std::make_pair("GetDefault", &getDefaultInt));
}

ControllerInt::~ControllerInt()
{
	// delete(m_parameterSlider);
}

QWidget* ControllerInt::CreateWidget()
{
	return parameterSlider = new QParameterSlider(param.GetValue(), param.GetMin(), param.GetMax(), 0);
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
	Controller(x_param.GetName()),
	param(x_param)
{
	m_actions.insert(std::make_pair("Set", &setControlledValueDouble));
	m_actions.insert(std::make_pair("GetCurrent", &getCurrentDouble));
	m_actions.insert(std::make_pair("GetDefault", &getDefaultDouble));
}

ControllerDouble::~ControllerDouble()
{
	// delete(m_parameterSlider);
}

QWidget* ControllerDouble::CreateWidget()
{
	return parameterSlider = new QParameterSlider(param.GetValue(), param.GetMin(), param.GetMax(), PRECISION_DOUBLE);
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
	Controller(x_param.GetName()),
	param(x_param)
{
	m_actions.insert(std::make_pair("Set", &setControlledValueBool));
	m_actions.insert(std::make_pair("GetCurrent", &getCurrentBool));
	m_actions.insert(std::make_pair("GetDefault", &getDefaultBool));
}

ControllerBool::~ControllerBool()
{
	// delete(m_checkBox);
}

QWidget* ControllerBool::CreateWidget()
{
	checkBox = new QCheckBox("Enabled");
	(*getCurrentBool)(this);
	return checkBox;
}
/*------------------------------------------------------------------------------------------------*/
void setControlledValueString(Controller* x_ctr)
{
	ControllerString* ctr = dynamic_cast<ControllerString*>(x_ctr);
	assert(ctr != NULL);
	ctr->param.SetValue(ctr->lineEdit->text().toStdString(), PARAMCONF_GUI);
}

void getCurrentString(Controller* x_ctr)
{
	ControllerString* ctr = dynamic_cast<ControllerString*>(x_ctr);
	assert(ctr != NULL);
	ctr->lineEdit->setText(ctr->param.GetValue().c_str());
}

void getDefaultString(Controller* x_ctr)
{
	ControllerString* ctr = dynamic_cast<ControllerString*>(x_ctr);
	assert(ctr != NULL);
	ctr->lineEdit->setText(ctr->param.GetDefault().c_str());
}

ControllerString::ControllerString(ParameterString& x_param):
	Controller(x_param.GetName()),
	param(x_param)
{
	m_actions.insert(std::make_pair("Set", &setControlledValueString));
	m_actions.insert(std::make_pair("GetCurrent", &getCurrentString));
	m_actions.insert(std::make_pair("GetDefault", &getDefaultString));
}

ControllerString::~ControllerString()
{
	// CLEAN_DELETE(m_lineEdit); // TODO: this causes a crash at closing if the parameter screen is activated.
}

QWidget* ControllerString::CreateWidget()
{
	lineEdit = new QLineEdit();
	lineEdit->setStyleSheet("color: black; background-color: white");
	(*getCurrentString)(this);
	return lineEdit;
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
	Controller(x_param.GetName()),
	param(x_param)
{
	m_actions.insert(std::make_pair("Set", &setControlledValueFloat));
	m_actions.insert(std::make_pair("GetCurrent", &getCurrentFloat));
	m_actions.insert(std::make_pair("GetDefault", &getDefaultFloat));
}

ControllerFloat::~ControllerFloat()
{
	// delete(m_parameterSlider);
}


QWidget* ControllerFloat::CreateWidget()
{
	return parameterSlider = new QParameterSlider(param.GetValue(), param.GetMin(), param.GetMax(), PRECISION_DOUBLE);
}

/*------------------------------------------------------------------------------------------------*/
void setControlledValueEnum(Controller* x_ctr)
{
	ControllerEnum* ctr = dynamic_cast<ControllerEnum*>(x_ctr);
	assert(ctr != NULL);
	ctr->param.SetValue(ctr->comboBox->itemData(ctr->comboBox->currentIndex()).toInt(), PARAMCONF_GUI);
}

void getCurrentEnum(Controller* x_ctr)
{
	ControllerEnum* ctr = dynamic_cast<ControllerEnum*>(x_ctr);
	assert(ctr != NULL);
	int index = ctr->comboBox->findData(ctr->param.GetValue());
	ctr->comboBox->setCurrentIndex(index);
}

void getDefaultEnum(Controller* x_ctr)
{
	ControllerEnum* ctr = dynamic_cast<ControllerEnum*>(x_ctr);
	assert(ctr != NULL);
	int index = ctr->comboBox->findData(ctr->param.GetDefault());
	ctr->comboBox->setCurrentIndex(index);
}

ControllerEnum::ControllerEnum(ParameterEnum& x_param):
	Controller(x_param.GetName()),
	param(x_param)
{
	m_actions.insert(std::make_pair("Set", &setControlledValueEnum));
	m_actions.insert(std::make_pair("GetCurrent", &getCurrentEnum));
	m_actions.insert(std::make_pair("GetDefault", &getDefaultEnum));
}

ControllerEnum::~ControllerEnum()
{
	// delete(comboBox);
}

QWidget* ControllerEnum::CreateWidget()
{
	comboBox = new QComboBox;
	for(std::map<std::string, int>::const_iterator it = param.RefEnum().begin() ; it != param.RefEnum().end() ; it++)
	{
		comboBox->addItem(it->first.c_str(), it->second);
	}
	(*getCurrentEnum)(this);
	return comboBox;
}


/*------------------------------------------------------------------------------------------------*/
