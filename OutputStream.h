#ifndef OUTPUT_STREAM_H
#define OUTPUT_STREAM_H

#include "cv.h"


enum StreamType
{
	STREAM_DEBUG,
	STREAM_OUTPUT
};


class OutputStream
{
public:
	OutputStream(const std::string& x_name, StreamType x_type, IplImage* x_image);
	~OutputStream();
	const std::string& GetName() const {return m_name;};
	const IplImage* GetImageRef() const {return m_image;};
protected:
	const std::string m_name;
	const IplImage * m_image;
	const StreamType m_type;
	
private:
	OutputStream& operator=(const OutputStream&);
	OutputStream(const OutputStream&);
};

#endif