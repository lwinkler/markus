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

class DetectorParameter
{
public:
	DetectorParameter()
	{
		background_alpha = 0.02;
		foreground_thres = 0.2;
		inputBlurSize = 3;
		foregroundFilterSize = 3;
	};
	float background_alpha;
	float foreground_thres;
	int inputBlurSize;
	int foregroundFilterSize;
	bool emptyBackgroundSubtraction;
	bool emptyTemporalDiff;
};

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
	bool m_emptyBackgroundSubtraction;
	bool m_emptyTemporalDiff;

	const DetectorParameter& m_param;
	
public:
	Detector(const DetectorParameter& param, int width, int height, int depth, int channels);
	~Detector(void);

	void Reset();
	void BlurInput(const IplImage* src, IplImage* dst);
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
};

#endif

