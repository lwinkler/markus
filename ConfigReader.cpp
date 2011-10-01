#include "ConfigReader.h"
#include "Module.h"
#include "Manager.h"
#include "Detector.h"

using namespace std;

/* Reads the configuration file with tinyxml */

void ConfigReader::ReadConfig(Manager * x_manager)
{
	TiXmlDocument doc( "../config.xml" );
	bool loadOkay = doc.LoadFile();

	if ( !loadOkay )
	{
		printf( "Could not load test file '%s'. Error='%s'. Exiting.\n", m_fileName.c_str(), doc.ErrorDesc() );
		exit( 1 );
	}

	//doc.Print( stdout );

	TiXmlNode* node = 0;
	TiXmlElement* moduleElement = 0;
	TiXmlElement* paramElement = 0;
	
	node = doc.FirstChild( "Global");
	assert( node );
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
		
		if(!strcmp(name, "input"))x_manager->m_input = value;
		if(!strcmp(name, "mode"))x_manager->m_mode = value;
	}
}


void ConfigReader::ReadConfig(Module * x_module)
{
	TiXmlDocument doc( m_fileName );
	bool loadOkay = doc.LoadFile();

	if ( !loadOkay )
	{
		printf( "Could not load test file '%s'. Error='%s'. Exiting.\n", m_fileName.c_str(), doc.ErrorDesc() );
		exit( 1 );
	}

	//doc.Print( stdout );

	TiXmlNode* node = 0;
	TiXmlElement* moduleElement = 0;
	TiXmlElement* paramElement = 0;
	if(x_module == NULL)
	{
		assert(false);
	}
	else if(x_module->GetName() == "ObjectTracker")
	{
		Detector * detect = dynamic_cast<Detector*>(x_module);
		node = doc.FirstChild( "ObjectTracker");
		assert( node );
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
			
			if(!strcmp(name, "background_alpha"))detect->SetBackgroundAlpha((float) atof(value));
			if(!strcmp(name, "foreground_thres"))detect->SetForegroundThres((float) atof(value));
			if(!strcmp(name, "foreground_filter_size"))detect->SetForegroundFilterSize(atoi(value));
			if(!strcmp(name, "input_blur_size"))detect->SetInputBlurSize(atoi(value));
		}
	}
}
