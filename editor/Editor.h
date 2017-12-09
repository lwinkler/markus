
#ifndef EDITOR_H
#define EDITOR_H

#include <QUrl>
#include <QWidget>
#include <QWebView>
#include <QMainWindow>

#include <QObject>
#include <iostream>

namespace mk {
class Manager;

/*
class QManager : public QObject {
	Q_OBJECT
public:
	QManager(Manager& xr_manager);
	Q_INVOKABLE void CreateModule(QString x_jsonString);
private:
	Manager& mr_manager;
};
*/


class Editor : public QMainWindow
{
	Q_OBJECT

private:
	QWebView m_view;
	QString m_currentProject;
	std::string m_projectToLoad;
	QAction *aboutAct = nullptr;
	QAction *loadProjectAct = nullptr;
	QAction *saveProjectAct = nullptr;
	QAction *saveProjectAsAct = nullptr;
	QAction *updateModulesAct = nullptr;

	QMenu *fileMenu = nullptr;
	QMenu *viewMenu = nullptr;
	QMenu *helpMenu = nullptr;

	void CreateActions();
	void CreateMenus();

public:
	Editor(Manager* xp_manager = nullptr, const std::string& x_project = "", QWidget *parent = 0);
	~Editor();
	void setUrl(const QUrl &url);

public slots:
	void adaptDom(bool x_loadOk);
	void loadProject();
	bool save();
	bool saveAs();
	bool saveProject(const QString& x_fileName);
	void updateModules();
	void about();
	bool maybeSave();
};

}
#endif
