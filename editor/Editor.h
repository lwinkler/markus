
#ifndef EDITOR_H
#define EDITOR_H

#include <QUrl>
#include <QWidget>
#include <QWebView>
// #include "ui_window.h"

class Editor : public QWidget//, private Ui::Window
{
	Q_OBJECT

	private:
		QWebView m_view;
	public:
		Editor(QWidget *parent = 0);
		void setUrl(const QUrl &url);

	public slots:
		void AdaptDom(bool x_loadOk);
		void LoadProject(QString x_file);
		void SaveProject(QString x_file);
};

#endif
