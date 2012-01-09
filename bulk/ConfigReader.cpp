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

#include "ConfigReader.h"
#include "Module.h"
#include "Manager.h"

using namespace std;

/* Reads the configuration file with tinyxml */

ConfigReader ConfigReader::SubConfig(const std::string& x_objectType, string x_objectName) const
{
	TiXmlNode* newNode = mp_node->FirstChild(x_objectType);
	
	if(x_objectName.compare(""))
	{
		while(newNode != NULL && x_objectName.compare(newNode->ToElement()->Attribute("name")))
		{
			//printf("Name = %s\n", newNode->ToElement()->Attribute("name"));
			newNode = newNode->NextSibling(x_objectType);
		}
	}
	//if(newNode == NULL) 
	//	throw("Impossible to find <" + x_objectType + " name=" + x_objectName + "> in config file");
	
	//moduleElement = newNode->ToElement();
	//assert( moduleElement  );
	
	return ConfigReader(newNode);
}

ConfigReader ConfigReader::NextSubConfig(const std::string& x_objectType, string x_objectName) const
{
	TiXmlNode* newNode = mp_node->NextSibling(x_objectType);
	
	if(x_objectName.compare(""))
	{
		while(newNode != NULL && x_objectName.compare(newNode->ToElement()->Attribute("name")))
		{
			//printf("Name = %s\n", newNode->ToElement()->Attribute("name"));
			newNode = newNode->NextSibling(x_objectType);
		}
	}
	//if(newNode == NULL) 
	//	throw("Impossible to find <" + x_objectType + " name=" + x_objectName + "> in config file");
	
	//moduleElement = newNode->ToElement();
	//assert( moduleElement  );
	
	return ConfigReader(newNode);
}

const string ConfigReader::GetAttribute(const std::string& x_attributeName) const
{
	TiXmlElement* element = mp_node->ToElement();
	//string s(*element->Attribute(x_attributeName));
	
	const string * str = element->Attribute(x_attributeName);
	if(str == NULL)
		return "";
	else
		return *str;
}

const string ConfigReader::GetValue() const
{
	TiXmlElement* element = mp_node->ToElement();
	const char * str = element->GetText();
	if(str == NULL)
		return "";
	else
		return str;
}

