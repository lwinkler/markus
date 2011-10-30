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

	if ( !loadOkay )
		throw("Could not load test file '" + m_fileName + "'. Error='" + doc.ErrorDesc() + "'. Exiting.");

	TiXmlNode* node = 0;
	TiXmlElement* moduleElement = 0;
	TiXmlElement* paramElement = 0;
	
	node = doc.FirstChild(x_objectName);
	if(node == NULL)
		throw("Impossible to find module in config file : " + x_objectName);
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
		const char* type  = paramElement->Attribute( "type");
		const char* value = paramElement->GetText();
		if(id == NULL) id = "-1";
		assert(name);
		assert(value);
		if(type == NULL)
			type = "";
		//cout<<"name"<<name<<" value "<<value<<endl;
		
		parameterList.push_back(ParameterValue(atoi(id), std::string(name), std::string(type), value));
	}
	
	return parameterList;
}

// Read the config of all modules that will run

vector<ParameterValue> ConfigReader::ReadConfigObjectFromVect(const std::string& x_vectorType, const std::string& x_objectType, int x_objectNumber)
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
		const char* type = paramElement->Attribute( "type");
		const char* name = paramElement->Attribute( "name");
		const char* value = paramElement->GetText();
		if(id == NULL) id = "-1";
		assert(name);
		assert(type);
		//cout<<"name"<<name<<" value "<<value<<endl;
		
		parameterList.push_back(ParameterValue(atoi(id), name, type, value));
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

int ConfigReader::ReadConfigGetVectorSize(const std::string& x_vectorType, const std::string& x_type)
{

}
