// +--------------------------------------------------------------------------+
// | File      : Detector.h                                                   |
// | Utility   : Background extraction/pre-processing                         |
// | Author    : Laurent Winkler                                              |
// | Date      : July 2010                                                    |
// | Remarque  : none                                                         |
// +--------------------------------------------------------------------------+
#ifndef DETECTOR_H
#define DETECTOR_H

#include <cv.h>

/*! \class Detector
 *  \brief Class containing methods/attributes to detect moving objects in a scene
 *
 * Main parts are background modelization and foreground extraction
 */

class Detector
{
private:
	// Background subtraction	
	IplImage* m_foreground;
	IplImage* m_foreground_rff;
	IplImage* m_background;
	// Temporal differencing
	IplImage* m_lastImg;
	IplImage* m_temporalDiff;
	IplImage* m_blobsImg;


	static float m_background_alpha;
	static float m_foreground_thres;

	static int m_background_alpha_d;
	static int m_foreground_thres_d;
	
	static int m_inputBlurSize;
	static int m_foregroundFilterSize;
	
	bool m_emptyBackgroundSubtraction;
	bool m_emptyTemporalDiff;
	
public:
	Detector(int width, int height, int depth, int channels);
	~Detector(void);

	void CreateParamWindow();
	
	void Reset();
	void BlurInput(IplImage* src, IplImage* dst);
	void UpdateBackground(IplImage* img);
	void UpdateBackgroundMask(IplImage* img, IplImage* x_mask);
	void ExtractForeground(IplImage* img);
	void ExtractForegroundMax(IplImage* img);
	void RemoveFalseForegroundNeigh();
	void RemoveFalseForegroundMorph();

	void TemporalDiff(IplImage* img);
	
	void ExtractBlobs();

	inline IplImage* GetForeground(){ return m_foreground;};
	inline IplImage* GetForegroundRff(){ return m_foreground_rff;};
	inline IplImage* GetBackground(){ return m_background;};
	inline IplImage* GetTemporalDiff(){ return m_temporalDiff;};



	inline static float GetInputBlurSize(){ return m_inputBlurSize;};
	inline static void SetInputBlurSize(int x)
	{ 
		m_inputBlurSize = x; 
	};
	
	inline static float GetForegroundThres(){ return m_foreground_thres;};
	inline static void SetForegroundThres(float x)
	{ 
		m_foreground_thres= x; 
		m_foreground_thres_d = m_foreground_thres * m_foreground_thres_r;
	};
	
	inline static float GetBackgroundAlpha(){ return m_background_alpha;};
	inline static void SetBackgroundAlpha(float x)
	{ 
		m_background_alpha = x; 
		m_background_alpha_d = m_background_alpha * m_background_alpha_r;
	};
	
	inline static float GetForegroundFilterSize(){ return m_foregroundFilterSize;};
	inline static void SetForegroundFilterSize(int x)
	{ 
		m_foregroundFilterSize = x; 
	};
	
	
	static const float m_background_alpha_r;
	static const float m_foreground_thres_r;

};

#endif

