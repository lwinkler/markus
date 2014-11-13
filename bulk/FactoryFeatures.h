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
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  <See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with Markus.  If not, see <http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------------------*/

#ifndef FACTORY_FEATURES_H
#define FACTORY_FEATURES_H

#include "MkException.h"
#include "Feature.h"

#include <map>
#include <string>
#include <vector>
#include <assert.h>



/// This class is a factory for features: it creates a feature of each type as specified by a string
class FactoryFeatures
{
	typedef Feature* (*CreateFeatureFunc)();
	typedef std::map<std::string, CreateFeatureFunc> FeatureRegistry;
	template<class T> static Feature* createFeature() {return new T();}

public:
	FactoryFeatures();
	template<class T> void RegisterFeature(const std::string& x_name)
	{
		CreateFeatureFunc func = createFeature<T>;
		assert(m_register.find(x_name) == m_register.end());
		m_register.insert(FeatureRegistry::value_type(x_name, func));

		// Temporrily create a feature to get its signature
		Feature* feat = func();

		// std::cout<<"Register "<<feat->Signature()<<std::endl;
		assert(m_registerBySignature.find(x_name) == m_registerBySignature.end());
		m_registerBySignature.insert(FeatureRegistry::value_type(feat->Signature(), func));
		delete feat;
	}
	Feature * CreateFeature(const std::string& x_type) const;
	Feature * CreateFeatureFromSignature(const std::string& x_type) const;
	void RegisterAllFeatures();
	void ListFeatures(std::vector<std::string>& xr_types) const;

protected:
	FeatureRegistry m_register;
	FeatureRegistry m_registerBySignature;
};
#endif
