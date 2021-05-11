#include "definepathdlg.h"
#include "ui_definepathdlg.h"

#include <QFileDialog>

DefinePathDlg::DefinePathDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DefinePathDlg)
{
    ui->setupUi(this);
    connect(ui->pBrowseElastixPathTB,SIGNAL(clicked()),this,SLOT(browseElastixPath()));
    connect(ui->pBrowseTransformixPathTB,SIGNAL(clicked()),this,SLOT(browseTransformixPath()));
}

DefinePathDlg::~DefinePathDlg()
{
    delete ui;
}

QString DefinePathDlg::transformixPath()
{
    return ui->pTransformixPathLEdit->text();
}
QString DefinePathDlg::elastixPath()
{
    return ui->pElastixPathLEdit->text();
}

void DefinePathDlg::browseElastixPath()
{
    QString path=QFileDialog::getOpenFileName(0,"pick elastix");

    if (!path.isEmpty())
        ui->pElastixPathLEdit->setText(path);
}
void DefinePathDlg::browseTransformixPath()
{
    QString path=QFileDialog::getOpenFileName(0,"pick transformix");

    if (!path.isEmpty())
        ui->pTransformixPathLEdit->setText(path);
}
