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
#include "ParameterNum.h"
#include "ParameterEnum.h"
#include "ParameterString.h"
#include "ParameterSerializable.h"
#include "Stream.h"
#include "Controller.h"


class QParameterSlider;
class QCheckBox;
class QLineEdit;
class QTextEdit;
class QComboBox;
class QLabel;
class Processable;

/// Control class for any parameter
template<class T>class ControllerParameterT : public Controller
{
public:
	ControllerParameterT(Parameter& x_param, Processable& x_module) :
		Controller(x_param.GetName()),
		m_param(dynamic_cast<T&>(x_param)),
		m_module(x_module)
	{
		m_actions.insert(make_pair("GetType",    &ControllerParameterT::GetType));
		m_actions.insert(make_pair("GetRange",   &ControllerParameterT::GetRange));
		m_actions.insert(make_pair("Set",        &ControllerParameterT::SetControlledValue));
		m_actions.insert(make_pair("Get",        &ControllerParameterT::GetCurrent));
		m_actions.insert(make_pair("GetDefault", &ControllerParameterT::GetDefault));
	}

	MKCLASS("ControllerParameter")

	QWidget* CreateWidget() override;
	virtual void SetWidgetValue(const std::string& x_value);
	virtual Json::Value GetValueFromWidget();

	// Controllers
	void GetType(std::string* xp_value);
	void GetRange(std::string* xp_value);
	void SetControlledValue(std::string* xp_value);
	void GetCurrent(std::string* xp_value);
	void GetDefault(std::string* xp_value);

	typedef void (ControllerParameterT::*action)(std::string*);
	DECLARE_CALL_ACTION(action);
	DECLARE_LIST_ACTION(action);

protected:
	std::map<std::string, const action> m_actions;
	Processable& m_module;

	QWidget * mp_widget = nullptr;
	T& m_param;
};

typedef ControllerParameterT<ParameterInt> ControllerInt;
typedef ControllerParameterT<ParameterUInt> ControllerUInt;
typedef ControllerParameterT<ParameterDouble> ControllerDouble;
typedef ControllerParameterT<ParameterFloat> ControllerFloat;
typedef ControllerParameterT<ParameterBool> ControllerBool;
typedef ControllerParameterT<ParameterString> ControllerString;
typedef ControllerParameterT<ParameterSerializable> ControllerSerializable;
typedef ControllerParameterT<ParameterEnum> ControllerEnum;


/// Control class for a parameter: does nothing
class ControllerVoid : public Controller
{
public:
	ControllerVoid(Parameter& x_param, Processable& x_module) : Controller(x_param.GetName()) {}
	virtual QWidget* CreateWidget();
	virtual void SetWidgetValue(const std::string& x_value);
	virtual Json::Value GetValueFromWidget();

	virtual void ListActions(std::vector<std::string>& xr_actions) const {}
	virtual void CallAction(const std::string& x_name, std::string* xp_value) {}

	MKCLASS("ControllerParameter")

protected:
	// cppcheck-suppress unsafeClassCanLeak // we must return this pointer to Qt
	QLineEdit             * mp_lineEdit;
	// Stream & m_param2;
};

/*
/// Control class for a object height parameter
class ControllerCalibrationByHeight : public Controller
{
public:
	ControllerCalibrationByHeight(Parameter& x_param, Processable& x_module);
	virtual QWidget* CreateWidget();
	virtual void SetWidgetValue(const std::string& x_value);
	virtual Json::Value GetValueFromWidget();

	inline const std::string& GetClass() const override {return m_class;}

	typedef void (ControllerCalibrationByHeight::*action)(std::string*);
	DECLARE_CALL_ACTION(action);
	DECLARE_LIST_ACTION(action);

protected:
	Processable& m_module;
	std::map<std::string, const action> m_actions;
	//QPainter * parameterLine;
	ParameterSerializable & m_param2;
	// cppcheck-suppress unsafeClassCanLeak // we must return this pointer to Qt
	QWidget * mp_widget;
	// cppcheck-suppress unsafeClassCanLeak // we must return this pointer to Qt
	QParameterSlider * mp_sliderX;
	// cppcheck-suppress unsafeClassCanLeak // we must return this pointer to Qt
	QParameterSlider * mp_sliderY;
	// cppcheck-suppress unsafeClassCanLeak // we must return this pointer to Qt
	QParameterSlider * mp_sliderHeight;
	// cppcheck-suppress unsafeClassCanLeak // we must return this pointer to Qt
	QLabel* mp_labX;
	// cppcheck-suppress unsafeClassCanLeak // we must return this pointer to Qt
	QLabel* mp_labY;
	// cppcheck-suppress unsafeClassCanLeak // we must return this pointer to Qt
	QLabel* mp_labHeight ;

private:
	static const std::string m_class;
}; */

#endif
