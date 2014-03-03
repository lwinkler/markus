
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
		QAction *aboutAct;
		QAction *loadProjectAct;
		QAction *saveProjectAct;
		QAction *saveProjectAsAct;

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
		void about();
		bool maybeSave();
};

#endif
