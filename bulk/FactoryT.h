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
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with Markus.  If not, see <http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------------------*/

#ifndef FACTORY_T_H
#define FACTORY_T_H

#include "MkException.h"
#include "define.h"

#include <map>
#include <vector>
#include <sstream>



/// This class is a template class for factories: a factory creates an instance of an object, the type of the object is specified as a string
template<class T0, class T1, typename... Args> class FactoryT
{
	typedef T1* (*CreateObjectFunc)(Args... args);
	typedef std::map<T0, CreateObjectFunc> Registry;
	template<class T2> static T1* createObject(Args... args) {return new T2(args...);}

public:
	FactoryT() {}
	template<class T2> void Register(const T0& name)
	{
		CreateObjectFunc func = createObject<T2>;
		auto it = m_register.find(name);
		if (it != m_register.end())
		{
			std::stringstream ss;
			ss << "Cannot register two instances with the same name: "<<name;
			throw MkException(ss.str(), LOC);
		}
		m_register.insert(typename FactoryT<T0, T1, Args...>::Registry::value_type(name, func));
	}

	/// Create a new instance
	T1 * Create(const T0& x_type, Args... args) const
	{
		auto it = m_register.find(x_type);

		if (it == m_register.end())
		{
			std::stringstream ss;
			ss << "Cannot find a constructor for module of type "<<x_type;
			throw MkException(ss.str(), LOC);
		}

		CreateObjectFunc func = it->second;
		T1* pmod = func(args...);
		// if(pmod->GetClass() != x_type)
		// throw MkException("Module \"" + x_type + "\" must have the same name as its class \"" + pmod->GetClass() + "\"", LOC);
		return pmod;
	}

	/// List all available types registred
	void List(std::vector<T0>& xr_types) const
	{
		xr_types.clear();
		for(const auto it : m_register)
			xr_types.push_back(it.first);
	}

private:
	DISABLE_COPY(FactoryT)
	Registry m_register;
};
#endif
