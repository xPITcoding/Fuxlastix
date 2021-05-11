#ifndef PREVIEWDIALOG_H
#define PREVIEWDIALOG_H
#include "fuxlastix.h"
#include <QDialog>
class fuxlastix;
namespace Ui {
class previewDialog;
}

class previewDialog : public QDialog
{
    Q_OBJECT

public:
     previewDialog(fuxlastix*);
    ~previewDialog();


private slots:
    void createMask(QImage,int);
    QImage regionGrowing(QImage, int, long);
    void dispPixie(QImage, int);
    void on_pushButton_Preview_clicked();
    void on_pushButton_Exit_clicked();
    void startElastix();
    void on_cb_fixedmask_stateChanged(int arg1);
    void on_pushButton_PreviewFM_clicked();
    void on_cb_FM_stateChanged(int arg1);
    void on_cb_regiongrowing_fixed_stateChanged(int arg1);

    void on_cb_regiongrowing_moving_stateChanged(int arg1);

    void on_cb_rigiditypenalty_stateChanged(int arg1);

private:
    Ui::previewDialog *ui;
    fuxlastix* pRef= nullptr;
    fouMel* pFouMel = nullptr;
    inline QVector3D matmul(const QMatrix2x2& m,const QVector3D& p);
    void doFourierMellin(QImage, QImage, int);
    QImage rotateImage(QImage, float);
    QImage translateImage(QImage, long, long, float);
    void on_FMButton_clicked();
    int fixFormat = 0;
    int movFormat = 0;
    void prepareImagesFM(QImage&, QImage&);
    void OverlayImages();
};

#endif // PREVIEWDIALOG_H
