#include "OutputStream.h"


OutputStream::OutputStream(const std::string& x_name, StreamType x_type, IplImage* x_image) :
	m_name(x_name),
	m_type(x_type),
	m_image(x_image)
{
}

OutputStream::~OutputStream()
{

}
