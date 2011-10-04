#include "ConfigReader.h"
#include "Module.h"
#include "Manager.h"

#include "ObjectTracker.h"
#include "SlitCam.h"

using namespace std;

/* Reads the configuration file with tinyxml */
//const char * ConfigReader::m_fileName = "config.xml";

void ConfigReader::ReadConfig(const std::string& x_moduleName)
{
	m_parameterList.clear();
	const char * fileName = m_fileName.c_str();
	TiXmlDocument doc( fileName );
	bool loadOkay = doc.LoadFile();

	if ( !loadOkay )
	{
		cout<<"Could not load test file '"<<fileName<<"'. Error='"<<doc.ErrorDesc()<<"'. Exiting."<<endl;
		exit( 1 );
	}

	//doc.Print( stdout );

	TiXmlNode* node = 0;
	TiXmlElement* moduleElement = 0;
	TiXmlElement* paramElement = 0;
	
	node = doc.FirstChild(x_moduleName);
	if(node == NULL)
		throw("Impossible to find module in config file." + x_moduleName);
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
		assert(name);
		//cout<<"name"<<name<<" value "<<value<<endl;
		
		m_parameterList.push_back(ParameterValue(atoi(id), std::string(name), value));
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
