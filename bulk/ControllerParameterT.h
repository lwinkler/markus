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

#ifndef CONTROLLER_PARAMETER_T_H
#define CONTROLLER_PARAMETER_T_H

#include "Parameter.h"
#include "ParameterT.h"
#include "ParameterEnum.h"
#include "Stream.h"
#include "Controller.h"

class QCheckBox;
class QLineEdit;
class QTextEdit;
class QComboBox;
class QLabel;

namespace mk {
class QParameterSlider;
class Processable;

QWidget* defaultCreateWidget(Parameter& rx_param, QWidget*& rpx_widget);
void defaultSetWidgetValue(const std::string& x_value, QWidget*& rpx_widget);
mkjson defaultGetValueFromWidget(QWidget*& rpx_widget);

/// Control class for any parameter
template<class T>class ControllerParameterT : public Controller
{
public:
	ControllerParameterT(Parameter& x_param, Processable& x_module) :
		Controller(x_param.GetName()),
		m_param(dynamic_cast<T&>(x_param)),
		m_module(x_module)
	{
		m_actions.insert(make_pair("GetClass",   &ControllerParameterT::GetClass2));
		m_actions.insert(make_pair("GetRange",   &ControllerParameterT::GetRange));
		m_actions.insert(make_pair("Set",        &ControllerParameterT::SetControlledValue));
		m_actions.insert(make_pair("Get",        &ControllerParameterT::GetCurrent));
		m_actions.insert(make_pair("GetDefault", &ControllerParameterT::GetDefault));
	}

	MKCLASS("ControllerParameter")

	QWidget* CreateWidget() override{
		return defaultCreateWidget(m_param, mp_widget);
	}
	virtual void SetWidgetValue(const std::string& x_value){
		defaultSetWidgetValue(x_value, mp_widget);
	}
	virtual mkjson GetValueFromWidget(){
		return defaultGetValueFromWidget(mp_widget);
	}

	// Controllers
	void GetClass2(mkjson& rx_value)
	{
		Processable::ReadLock lock(m_module.RefLock());
		rx_value = m_param.GetClass();
		LOG_INFO(m_logger, "Parameter class is \"" + m_param.GetClass() + "\"");
	}
	void GetRange(mkjson& rx_value)
	{
		Processable::ReadLock lock(m_module.RefLock());
		rx_value = m_param.GetRange();
		LOG_INFO(m_logger, "Parameter range is \"" + oneLine(m_param.GetRange()) + "\"");
	}
	void SetControlledValue(mkjson& rx_value)
	{
		Processable::WriteLock lock(m_module.RefLock());
		const auto oldValue = m_param.GetValue();
		ParameterConfigType configType = m_param.GetConfigurationSource();

		if(!rx_value.is_null())
		{
			m_param.SetValue(rx_value, PARAMCONF_CMD);
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
			catch(std::exception& e)
			{
				LOG_ERROR(m_logger, "Error setting value of parameter from widget: " + std::string(e.what()));
			}
#endif
		}
		if(!m_param.CheckRange())
		{
			m_param.SetValue(oldValue, configType);
			throw MkException("Parameter " + m_param.GetName() + "=" + oneLine(rx_value) + " is out of range " + oneLine(m_param.GetRange()), LOC);
		}
	}

	void GetCurrent(mkjson& rx_value)
	{
		Processable::ReadLock lock(m_module.RefLock());
		if(!rx_value.is_null())
		{
			rx_value = m_param.GetValue();
			return;
		}
#ifdef MARKUS_NO_GUI
		assert(false);
#else
		std::stringstream ss;
		ss << m_param.GetValue();
		SetWidgetValue(ss.str());
#endif
	}

	void GetDefault(mkjson& rx_value)
	{
		Processable::ReadLock lock(m_module.RefLock());
		if(!rx_value.is_null())
		{
			rx_value = m_param.GetDefault();
			return;
		}
#ifdef MARKUS_NO_GUI
		assert(false);
#else
		std::stringstream ss;
		ss<<m_param.GetDefault();
		SetWidgetValue(ss.str());
#endif
	}

	typedef void (ControllerParameterT::*action)(mkjson&);
	DECLARE_CALL_ACTION(action);
	DECLARE_LIST_ACTION(action);

protected:
	std::map<std::string, const action> m_actions;
	Processable& m_module;

	QWidget * mp_widget = nullptr;
	T& m_param;
};


// Specializations
template<> QWidget* ControllerParameterT<int>::CreateWidget();
template<> void ControllerParameterT<int>::SetWidgetValue(const std::string& x_value);
template<> mkjson ControllerParameterT<int>::GetValueFromWidget();
template<> QWidget* ControllerParameterT<uint>::CreateWidget();
template<> void ControllerParameterT<uint>::SetWidgetValue(const std::string& x_value);
template<> mkjson ControllerParameterT<uint>::GetValueFromWidget();
template<> QWidget* ControllerParameterT<double>::CreateWidget();
template<> void ControllerParameterT<double>::SetWidgetValue(const std::string& x_value);
template<> mkjson ControllerParameterT<double>::GetValueFromWidget();
template<> QWidget* ControllerParameterT<float>::CreateWidget();
template<> void ControllerParameterT<float>::SetWidgetValue(const std::string& x_value);
template<> mkjson ControllerParameterT<float>::GetValueFromWidget();
template<> QWidget* ControllerParameterT<bool>::CreateWidget();
template<> void ControllerParameterT<bool>::SetWidgetValue(const std::string& x_value);
template<> mkjson ControllerParameterT<bool>::GetValueFromWidget();


typedef ControllerParameterT<ParameterInt> ControllerInt;
typedef ControllerParameterT<ParameterUInt> ControllerUInt;
typedef ControllerParameterT<ParameterDouble> ControllerDouble;
typedef ControllerParameterT<ParameterFloat> ControllerFloat;
typedef ControllerParameterT<ParameterBool> ControllerBool;
typedef ControllerParameterT<ParameterString> ControllerString;
// typedef ControllerParameterT<ParameterSerializable> ControllerSerializable;
typedef ControllerParameterT<ParameterEnum> ControllerEnum;


} // namespace mk
#endif
