#ifndef FUXLASTIX_H
#define FUXLASTIX_H

#include <QDialog>
#include <QProcess>
#include <QDir>
#include "foumel.h"
#include "previewdialog.h"
#include "gridcalc.h"
#include "tool_functions.h"

QT_BEGIN_NAMESPACE
namespace Ui { class fuxlastix; }
QT_END_NAMESPACE



class fuxlastix : public QDialog
{
    Q_OBJECT

public:
    fuxlastix(QWidget *parent = nullptr);
    ~fuxlastix();
    void run_elastix();
    void saveSettings();
    void loadSettings();
    void rename(QString);
    fouMel callFM;
    friend class previewDialog;
    friend class gridcalc;




public slots:
    QList<QImage> split(QImage);

    void saveMHDall (const QImage&, const QString&,const float& rx=0.2f, const float& ry=0.2f, dataType localDt=INVALID);




private slots:
    void on_pickFixedImageButton_clicked();
    void on_pickMovingImageButton_clicked();
    void on_pickDirectoryButton_clicked();
    void on_pickParameterFileButton_clicked();
    void on_pushQuitButton_clicked();
    void on_pushRunButton_clicked();
    void outputSlot();
    void fertigSlot();
    void ganzfertigSlot();
    //QImage loadTIFFImage(const QString& fname);
    QImage scale(QImage);
    void change_nr(const QString&);
    void run_transformix(const QString&);
    QImage merge(QString&);
    void copy_remove();
    void on_pickTempButton_clicked();
    void on_checkerButton_clicked();
    void on_saveCheckerButton_clicked();
    void on_gridButton_clicked();
    QImage loadMHD (QString);
    QImage loadAllImages(QString fname); 
    void callElastix();

signals:
    void MSG(const QString&);

private:
    Ui::fuxlastix *ui;
    QProcess *runElastix =  nullptr;
    QProcess *runTransformix = nullptr;
    dataType dt;
    QString temp = "";
    QString resPath = "";
    bool usemask = false;
    bool fouriermellin = false;

   // previewDialog* previewDlgWindow = nullptr;
    int transformixCount = 0;
    QImage result2;
    bool fouMelactive=false;
    void dispPixie(QImage, int);
   // QImage MASK;
    QImage ScaledFixed800;
    QImage ScaledMoving800;

    QString elastixpath, transformixpath;

public:
    int fixFormat=-1;
    int movFormat=-1;

};
#endif // FUXLASTIX_H

