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


vector<ParameterValue> ConfigReader::ReadConfigObject(const std::string& x_objectType, const std::string& x_objectName)
{
	vector<ParameterValue> parameterList;
	TiXmlDocument doc( m_fileName );
	bool loadOkay = doc.LoadFile();
printf("ReadConfigObject(%s, %s)\n", x_objectType.c_str(), x_objectName.c_str());
	if ( !loadOkay )
		throw("Could not load test file '" + m_fileName + "'. Error='" + doc.ErrorDesc() + "'. Exiting.");

	TiXmlNode* node = 0;
	TiXmlElement* moduleElement = 0;
	TiXmlElement* paramElement = 0;
	
	node = doc.FirstChild(x_objectType);
	
	// If a name is specified, search this name
	if(x_objectName.compare(""))
		while(node != NULL && x_objectName.compare(node->ToElement()->Attribute("name")))
		{
			//printf("Name = %s\n", node->ToElement()->Attribute("name"));
			node = node->NextSibling(x_objectType);
		}
	if(node == NULL) throw("Impossible to find <" + x_objectType + " name=" + x_objectName + "> in config file");
	moduleElement = node->ToElement();
	assert( moduleElement  );
	
	/*node = moduleElement->FirstChildElement();	// This skips the "PDA" comment.
	assert( node );
	paramElement = node->ToElement();
	assert( paramElement  );*/
	
	for( node = moduleElement->FirstChild( "parameter" );
		node;
		node = node->NextSibling( "parameter" ) )
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
		
		parameterList.push_back(ParameterValue(atoi(id), std::string(name), std::string(class1), value));
	}
	
	return parameterList;
}

// Read the config of all modules that will run

vector<ParameterValue> ConfigReader::ReadConfigObjectFromVect(const std::string& x_vectorType, const std::string& x_objectType, int x_objectNumber)
{
	vector<ParameterValue> parameterList;;
	TiXmlDocument doc( m_fileName );
	bool loadOkay = doc.LoadFile();
printf("ReadConfigObjectFromVect(%s, %s)\n", x_vectorType.c_str(), x_objectType.c_str());

	if ( !loadOkay )
		throw("Could not load test file '" + m_fileName + "'. Error='" + doc.ErrorDesc() + "'. Exiting.");

	TiXmlNode* node = 0;
	TiXmlElement* moduleElement = 0;
	TiXmlElement* paramElement = 0;
	
	node = doc.FirstChild(x_vectorType);
	if(node == NULL)
		throw("Impossible to find <" + x_vectorType + "> in config file.");
	moduleElement = node->ToElement();
	assert( moduleElement  );
	
	/*node = moduleElement->FirstChildElement();	// This skips the "PDA" comment.
	assert( node );
	paramElement = node->ToElement();
	assert( paramElement  );*/
	
	for( node = moduleElement->FirstChild( x_objectType );
		node;
		node = node->NextSibling( x_objectType ) )
	{
		if(x_objectNumber == 0) break;
		x_objectNumber --;
	}
	
	if(node == NULL) return parameterList;
	
	moduleElement = node->ToElement();
	assert( moduleElement  );
	
	for(TiXmlNode* nodeParam = moduleElement->FirstChild( "parameter" );
	nodeParam;
	nodeParam = nodeParam->NextSibling( "parameter" ) )
	{

		paramElement = nodeParam->ToElement();
		
		const char* id = paramElement->Attribute( "id");
		const char* class1 = paramElement->Attribute( "class");
		const char* name = paramElement->Attribute( "name");
		const char* value = paramElement->GetText();
		if(id == NULL) id = "-1";
		assert(name);
		if(class1 == NULL) class1 = "";
		//cout<<"name"<<name<<" value "<<value<<endl;
		
		parameterList.push_back(ParameterValue(atoi(id), name, class1, value));
	}

	
	return parameterList;
}


ParameterValue ConfigReader::GetParameterValue(const std::string& x_name, const vector<ParameterValue> & x_parameterList)
{
	for(vector<ParameterValue>::const_iterator it = x_parameterList.begin(); it != x_parameterList.end(); it++)
	{
		if(x_name.compare(it->m_name) == 0)
			return *it;
	}
}

// Return the size of a vector of config objects
int ConfigReader::ReadConfigGetVectorSize(const std::string& x_vectorType, const std::string& x_objectType)
{
	vector<ParameterValue> parameterList;;
	TiXmlDocument doc( m_fileName );
	bool loadOkay = doc.LoadFile();

	if ( !loadOkay )
		throw("Could not load test file '" + m_fileName + "'. Error='" + doc.ErrorDesc() + "'. Exiting.");

	TiXmlNode* node = 0;
	TiXmlElement* moduleElement = 0;
	TiXmlElement* paramElement = 0;
	
	node = doc.FirstChild(x_vectorType);
	if(node == NULL)
		throw("Impossible to find <" + x_vectorType + "> in config file.");
	moduleElement = node->ToElement();
	assert( moduleElement  );
	
	int cpt = 0;
	for( node = moduleElement->FirstChild( x_objectType );
		node;
		node = node->NextSibling( x_objectType ) )
	{
		cpt++;
	}

	
	return cpt;
	
}
