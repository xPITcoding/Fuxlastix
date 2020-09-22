#ifndef COLORCHECK_H
#define COLORCHECK_H

#include <QDialog>
#include "fuxlastix.h"

enum ddoptions
{
    COLOR = 0,
    AVG =   1,
    RED =   2,
    GREEN = 3,
    BLUE =  4
};

enum mhdFormats
{
    MET_UCHAR = 0,
    MET_USHORT= 1,
    MET_FLOAT = 2,
    MET_UCHAR_ARRAY =3
};

namespace Ui {
class colorCheck;
}

class colorCheck : public QDialog
{
    Q_OBJECT

public:
    explicit colorCheck(QWidget *parent = nullptr);
    ~colorCheck();

    fuxlastix fux;

private slots:
    void on_pushButton_clicked();

private:
    Ui::colorCheck *ui;
    ddoptions fixlist;
    void loadSettings();
    QString temp="";
    QImage loadMHD_RGB888(QString name);
    void saveMHDcc(const QImage& , const QString&);
    dataType dt;
    mhdFormats mhdF;
    int movFormat =-1;
    int fixFormat =-1;
    QImage loadMHD (QString name);
    bool fouMelActive=false;
};

#endif // COLORCHECK_H
