#include <QtGui/QApplication>
#include "markus.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    markus gui;
    gui.show();
    return app.exec();
}
