#include "fuxlastix.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    fuxlastix w;
    w.show();
    return a.exec();
}
