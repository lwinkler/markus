
#ifndef QOPENCVWIDGET_H
#define QOPENCVWIDGET_H

#include <opencv/cv.h>
#include <QPixmap>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QImage>
//#include <QIcon>
#include <QPainter>

class Manager;
class Module;
class OutputStream;
class QComboBox;
class QPainter;

class QOpenCVWidget : public QWidget 
{
	Q_OBJECT
public:
	QOpenCVWidget(const Manager * x_manager, QWidget *parent = 0);
	virtual ~QOpenCVWidget();
private:
	//QLabel *imagelabel;
	QVBoxLayout *layout;
	
	QImage m_image;
	const Module * 		m_currentModule;
	const OutputStream * 	m_currentOutputStream;
	const Manager* 		m_manager;
	
	int m_outputWidth;
	int m_outputHeight;
	
	//QSize size;
	//QIcon icon;
	void paintEvent(QPaintEvent *event);
	//void putImage();
	
	QComboBox * comboModules;
	QComboBox * comboOutputStreams;
	//virtual void resizeEvent(QResizeEvent * e);

	// Images for format conversion
	IplImage* m_img_tmp1_c1;
	IplImage* m_img_tmp2_c1;
	IplImage* m_img_tmp1_c3;
	IplImage* m_img_tmp2_c3;

public slots:
	//void Resize(int x_width, int x_height);
	void updateModule(const Module * x_module);
	void updateOutputStream(const OutputStream * x_outputStream);
	void updateModule(int x_index);
	void updateOutputStream(int x_index);
}; 

#endif
