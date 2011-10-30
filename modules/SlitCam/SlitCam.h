// +--------------------------------------------------------------------------+
// | File      : SlitCam.h                                                    |
// | Utility   : Module for a one slit camera                                 |
// | Author    : Laurent Winkler                                              |
// | Date      : Sept 2011                                                    |
// | Remarque  : none                                                         |
// +--------------------------------------------------------------------------+
#ifndef SLITCAM_H
#define SLITCAM_H

#include <cv.h>
#include "Module.h"
#include "Parameter.h"


/*! \class SlitCam
 *  \brief Class containing methods/attributes of a slit camera
 *
 */


class SlitCamParameter : public ModuleParameterStructure
{
	
public:
	SlitCamParameter(ConfigReader& x_confReader, const std::string& x_moduleName) : 
		ModuleParameterStructure(x_confReader, x_moduleName)
	{
		m_list.push_back(new ParameterT<int>(0, "aperture", 1, PARAM_INT, 1, 10, &aperture));
		
		ParameterStructure::Init();
	};
	
	int aperture;
};

class SlitCam : public Module
{
private:
	SlitCamParameter m_param;
	int m_position;
	static const char * m_type;
public:
	SlitCam(const std::string& x_name, ConfigReader& x_configReader);
	~SlitCam(void);
	//void CreateParamWindow();
	
	virtual void ProcessFrame(const IplImage * img);

protected:
	IplImage * m_inputCopy;
	virtual ModuleParameterStructure& GetRefParameter(){ return m_param;};
};

#endif

