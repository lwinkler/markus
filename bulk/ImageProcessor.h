#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include "cv.h"
#include "ConfigReader.h"
#include "Parameter.h"
//#include "OutputStream.h"
#include "Input.h"

class Module;
//class Input;

class ImageProcessorParameterStructure : public ParameterStructure
{
public:
	ImageProcessorParameterStructure(ConfigReader& x_confReader, int x_nb) : ParameterStructure(x_confReader, "ImageProcessors", "ImageProcessor", x_nb)
	{
		m_list.push_back(new ParameterT<std::string>(0, "input",  "(definp)",	PARAM_STR,	&input));
		m_list.push_back(new ParameterT<std::string>(0, "module", "(defmod)",	PARAM_STR,	&module));
	};

public:
	std::string input;
	std::string module;
};

class ImageProcessor : Configurable
{
public:
	ImageProcessor(const std::string& x_name, int x_nb, ConfigReader& x_confReader);
	//~ImageProcessor();
	
	void Process();
	const int & GetNumber(){return m_nb;};
	
	Input & GetInput() {return *m_input;}
	Module & GetModule() {return *m_module;}
	
protected:
	//const std::string m_name;
	const int m_nb;
	Module * m_module;
	Input * m_input;
	
	ImageProcessorParameterStructure m_param;
	virtual const ParameterStructure& GetRefParameter(){return m_param;};
};

#endif