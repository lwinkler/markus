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

#include "FactoryFeatures.h"
#include "AllFeatures.h"

using namespace std;

FactoryFeatures::FactoryFeatures()
{
	registerAllFeatures(*this);	
}

/**
* @brief Create a feature of given type
*
* @param x_type   Type of the feature as string
*
* @return 
*/
Feature * FactoryFeatures::CreateFeature(const std::string& x_type) const
{
	FeatureRegistry::const_iterator it = m_register.find(x_type);

	if (it == m_register.end())
	{
		throw MkException("Cannot find a constructor for feature of type " + x_type, LOC);
	}

	CreateFeatureFunc func = it->second;
	Feature* pmod = func();

	return pmod;
}

/**
* @brief Create a feature from its signature
*
* @param x_type   Signature of the feature as string
*
* @return 
*/
Feature * FactoryFeatures::CreateFeatureFromSignature(const std::string& x_signature) const
{
	FeatureRegistry::const_iterator it = m_registerBySignature.find(x_signature);

	if (it == m_registerBySignature.end())
	{
		throw MkException("Cannot find a constructor for feature of type " + x_signature, LOC);
	}

	CreateFeatureFunc func = it->second;
	Feature* pmod = func();
	// if(pmod->GetClass() != x_signature)
		// throw MkException("Feature \"" + x_signature + "\" must have the same name as its class \"" + pmod->GetClass() + "\"", LOC);
	return pmod;
}

/**
* @brief List all modules
*
* @param xr_types Output: The list of modules is added to this vector
*/
void FactoryFeatures::ListFeatures(vector<string>& xr_types) const
{
	for(FeatureRegistry::const_iterator it = m_register.begin() ; it != m_register.end() ; ++it)
		xr_types.push_back(it->first);
}
