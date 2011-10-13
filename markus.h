#ifndef markus_H
#define markus_H

#include <QtGui/QMainWindow>
#include "QOpenCVWidget.h"
#include <QScrollArea>

#include <vector>

class Manager;
class ConfigReader;

class markus : public QMainWindow
{
Q_OBJECT
public:
	markus(ConfigReader& rx_configReader, Manager& rx_manager);
	virtual ~markus();
private:
	//QOpenCVWidget cvwidget;

	ConfigReader& m_configReader;
	Manager& m_manager;
	void timerEvent(QTimerEvent*);
	
	int nbCols;
	int nbLines;
	
	void createActions();
	void createMenus();
	//void updateActions();
	void updateModuleViewers();
	
	QLabel *createLabel(const QString &text);
	
	std::vector<QOpenCVWidget *> m_moduleViewer;
	std::vector<QScrollArea   *> m_scroll;
	
	QAction *exitAct;
	QAction *view1x1Act;
	QAction *view1x2Act;
	QAction *view2x2Act;
	QAction *aboutAct;
	QAction *aboutQtAct;
	
	QMenu *fileMenu;
	QMenu *viewMenu;
	QMenu *helpMenu;
private slots:
	void about();
	void view1x1();
	void view1x2();
	void view2x2();
};

#endif // markus_H
