#ifndef GRIDCALC_H
#define GRIDCALC_H

#include <QDialog>
#include "fuxlastix.h"
#include <QList>
#include "tool_functions.h"
class fuxlastix;
namespace Ui {
class gridcalc;
}

class gridcalc : public QDialog
{
    Q_OBJECT

public:
     gridcalc(fuxlastix*);
    ~gridcalc();

private slots:
    void run_transformix(const QString&);
    void run_transformixP(const QString&);
    void run_transformixP2(const QString& input);

    void transformixDone();
    void transformixDone2();
    void on_closeButton_clicked();
    void on_pointButton_clicked();
 //   void on_readPButton_clicked();
 //   void on_preTriangleButton_clicked();
 //   void on_postTriangleButton_clicked();
    void on_transformButton_clicked();
    void on_jacobianButton_clicked();
    QList<QColor> createColorCircle();
    void on_TrafoPicButton_clicked();
    void on_pshowColCir_clicked();

private:
    Ui::gridcalc *ui;
    fuxlastix* pRef= nullptr;
    QProcess *runTransformix = nullptr;
    QProcess *runTransformix2 = nullptr;
    void saveMHDall (const QImage&, const QString&);
    QImage loadMHD(QString);
    QImage loadJacobianMHD(QString);
    mhdFormats type;
    dataType dt;
    QString temp="";
    void loadSettings();

    QList<int>getWH(QString name);
    void point_list(QImage);
    QImage read_points(QImage);
    void triangleCalc(QString);
    QImage DrawPoints(QImage Input);
    QImage DrawGrid(QImage Input);
    QImage MergeGrid(QImage Img, QImage Grid);
    void DispPixie(QImage Input);
    QImage makeBinary(QImage Input);
    void CreateAllPoints(QImage);
    QList<QList<QPointF>> CreateDeformationList();
    QImage drawColorWheel(QList<QColor>);
    void DispColorWheel(QImage Input);
    float getmaxmagnitude(QList<QPointF>);
    inline QColor getRGBA(QPointF, QList<QColor>, float);
    QImage drawColorWheelRound(QList<QColor>);


};

#endif // GRIDCALC_H
