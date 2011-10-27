
#include "QOpenCVWidget.h"
#include <cstdio>

#include <QComboBox>
#include <QGroupBox>
#include <QGridLayout>
#include <qevent.h>

#include <QPixmap>
#include <QPainter>

#include "Manager.h"
#include "Module.h"
#include "util.h"

// Constructor
QOpenCVWidget::QOpenCVWidget(const Manager* x_manager, QWidget *parent) : QWidget(parent)
{
	//Resize(320, 240);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_outputWidth  = QOpenCVWidget::width();
	m_outputHeight = QOpenCVWidget::height()/2;
	

	// Handlers for modules
	assert(x_manager != NULL);
	assert(x_manager->GetModuleList().size() > 0);
	m_manager 		= x_manager;
	m_currentModule 	= *x_manager->GetModuleList().begin();
	m_currentOutputStream 	= *m_currentModule->GetOutputStreamList().begin();
		
	comboModules 		= new QComboBox();
	comboOutputStreams 	= new QComboBox();
	layout = new QVBoxLayout;
	//QPainter painter(this);
	//imagelabel = new QLabel;
	//imagelabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	//QImage dummy(m_outputWidth, m_outputHeight, QImage::Format_RGB32);
	//m_image = dummy;
	
	//layout->addWidget(imagelabel);
	
	/*for (int x = 0; x < m_outputWidth; x ++) 
	{
		for (int y =0; y < m_outputHeight; y++) 
		{
			image.setPixel(x,y,qRgb(x, y, y));
		}
	}
	
	imagelabel->setPixmap(QPixmap::fromImage(image));
	*/
	QGroupBox *gbSettings = new QGroupBox(tr("Display options"));
	gbSettings->setFlat(true);
	QGridLayout * vbox = new QGridLayout;
	//vbox->addStretch(1);
	
	//vbox->addWidget(m_painter);
	
	QLabel* lab1 = new QLabel(tr("Module"));
	vbox->addWidget(lab1,0,0);
	comboModules->clear();
	int ind = 0;
	for(std::vector<Module*>::const_iterator it = x_manager->GetModuleList().begin(); it != x_manager->GetModuleList().end(); it++)
		comboModules->addItem(QString((*it)->GetName().c_str()), ind++);
	vbox->addWidget(comboModules,0,1);
	
	QLabel* lab2 = new QLabel(tr("Out stream"));
	vbox->addWidget(lab2,1,0);
	this->updateModule(*x_manager->GetModuleList().begin());
	vbox->addWidget(comboOutputStreams,1,1);
	
	gbSettings->setLayout(vbox);
	layout->addWidget(gbSettings);
	setLayout(layout);
	int index = 0;
	
	setPalette(QPalette(QColor(0, 0, 40)));
	setAutoFillBackground(true);

	//update();
	//m_image = QImage(m_outputWidth, m_outputHeight, QImage::Format_RGB32);
	
	//imagelabel->
	//m_painter.setBackground(QPixmap::fromImage(m_image));
	
	connect(comboModules, SIGNAL(activated(int)), this, SLOT(updateModule(int) ));
	connect(comboOutputStreams, SIGNAL(activated(int)), this, SLOT(updateOutputStream(int)));
}

QOpenCVWidget::~QOpenCVWidget(void) 
{
}

/*void QOpenCVWidget::Resize(int x_width, int x_height)
{
	QResizeEvent* e = new QResizeEvent(QSize(x_width, x_height), size());
	resizeEvent(e);
	delete(e);
}*/

/*void QOpenCVWidget::resizeEvent(QResizeEvent * e)
{
	m_outputWidth  = width();//e->size().width() * 4;
	m_outputHeight = height() / 2; //e->size().height() * 2;
	//QImage dummy(m_outputWidth, m_outputHeight, QImage::Format_RGB32);
	//image = dummy;
	m_outputWidth = imagelabel->width();
	m_outputHeight = imagelabel->height();
	
	m_image =  QImage(m_outputWidth, m_outputHeight, QImage::Format_RGB32);

}*/

/*void QOpenCVWidget::putImage() 
{
	//paintEvent(NULL);
	update();
}*/
void QOpenCVWidget::paintEvent(QPaintEvent * e) 
{
	int cvIndex, cvLineStart;
	const IplImage * cvimage = m_currentOutputStream->GetImageRef();
	
	//m_outputWidth  = QOpenCVWidget::width();
	//m_outputHeight = QOpenCVWidget::height()/2;
	m_outputWidth  = width();
	m_outputHeight = height();
	
	m_image =  QImage(m_outputWidth, m_outputHeight, QImage::Format_RGB32);
	//imagelabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	//m_outputWidth = this->width();
	//m_outputHeight = (this->width() * cvimage->height) /  cvimage->width;
	
	//delete mp_image;
	//mp_image = new QImage(m_outputWidth, m_outputHeight, QImage::Format_RGB32);
	
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
					if ( (cvoutput->width != m_image.width()) || (cvoutput->height != m_image.height()) ) {
						QImage temp(cvoutput->width, cvoutput->height, QImage::Format_RGB32);
						m_image = temp;
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
							
							m_image.setPixel(x,y,qRgb(red, green, blue));
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
	//imagelabel->
	//m_painter.setBackground(QPixmap::fromImage(m_image));
	//m_painter.setBrush();
	
	QPainter painter(this);
	painter.drawImage(QRect(0, 0, m_outputWidth, m_outputHeight), m_image);//,QRect(0, m_outputWidth, 0, m_outputHeight), Qt::ImageConversionFlag0);
	
}

void QOpenCVWidget::updateModule(const Module * x_module)
{
	m_currentModule = x_module;
	comboOutputStreams->clear();
	for(std::vector<OutputStream*>::const_iterator it = m_currentModule->GetOutputStreamList().begin(); it != m_currentModule->GetOutputStreamList().end(); it++)
			comboOutputStreams->addItem(QString((*it)->GetName().c_str()), 1);

	updateOutputStream(*(m_currentModule->GetOutputStreamList().begin()));
}

void QOpenCVWidget::updateOutputStream(const OutputStream * x_outputStream)
{
	m_currentOutputStream = x_outputStream;
}

void QOpenCVWidget::updateModule(int x_index)
{
	std::vector<Module*>::const_iterator it = m_manager->GetModuleList().begin();
	
	while(x_index-- > 0 && it != m_manager->GetModuleList().end())
		it++;
	
	updateModule(*it);
}

void QOpenCVWidget::updateOutputStream(int x_index)
{
	std::vector<OutputStream*>::const_iterator it = m_currentModule->GetOutputStreamList().begin();
	
	while(x_index-- > 0 && it != m_currentModule->GetOutputStreamList().end())
		it++;
	
	updateOutputStream((*it));
}



#include "QOpenCVWidget.moc"