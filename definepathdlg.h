#ifndef DEFINEPATHDLG_H
#define DEFINEPATHDLG_H

#include <QDialog>

namespace Ui {
class DefinePathDlg;
}

class DefinePathDlg : public QDialog
{
    Q_OBJECT

public:
    explicit DefinePathDlg(QWidget *parent = nullptr);
    ~DefinePathDlg();

    QString transformixPath();
    QString elastixPath();

protected slots:
    void browseElastixPath();
    void browseTransformixPath();

private:
    Ui::DefinePathDlg *ui;
};

#endif // DEFINEPATHDLG_H
