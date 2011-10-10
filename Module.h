#ifndef MODULE_H
#define MODULE_H

#include "cv.h"
#include "ConfigReader.h"
#include "Parameter.h"
#include "OutputStream.h"

class ModuleParameterStructure : public ParameterStructure
{
public:
	ModuleParameterStructure(ConfigReader& x_confReader, const std::string& x_moduleName) : ParameterStructure(x_confReader, x_moduleName)
	{
		m_list.push_back(new ParameterT<int>(0, "width", 		640, 	PARAM_INT, 	0, 	4000,	&width));
		m_list.push_back(new ParameterT<int>(0, "height", 	480, 	PARAM_INT, 	0, 	3000,	&height));
		m_list.push_back(new ParameterT<int>(0, "depth", 	IPL_DEPTH_8U, PARAM_INT, 	0, 	32,	&depth));
		m_list.push_back(new ParameterT<int>(0, "channels", 	3, 	PARAM_INT, 	1, 	3,	&channels));
	};
	int width;
	int height;
	int depth;
	int channels;
	void Init()
	{
	};
};

class Module
{
public:
	Module(const std::string& x_name, ConfigReader& x_confReader);
	~Module();
	
	void ReadParametersFromConfig();
	virtual void ProcessFrame(const IplImage * m_input) = 0;
	const std::string& GetName(){return m_name;};
	const IplImage * GetOutput(){return m_output;}
	void AddStream(const std::string& x_name, StreamType x_type, IplImage* m_image);
	const std::list<OutputStream>& GetOutputStreamList(){return m_outputStreams;};
	
private:
	ConfigReader& m_configReader;
	virtual ModuleParameterStructure & GetRefParameter() = 0;
	
protected:
	double m_timeInterval;
	IplImage * m_output;
	const std::string m_name;
	std::list<OutputStream> m_outputStreams;
};

#endif