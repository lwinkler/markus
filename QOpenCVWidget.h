
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
private:
	//QLabel *imagelabel;
	QVBoxLayout *layout;
	
	QImage m_image;
	//QPainter m_painter;
	const Module * 		m_currentModule;
	const OutputStream * 	m_currentOutputStream;
	const Manager* 		m_manager;
	
	int m_outputWidth;
	int m_outputHeight;
	
	//QSize size;
	//QIcon icon;
	void paintEvent(QPaintEvent *event);
public:
	QOpenCVWidget(const Manager * x_manager, QWidget *parent = 0);
	~QOpenCVWidget(void);
	//void putImage();
	
	QComboBox * comboModules;
	QComboBox * comboOutputStreams;
	//virtual void resizeEvent(QResizeEvent * e);

public slots:
	//void Resize(int x_width, int x_height);
	void updateModule(const Module * x_module);
	void updateOutputStream(const OutputStream * x_outputStream);
	void updateModule(int x_index);
	void updateModule();
	void updateOutputStream(int x_index);
}; 

#endif
