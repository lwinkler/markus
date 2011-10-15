#ifndef markus_H
#define markus_H

#include "QOpenCVWidget.h"

#include <QMainWindow>
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
	
	QWidget m_mainWidget;
	QGridLayout m_mainLayout;
	std::vector<QOpenCVWidget *> m_moduleViewer;
	std::vector<QScrollArea   *> m_scroll;
	//std::vector<QWidget   *> m_scroll;
	
	QAction *exitAct;
	QAction *view1x1Act;
	QAction *view1x2Act;
	QAction *view2x2Act;
	QAction *view2x3Act;
	QAction *aboutAct;
	QAction *aboutQtAct;
	
	QMenu *fileMenu;
	QMenu *viewMenu;
	QMenu *helpMenu;
private slots:
	void about();
	void view2x3();
	void view1x1();
	void view1x2();
	void view2x2();
};

#endif // markus_H
