#ifndef GRIDCALC_H
#define GRIDCALC_H

#include <QDialog>
#include "fuxlastix.h"
#include <QList>

class U16Image : public QImage
{
public:
    U16Image(const uchar*b,int w,int h):QImage(b,w,h,QImage::Format_Grayscale16){}
    U16Image(const float*b,int w,int h):QImage(w,h,QImage::Format_Grayscale16){
       for(long x=0; x<w; x++)
           for(long y=0; y<h; y++)
           {
               setPixel(x,y,b[x+y*w]);
           }
    }
    quint16 pixel(int x,int y)
    {
        return ((quint16*)scanLine(y))[x];
    }
    void setPixel(int x,int y,quint16 val)
    {
        ((quint16*)scanLine(y))[x]=val;
    }
};
class S16Image : public QImage
{
public:
    S16Image(const uchar*b,int w,int h):QImage(b,w,h,QImage::Format_Grayscale16){}
    qint16 pixel(int x,int y)
    {
        return ((qint16*)scanLine(y))[x];
    }
    void setPixel(int x,int y,qint16 val)
    {
        ((qint16*)scanLine(y))[x]=val;
    }
};

namespace Ui {
class gridcalc;
}

class gridcalc : public QDialog
{
    Q_OBJECT

public:
    explicit gridcalc(QWidget *parent = nullptr);
    ~gridcalc();

private slots:
    void run_transformix(const QString&);
    void run_transformixP(const QString&);
    void on_closeButton_clicked();
    void on_makeGridButton_clicked();
    void on_pointButton_clicked();



    void on_readPButton_clicked();

    void on_preTriangleButton_clicked();

    void on_postTriangleButton_clicked();

    void on_compAreaButton_clicked();
    
    void on_pushButton_clicked();

private:
    Ui::gridcalc *ui;
    QProcess *runTransformix = nullptr;
    void saveMHDall (const QImage&, const QString&);
    QImage loadMHD(QString);
    QImage loadMHD16(QString);
    dataType dt;
    QString temp="";
    void loadSettings();
    float* loadMHDraw32to32(QString name);
    ushort* loadMHDraw32to16(QString name);
    ushort* loadMHDraw16to16(QString name);
    void regionGrowing(U16Image bild, QString output);
    QList<int>getWH(QString name);
    void point_list();
    void read_points();
    void triangleCalc(QString);
};

#endif // GRIDCALC_H
