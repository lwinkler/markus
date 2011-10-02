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


class SlitCamParameter : public ParameterStructure
{
	
public:
	SlitCamParameter()
	{
		m_list.push_back(Parameter(0, "aperture", 1, PARAM_INT, 1, 10, &aperture));
	};
	void Init()
	{
	}
	
	int aperture;
};

class SlitCam : public Module
{
private:
	SlitCamParameter m_param;
	//IplImage * m_image;
	int m_position;
	static const char * m_name;
public:
	SlitCam(ConfigReader& x_configReader, int width, int height, int depth, int channels);
	~SlitCam(void);
	//void CreateParamWindow();
	virtual const char* GetName() {return m_name;}
	
	virtual void Init();
	virtual void ProcessFrame(const IplImage * img);
	
	inline static float GetTimeInterval(){ return m_time_interval;};
	inline static void SetTimeInterval(int x)
	{ 
		m_time_interval = x; 
	};
	
	
	static float m_time_interval;
protected:
	virtual ParameterStructure& GetRefParameter(){ return m_param;};
};

#endif

