#ifndef markus_H
#define markus_H

#include <QtGui/QMainWindow>
#include "QOpenCVWidget.h"

class Manager;
class ConfigReader;

class markus : public QMainWindow
{
Q_OBJECT
public:
	markus(ConfigReader& rx_configReader, Manager& rx_manager);
	virtual ~markus();
private:
	QOpenCVWidget *cvwidget;

	ConfigReader& m_configReader;
	Manager& m_manager;
	void timerEvent(QTimerEvent*);        
};

#endif // markus_H
