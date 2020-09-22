#ifndef FOUMEL_H
#define FOUMEL_H

#include <QDialog>
#include <QMatrix3x3>
#include "alglib/src/linalg.h"
#include "alglib/src/fasttransforms.h"

using namespace alglib;


struct XYZ
{
public:
    long x,y,z;
    XYZ(long xx,long yy, long zz){x=xx;y=yy;z=zz;}
};

namespace Ui {
class fouMel;
}

class fouMel : public QDialog
{
    Q_OBJECT

public:
    explicit fouMel(QWidget *parent = nullptr);
    ~fouMel();

    QMatrix3x3 dyad (XYZ v1, XYZ v2);
    complex_2d_array* makeFFT(QImage);
    real_2d_array* invFFT(complex_2d_array* ,const long& ,const long&);
    QImage flipImg (QImage);
    QImage createImg (real_2d_array *, const long&, const long&, long&, long&, long&, long&);
    QImage polarImage(const QImage&,const int&,const int&);
    QImage Corr(QImage, QImage, long&, long&, long&, long&);

private:
    Ui::fouMel *ui;
};

#endif // FOUMEL_H
