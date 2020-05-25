#ifndef FUXLASTIX_H
#define FUXLASTIX_H

#include <QDialog>
#include <QProcess>
#include <QDir>


QT_BEGIN_NAMESPACE
namespace Ui { class fuxlastix; }
QT_END_NAMESPACE


enum dataType
{
    bit32,
    bit16,
    RGB24,
    bit8


};


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
private slots:
    void on_pickFixedImageButton_clicked();

    void on_pickMovingImageButton_clicked();

    void on_pickMovingColourButton_clicked();

    void on_pickDirectoryButton_clicked();

    void on_pickParameterFileButton_clicked();

    void on_pushQuitButton_clicked();

    void on_pushRunButton_clicked();

    void outputSlot();

    void fertigSlot();

    void ganzfertigSlot();

    QImage loadTIFFImage(const QString& fname);

    QImage scale(QImage);

    QList<QImage> split(QImage);

    void saveMHD(const QImage&, const QString&);

    void change_nr(const QString&);

   void run_transformix(const QString&);

   QImage merge(QString&);

   void saveMHDscaled(const QImage&, const QString&);

   void saveMHDall (const QImage&, const QString&);

   void copy_remove();

   QImage makeCheck(QImage);

   void on_pickTempButton_clicked();

   void on_checkerButton_clicked();

   void on_saveCheckerButton_clicked();

signals:
    void MSG(const QString&);

private:
    Ui::fuxlastix *ui;
    QProcess *runElastix =  nullptr;
    QProcess *runTransformix = nullptr;
    dataType dt;
    QString temp = "";
    int transformixCount = 0;
    QImage check2;
    QImage check1;
    QImage result2;
};
#endif // FUXLASTIX_H

