
#include "QOpenCVWidget.h"
#include "cstdio"

#include "Manager.h"
#include "Module.h"
#include "util.h"

// Constructor
QOpenCVWidget::QOpenCVWidget(const Manager* x_manager, QWidget *parent) : QWidget(parent) 
{
	// Handlers for modules
	assert(x_manager != NULL);
	m_manager 		= x_manager;
	m_currentModule 	= *x_manager->GetModuleList().begin();
	m_currentOutputStream 	= &(*m_currentModule->GetOutputStreamList().begin());

	m_outputWidth  = 640;
	m_outputHeight = 480;

	layout = new QVBoxLayout;
	imagelabel = new QLabel;
	QImage dummy(m_outputWidth, m_outputHeight, QImage::Format_RGB32);
	image = dummy;
	layout->addWidget(imagelabel);

	for (int x = 0; x < m_outputWidth; x ++) 
	{
		for (int y =0; y < m_outputHeight; y++) 
		{
			image.setPixel(x,y,qRgb(x, y, y));
		}
	}
	
	imagelabel->setPixmap(QPixmap::fromImage(image));
	setLayout(layout);
}

QOpenCVWidget::~QOpenCVWidget(void) {
    
}

void QOpenCVWidget::putImage() 
{
    int cvIndex, cvLineStart;
    const IplImage * cvimage = m_currentOutputStream->GetImageRef();
    
    // Write output to screen
	static IplImage *cvoutput = cvCreateImage( cvSize(m_outputWidth, m_outputHeight), cvimage->depth, cvimage->nChannels);
	static IplImage* tmp1_c1 = NULL;
	static IplImage* tmp2_c1 = NULL;
	static IplImage* tmp1_c3 = NULL;
	static IplImage* tmp2_c3 = NULL;
	
	if(cvimage->nChannels == 3)
		adjust(cvimage, cvoutput, tmp1_c3, tmp2_c3);
	else
		adjust(cvimage, cvoutput, tmp1_c1, tmp2_c1);
	
    // switch between bit depths
    switch (cvoutput->depth) {
        case IPL_DEPTH_8U:
            switch (cvoutput->nChannels) {
                case 3:
                    if ( (cvoutput->width != image.width()) || (cvoutput->height != image.height()) ) {
                        QImage temp(cvoutput->width, cvoutput->height, QImage::Format_RGB32);
                        image = temp;
                    }
                    cvIndex = 0; cvLineStart = 0;
                    for (int y = 0; y < cvoutput->height; y++) {
                        unsigned char red,green,blue;
                        cvIndex = cvLineStart;
                        for (int x = 0; x < cvoutput->width; x++) {
                            // DO it
                            red = cvoutput->imageData[cvIndex+2];
                            green = cvoutput->imageData[cvIndex+1];
                            blue = cvoutput->imageData[cvIndex+0];
                            
                            image.setPixel(x,y,qRgb(red, green, blue));
                            cvIndex += 3;
                        }
                        cvLineStart += cvoutput->widthStep;                        
                    }
                    break;
                default:
                    printf("This number of channels is not supported\n");
                    break;
            }
            break;
        default:
            printf("This type of IplImage is not implemented in QOpenCVWidget\n");
            break;
    }
    imagelabel->setPixmap(QPixmap::fromImage(image));    
}

