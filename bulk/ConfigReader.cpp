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
//const char * ConfigReader::m_fileName = "config.xml";

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

vector< ParameterValue > ConfigReader::ReadParameters(const std::string& rx_type, const std::string& rx_name) const
{
	vector<ParameterValue> parameterList;
	
	TiXmlElement* element = mp_node->ToElement();
	//TiXmlNode* nodeParams = element->FirstChild(rx_type);
	//if(nodeParams == NULL) throw("Impossible to find <" + rx_type + "> section");
	//TiXmlElement* paramElement = nodeParams->ToElement();
	
	for( TiXmlNode * node = element->FirstChild(rx_type);
		node;
		node = node->NextSibling(rx_type) )
	{
		element = node->ToElement();
		
		const char* id 		= element->Attribute("id");
		const char* name  	= element->Attribute("name");
		const char* class1  	= element->Attribute("class");
		const char* value 	= element->GetText();
		if(id == NULL) id = "-1";
		assert(name);
		assert(value);
		if(class1 == NULL)
			class1 = "";
		//cout<<"name"<<name<<" value "<<value<<endl;
		//if(!x_getClassOnly || !strcmp(name, "class"))
			parameterList.push_back(ParameterValue(atoi(id), std::string(name),  value));
	}
	return parameterList;
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

#if 0
vector<ParameterValue> ConfigReader::ReadParameters(const std::string& x_objectType, const std::string& x_objectName, bool x_getClassOnly) const
{
	vector<ParameterValue> parameterList;
	bool loadOkay = doc.LoadFile();
//printf("ReadConfigObject(%s, %s)\n", x_objectType.c_str(), x_objectName.c_str());
	if ( !loadOkay )
		throw("Could not load test file '" + m_fileName + "'. Error='" + doc.ErrorDesc() + "'. Exiting.");

	TiXmlNode* node = 0;
	TiXmlElement* moduleElement = 0;
	TiXmlElement* paramElement = 0;
	
	node = doc.FirstChild(x_objectType);
	
	// If a name is specified, search this name
	if(x_objectName.compare(""))
	{
		while(node != NULL && x_objectName.compare(node->ToElement()->Attribute("name")))
		{
			//printf("Name = %s\n", node->ToElement()->Attribute("name"));
			node = node->NextSibling(x_objectType);
		}
	}
	if(node == NULL) 
		throw("Impossible to find <" + x_objectType + " name=" + x_objectName + "> in config file");
	
	moduleElement = node->ToElement();
	assert( moduleElement  );
	
	TiXmlNode* nodeParams = moduleElement->FirstChild( "parameters" );
	if(nodeParams == NULL) throw("Impossible to find <parameters> section for module " + x_objectName + " of type " + x_objectType);
	TiXmlElement* paramsElement = nodeParams->ToElement();
	
	for( node = paramsElement->FirstChild( "param" );
		node;
		node = node->NextSibling( "param" ) )
	{
		paramElement = node->ToElement();
		
		const char* id = paramElement->Attribute( "id");
		const char* name  = paramElement->Attribute( "name");
		const char* class1  = paramElement->Attribute( "class");
		const char* value = paramElement->GetText();
		if(id == NULL) id = "-1";
		assert(name);
		assert(value);
		if(class1 == NULL)
			class1 = "";
		//cout<<"name"<<name<<" value "<<value<<endl;
		if(!x_getClassOnly || !strcmp(name, "class"))
			parameterList.push_back(ParameterValue(atoi(id), std::string(name),  value));
	}
	
	return parameterList;
}


// Read the config of all modules that will run

vector<ParameterValue> ConfigReader::ReadModules(const std::string& x_vectorType, const std::string& x_objectType, bool x_getClassOnly) const
{
	//assert(false);
	vector<ParameterValue> parameterList;;
	TiXmlDocument doc( m_fileName );
	bool loadOkay = doc.LoadFile();
//printf("ReadConfigObjectFromVect(%s, %s)\n", x_vectorType.c_str(), x_objectType.c_str());

	if ( !loadOkay )
		throw("Could not load test file '" + m_fileName + "'. Error='" + doc.ErrorDesc() + "'. Exiting.");

	TiXmlNode* node = 0;
	TiXmlElement* moduleElement = 0;
	TiXmlElement* paramElement = 0;
	
	if(x_vectorType.size() == 0)
	{
		node = doc.FirstChild( x_objectType );
	}
	else
	{
		node = doc.FirstChild(x_vectorType);
		if(node == NULL)
			throw("Impossible to find <" + x_vectorType + "> in config file.");
		moduleElement = node->ToElement();
		assert( moduleElement  );
		node = moduleElement->FirstChild( x_objectType );
	}
	
	/*for( ; node ; node = node->NextSibling( x_objectType ) )
	{
		if(x_objectNumber == 0) break;
		x_objectNumber --;
	}
	
	if(node == NULL) return parameterList;
	
	moduleElement = node->ToElement();
	assert( moduleElement  );*/
	
	for(TiXmlNode* nodeParam = moduleElement->FirstChild( "module" );
	nodeParam;
	nodeParam = nodeParam->NextSibling( "module" ) )
	{

		paramElement = nodeParam->ToElement();
		
		const char* id = paramElement->Attribute( "id");
		//const char* class1 = paramElement->Attribute( "class");
		const char* name = paramElement->Attribute( "name");
		const char* value = paramElement->GetText();
		if(id == NULL) id = "-1";
		assert(name);
		//if(class1 == NULL) class1 = "";
		//cout<<"name"<<name<<" value "<<value<<endl;
		if(!x_getClassOnly || !strcmp(name, "class"))
			parameterList.push_back(ParameterValue(atoi(id), name, value));
	}

	
	return parameterList;
}

#endif

ParameterValue ConfigReader::GetParameterValue(const std::string& x_name, const vector<ParameterValue> & x_parameterList)
{
	for(vector<ParameterValue>::const_iterator it = x_parameterList.begin(); it != x_parameterList.end(); it++)
	{
		if(x_name.compare(it->m_name) == 0)
			return *it;
	}
	assert(false);
	return ParameterValue(0, "", "");
}
/*
// Return the size of a vector of config objects
int ConfigReader::ReadConfigGetVectorSize(const std::string& x_vectorType, const std::string& x_objectType) const
{
	vector<ParameterValue> parameterList;;
	TiXmlDocument doc( m_fileName );
	bool loadOkay = doc.LoadFile();

	if ( !loadOkay )
		throw("Could not load test file '" + m_fileName + "'. Error='" + doc.ErrorDesc() + "'. Exiting.");

	TiXmlNode* node = 0;
	TiXmlElement* moduleElement = 0;
	//TiXmlElement* paramElement = 0;
	
	if(x_vectorType.size() == 0)
	{
		node = doc.FirstChild( x_objectType );
	}
	else
	{
		node = doc.FirstChild(x_vectorType);
		if(node == NULL)
			throw("Impossible to find <" + x_vectorType + "> in config file.");
		moduleElement = node->ToElement();
		assert( moduleElement  );
		node = moduleElement->FirstChild( x_objectType );
	}
	int cpt = 0;
	for( ;
		node;
		node = node->NextSibling( x_objectType ) )
	{
		cpt++;
	}

	
	return cpt;
	
}
*/
