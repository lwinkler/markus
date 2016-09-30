
#ifndef EDITOR_H
#define EDITOR_H

#include <QUrl>
#include <QWidget>
#include <QWebView>
#include <QMainWindow>

#include <QObject>
#include <iostream>

class Manager;

class QManager : public QObject {
	Q_OBJECT
public:
	QManager(Manager& xr_manager);
	Q_INVOKABLE void CreateModule(QString x_xmlString);
private:
	Manager& mr_manager;
};


class Editor : public QMainWindow
{
	Q_OBJECT

private:
	QWebView m_view;
	QString m_currentProject;
	std::string m_projectToLoad;
	QAction *aboutAct;
	QAction *loadProjectAct;
	QAction *saveProjectAct;
	QAction *saveProjectAsAct;
	QAction *updateProjectsAct;
	QAction *updateModulesAct;

	QMenu *fileMenu;
	QMenu *viewMenu;
	QMenu *helpMenu;

	void CreateActions();
	void CreateMenus();

public:
	Editor(Manager* xp_manager = nullptr, const std::string& x_project = "", QWidget *parent = 0);
	void setUrl(const QUrl &url);

public slots:
	void adaptDom(bool x_loadOk);
	void loadProject();
	bool save();
	bool saveAs();
	bool saveProject(const QString& x_fileName);
	void updateProjects();
	void updateModules();
	void about();
	bool maybeSave();
};

#endif
