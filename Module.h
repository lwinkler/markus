#ifndef MODULE_H
#define MODULE_H

#include "cv.h"
#include "ConfigReader.h"
#include "Parameter.h"

class Module
{
public:
	Module(ConfigReader& x_confReader, int width, int height, int depth, int channels);
	~Module();
	
	virtual void Init();
	void ReadParametersFromConfig();
	virtual void ProcessFrame(const IplImage * m_input) = 0;
	virtual const char* GetName() = 0;
	const IplImage * GetOutput(){return m_output;}
	
private:
	ConfigReader& m_configReader;
	virtual ParameterStructure & GetRefParameter() = 0;
	
protected:
	double m_timeInterval;
	IplImage * m_output;
	const int m_width;
	const int m_height;
	const int m_depth;
	const int m_channels;
};

#endif