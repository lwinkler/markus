#include <QtGui/QApplication>
#include "markus.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    markus foo;
    foo.show();
    return app.exec();
}
