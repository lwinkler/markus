#include "ConfigReader.h"
#include "Module.h"
#include "Manager.h"

#include "ObjectTracker.h"
#include "SlitCam.h"

using namespace std;

/* Reads the configuration file with tinyxml */
//const char * ConfigReader::m_fileName = "config.xml";

void ConfigReader::ReadConfigModule(const std::string& x_moduleName)
{
	m_parameterList.clear();
	TiXmlDocument doc( m_fileName );
	bool loadOkay = doc.LoadFile();

	if ( !loadOkay )
		throw("Could not load test file '" + m_fileName + "'. Error='" + doc.ErrorDesc() + "'. Exiting.");

	TiXmlNode* node = 0;
	TiXmlElement* moduleElement = 0;
	TiXmlElement* paramElement = 0;
	
	node = doc.FirstChild(x_moduleName);
	if(node == NULL)
		throw("Impossible to find module in config file : " + x_moduleName);
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
		const char* name = paramElement->Attribute( "name");
		const char* value = paramElement->GetText();
		if(id == NULL) id = "-1";
		assert(name);
		assert(value);
		//cout<<"name"<<name<<" value "<<value<<endl;
		
		m_parameterList.push_back(ParameterValue(atoi(id), std::string(name), value));
	}
}

void ConfigReader::ReadConfigModules()
{
	m_parameterList.clear();
	TiXmlDocument doc( m_fileName );
	bool loadOkay = doc.LoadFile();

	if ( !loadOkay )
		throw("Could not load test file '" + m_fileName + "'. Error='" + doc.ErrorDesc() + "'. Exiting.");

	TiXmlNode* node = 0;
	TiXmlElement* moduleElement = 0;
	TiXmlElement* paramElement = 0;
	
	node = doc.FirstChild("Modules");
	if(node == NULL)
		throw("Impossible to find <Modules> in config file.");
	moduleElement = node->ToElement();
	assert( moduleElement  );
	
	/*node = moduleElement->FirstChildElement();	// This skips the "PDA" comment.
	assert( node );
	paramElement = node->ToElement();
	assert( paramElement  );*/
	
	for( node = moduleElement->FirstChild( "module" );
		node;
		node = node->NextSibling( "module" ) )
	{
		paramElement = node->ToElement();
		
		const char* id = paramElement->Attribute( "id");
		const char* type = paramElement->Attribute( "type");
		const char* name = paramElement->GetText();
		if(id == NULL) id = "-1";
		assert(name);
		assert(type);
		//cout<<"name"<<name<<" value "<<value<<endl;
		
		m_parameterList.push_back(ParameterValue(atoi(id), type, name));
	}
}


string ConfigReader::GetParameterValue(const std::string& x_name) const
{
	for(list<ParameterValue>::const_iterator it = m_parameterList.begin(); it != m_parameterList.end(); it++)
	{
		if(x_name.compare(it->m_name) == 0)
			return it->m_value;
	}
}
