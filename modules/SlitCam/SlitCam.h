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

/*! \class SlitCam
 *  \brief Class containing methods/attributes of a slit camera
 *
 */

class SlitCam : public Module
{
private:
	//IplImage * m_image;
	int m_position;
	static const char * m_name;
public:
	SlitCam(ConfigReader& x_configReader, int width, int height, int depth, int channels);
	~SlitCam(void);
	void CreateParamWindow();
	virtual const char* GetName() {return m_name;}
	
	void Init();
	void ProcessFrame(const IplImage * img);
	
	inline static float GetTimeInterval(){ return m_time_interval;};
	inline static void SetTimeInterval(int x)
	{ 
		m_time_interval = x; 
	};
	
	
	static float m_time_interval;
};

#endif

