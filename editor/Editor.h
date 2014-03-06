
#ifndef EDITOR_H
#define EDITOR_H

#include <QUrl>
#include <QWidget>
#include <QWebView>
#include <QMainWindow>

class Editor : public QMainWindow
{
	Q_OBJECT

	private:
		QWebView m_view;
		QString m_currentProject;
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
		Editor(QWidget *parent = 0);
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
