#include "fuxlastix.h"

#include <QApplication>
#include <QStyle>
#include <QStyleFactory>


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("fusion"));
    fuxlastix w;
    QPalette pal=w.palette();
    pal.setColor(QPalette::Highlight,Qt::red);
    a.setPalette(pal);
    w.show();
    return a.exec();

}
