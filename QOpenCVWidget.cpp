
#include "QOpenCVWidget.h"
#include <cstdio>

#include <QComboBox>
#include <QGroupBox>
#include <QGridLayout>

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
	
	comboModules 		= new QComboBox();
	comboOutputStreams 	= new QComboBox();
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
	
	QGroupBox *gbSettings = new QGroupBox(tr("Display options"));
	gbSettings->setFlat(true);
	QGridLayout * vbox = new QGridLayout;
	//vbox->addStretch(1);
	
	QLabel* lab1 = new QLabel(tr("Module"));
	vbox->addWidget(lab1,0,0);
	comboModules->clear();
	for(std::list<Module*>::const_iterator it = x_manager->GetModuleList().begin(); it != x_manager->GetModuleList().end(); it++)
		comboModules->addItem(QString((*it)->GetName().c_str()), 1);
	vbox->addWidget(comboModules,0,1);
	
	QLabel* lab2 = new QLabel(tr("Out stream"));
	vbox->addWidget(lab2,1,0);
	this->updateModule(*x_manager->GetModuleList().begin());
	vbox->addWidget(comboOutputStreams,1,1);
	
	gbSettings->setLayout(vbox);
	layout->addWidget(gbSettings);
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

void QOpenCVWidget::updateModule(const Module * x_module)
{
	m_currentModule = x_module;
	comboOutputStreams->clear();
	for(std::list<OutputStream>::const_iterator it = m_currentModule->GetOutputStreamList().begin(); it != m_currentModule->GetOutputStreamList().end(); it++)
			comboOutputStreams->addItem(QString(it->GetName().c_str()), 1);

	updateOutputStream(&*(m_currentModule->GetOutputStreamList().begin()));
}

void QOpenCVWidget::updateOutputStream(const OutputStream * x_outputStream)
{
	m_currentOutputStream = x_outputStream;
}

