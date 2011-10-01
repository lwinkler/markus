#ifndef MODULE_H
#define MODULE_H

#include "cv.h"
#include "ConfigReader.h"

class Module
{
public:
	Module(ConfigReader& x_confReader, int width, int height, int depth, int channels);
	~Module();
	
	virtual void Init() = 0;
	
	virtual void ProcessFrame(const IplImage * m_input) = 0;
	virtual const char* GetName() = 0;
	const IplImage * GetOutput(){return m_output;}
	
private:
	ConfigReader& m_configReader;
	
protected:
	double m_timeInterval;
	IplImage * m_output;
};

#endif