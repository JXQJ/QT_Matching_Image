#include "matchimage.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    matchImage w;
    w.show();

    return a.exec();
}
