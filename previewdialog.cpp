#include "previewdialog.h"
#include "ui_previewdialog.h"
#include"tool_functions.h"
#include <QTextStream>
#include <QDataStream>
#include <QByteArray>
#include <QPixmap>
#include <QMessageBox>
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <iostream>
#include <QThread>
#include <QPainter>
#include "gridcalc.h"
#include "foumel.h"
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <QImageReader>
#include <QProcess>
#include <QVector3D>

using namespace std;
previewDialog::previewDialog(fuxlastix* ref) :
    QDialog(),
    ui(new Ui::previewDialog)
{
    ui->setupUi(this);
    connect(ui->pushButton_Run, SIGNAL(clicked()), this, SLOT(startElastix()));
    pRef = ref;
    if (pRef->ScaledFixed800.format() == QImage::Format_Grayscale16 || pRef->ScaledFixed800.format() == QImage::Format_Grayscale8)
        ui->combobox_color_fixedimage->setEnabled(false);
    if (pRef->ScaledMoving800.format() == QImage::Format_Grayscale16 || pRef->ScaledMoving800.format() == QImage::Format_Grayscale8)
        ui->combobox_color_movingimage->setEnabled(false);
    dispPixie(pRef->ScaledFixed800,0);
    dispPixie(pRef->ScaledMoving800,2);
    ui->pushButton_PreviewFM->setDisabled(true);
    ui->le_regiongrowing_fixed->setDisabled(true);
    ui->le_regiongrowing_moving->setDisabled(true);
    ui->lineEdit_fixedmask->setDisabled(true);
    ui->lineEdit_rigiditypenalty->setDisabled(true);

    pFouMel = new fouMel();
}

previewDialog::~previewDialog()
{
    delete ui;
}

void previewDialog::on_pushButton_Exit_clicked()
{
    reject();
}

void previewDialog::on_pushButton_Preview_clicked()
{
    //Getting Pictures from Fuxlatix
    QImage ScaledMovingPrev(pRef->ScaledMoving800);
    QImage ScaledFixedPrev(pRef->ScaledFixed800);

    //Automized Invert of Moving Image
    int dark=0, bright=0;
    for (int x=0;x<ScaledMovingPrev.width();x++)
    {
        if (qGray(ScaledMovingPrev.pixel(x,0))<125)
            dark++;
        else
            bright++;
    }
    if (bright>dark)
        ScaledMovingPrev.invertPixels();

    //Region Growing
    if (ui->cb_regiongrowing_moving->isChecked())
        ScaledMovingPrev = regionGrowing(ScaledMovingPrev,ui->le_regiongrowing_moving->text().toInt(),2000);

    if (ui->cb_regiongrowing_fixed->isChecked())
    {
        ScaledFixedPrev = regionGrowing(ScaledFixedPrev,ui->le_regiongrowing_fixed->text().toInt(),2000);

    }

    //Set Image Format
    if(ScaledFixedPrev.format() != QImage::Format_Grayscale16 || ScaledFixedPrev.format() != QImage::Format_Grayscale8)
    {
            QList <QImage> fixList = pRef->split(ScaledFixedPrev);
        switch (ui->combobox_color_fixedimage ->currentIndex())
        {
        case 0: //save color image;
            break;
        case 1: // save gv image;
            ScaledFixedPrev = ScaledFixedPrev.convertToFormat(QImage::Format_Grayscale16);
            break;
        case 2: //save red channel;
            ScaledFixedPrev = fixList[0].convertToFormat(QImage::Format_Grayscale16);
            break;
        case 3: //save green channel;
            ScaledFixedPrev = fixList[1].convertToFormat(QImage::Format_Grayscale16);
            break;
        case 4: //save blue channel ;
            ScaledFixedPrev = fixList[2].convertToFormat(QImage::Format_Grayscale16);
            break;
        }
    }

    if(ScaledMovingPrev.format() != QImage::Format_Grayscale16 || ScaledMovingPrev.format() != QImage::Format_Grayscale8)
    {
        QList <QImage> movList = pRef->split(ScaledMovingPrev);
        switch (ui->combobox_color_movingimage->currentIndex())
        {
        case 0: // save gv image;
            ScaledMovingPrev = ScaledMovingPrev.convertToFormat(QImage::Format_Grayscale16);
            break;
        case 1: //save red channel;
            ScaledMovingPrev = movList[0].convertToFormat(QImage::Format_Grayscale16);
            break;
        case 2: //save green channel;
            ScaledMovingPrev = movList[1].convertToFormat(QImage::Format_Grayscale16);
            break;
        case 3: //save blue channel ;
            ScaledMovingPrev = movList[2].convertToFormat(QImage::Format_Grayscale16);
            break;
        }
    }

    dispPixie(ScaledFixedPrev,0);
    dispPixie(ScaledMovingPrev,2);

    QPalette pal=palette();
    pal.setColor(QPalette::Active,QPalette::WindowText,Qt::white);
    ui->label_FixedImageLabel->setPalette(pal);
    ui->label_FixedImageLabel->setText("Fixed Image");
    ui->label_FixedMaskLabel->setPalette(pal);
    ui->label_FixedMaskLabel->setText("Fixed Mask");
    ui->label_MovingImageLabel->setPalette(pal);
    ui->label_MovingImageLabel->setText("Moving Image");
    ui->label_PenalyMaskLabel->setPalette(pal);
    ui->label_PenalyMaskLabel->setText("Penalty Mask");

    if(ui->cb_fixedmask->isChecked())
    {
         createMask(ScaledFixedPrev,0);
         QImage mask(pRef->temp+"fixedmaskpadded.png");
         dispPixie(mask,1);
    }

    if(ui->cb_rigiditypenalty->isChecked())
    {
         createMask(ScaledMovingPrev,1);
         QImage pmask(pRef->temp+"movingmaskpadded.png");
         dispPixie(pmask,3);
    }


}

void previewDialog::on_pushButton_PreviewFM_clicked()
{   
    prepareImagesFM(pRef->ScaledFixed800,pRef->ScaledMoving800);
    QImage i1(pRef->temp+"FixedFM.png");
    QImage i2(pRef->temp+"MovingFM.png");
    doFourierMellin(i1,i2,0);
    OverlayImages();

    QPalette pal=palette();

    pal.setColor(QPalette::Active,QPalette::WindowText,Qt::green);
    ui->label_FixedImageLabel->setPalette(pal);
    ui->label_FixedImageLabel->setText("Original Moving Image");

    pal.setColor(QPalette::Active,QPalette::WindowText,Qt::blue);
    ui->label_FixedMaskLabel->setPalette(pal);
    ui->label_FixedMaskLabel->setText("Moving Image After FM");

    pal.setColor(QPalette::Active,QPalette::WindowText,Qt::red);
    ui->label_MovingImageLabel->setPalette(pal);
    ui->label_MovingImageLabel->setText("Fixed Image");

    ui->label_PenalyMaskLabel->setText("Overlay");
}

void previewDialog::startElastix()
{

    //Getting Pictures from Fuxlatix
    QImage ScaledMovingPrev(pRef->ScaledMoving800);
    QImage ScaledFixedPrev(pRef->ScaledFixed800);

    //Automized Invert of Moving Image
    int dark=0, bright=0;
    for (int x=0;x<ScaledMovingPrev.width();x++)
    {
        if (qGray(ScaledMovingPrev.pixel(x,0))<125)
            dark++;
        else
            bright++;
    }
    if (bright>dark)
        ScaledMovingPrev.invertPixels();

    //Region Growing
    if (ui->cb_regiongrowing_moving->isChecked())
        ScaledMovingPrev = regionGrowing(ScaledMovingPrev,ui->le_regiongrowing_moving->text().toInt(),1000);

    if (ui->cb_regiongrowing_fixed->isChecked())
    {
        ScaledFixedPrev = regionGrowing(ScaledFixedPrev,ui->le_regiongrowing_fixed->text().toInt(),1000);

    }

    //Set Image Format
    if(ScaledFixedPrev.format() != QImage::Format_Grayscale16 || ScaledFixedPrev.format() != QImage::Format_Grayscale8)
    {
            QList <QImage> fixList = pRef->split(ScaledFixedPrev);
        switch (ui->combobox_color_fixedimage ->currentIndex())
        {
        case 0: //save color image;
            break;
        case 1: // save gv image;
            ScaledFixedPrev = ScaledFixedPrev.convertToFormat(QImage::Format_Grayscale16);
            break;
        case 2: //save red channel;
            ScaledFixedPrev = fixList[0].convertToFormat(QImage::Format_Grayscale16);
            break;
        case 3: //save green channel;
            ScaledFixedPrev = fixList[1].convertToFormat(QImage::Format_Grayscale16);
            break;
        case 4: //save blue channel ;
            ScaledFixedPrev = fixList[2].convertToFormat(QImage::Format_Grayscale16);
            break;
        }
    }

    if(ScaledMovingPrev.format() != QImage::Format_Grayscale16 || ScaledMovingPrev.format() != QImage::Format_Grayscale8)
    {
        QList <QImage> movList = pRef->split(ScaledMovingPrev);
        switch (ui->combobox_color_movingimage->currentIndex())
        {
        case 0: // save gv image;
            ScaledMovingPrev = ScaledMovingPrev.convertToFormat(QImage::Format_Grayscale16);
            break;
        case 1: //save red channel;
            ScaledMovingPrev = movList[0].convertToFormat(QImage::Format_Grayscale16);
            break;
        case 2: //save green channel;
            ScaledMovingPrev = movList[1].convertToFormat(QImage::Format_Grayscale16);
            break;
        case 3: //save blue channel ;
            ScaledMovingPrev = movList[2].convertToFormat(QImage::Format_Grayscale16);
            break;
        }
    }

    pRef->saveMHDall(ScaledFixedPrev,pRef->temp+"scaledFixed.raw", 1.0f, 1.0f);
    pRef->saveMHDall(ScaledMovingPrev,pRef->temp+"scaledMoving.raw", 1.0f, 1.0f);

    if(ui->cb_FM->isChecked())
    {
        prepareImagesFM(ScaledFixedPrev,ScaledMovingPrev);
        QImage i1(pRef->temp+"FixedFM.png");
        QImage i2(pRef->temp+"MovingFM.png");
        doFourierMellin(i1,i2,1);
        ScaledMovingPrev = pFouMel->movingPreKernFM;
    }
    else
        pRef->saveMHDall(ScaledMovingPrev,pRef->temp+"scaledMoving.raw", 1.0f, 1.0f);

    if(ui->cb_rigiditypenalty->isChecked())
    {
         createMask(ScaledMovingPrev,1);
    }
    if(ui->cb_fixedmask->isChecked())
    {
         createMask(ScaledFixedPrev,0);
         pRef->usemask=true;
    }

    pRef->saveMHDall(ScaledFixedPrev,pRef->temp+"scaledFixed.raw", 1.0f, 1.0f);


    accept();
}



void previewDialog::dispPixie(QImage input, int side)
{
  QPixmap pix;
  pix.convertFromImage(input);
  if (input.width()>input.height())
      pix=pix.scaledToWidth(350);
  else
      pix=pix.scaledToHeight(350);

  if (side == 0)
      ui->label_leftUp->setPixmap(pix);
  else if (side == 1)
      ui->label_rightUp->setPixmap(pix);
  else if (side == 2)
      ui->label_leftDown->setPixmap(pix);
  else
      ui->label_rightDown->setPixmap(pix);
}

void previewDialog::createMask(QImage grayImg, int pic)
{
    int threshhold;
    if (pic == 0)
        threshhold = ui->lineEdit_fixedmask->text().toInt();
    else
        threshhold = ui->lineEdit_rigiditypenalty->text().toInt();
    grayImg = grayImg.convertToFormat(QImage::Format_Grayscale8);
    QImage maskImg(grayImg.width(),grayImg.height(),QImage::Format_Grayscale8);
    maskImg.fill(qRgb(0,0,0));
    for (int x=0; x<grayImg.width();x++)
    {
        for (int y=0; y<grayImg.height();y++)
        {
            int val = qGray(grayImg.pixel(x,y));
            if (val>threshhold)
            {
                maskImg.setPixel(x,y,qRgb(255,255,255));
            }
        }
    }
   // dispPixie(maskImg,1);
    QImage maskImg2(grayImg.width()+2,grayImg.height()+2,QImage::Format_Grayscale8);
    QPainter pain(&maskImg2);
    pain.drawImage(QPoint(1,1),maskImg);
    pain.end();
    QImage maskFilled(maskImg2.width(),maskImg2.height(),QImage::Format_Grayscale8);
    maskFilled.fill(qRgb(0,0,0));
    QPainter mal(&maskFilled);
    mal.setPen(Qt::white);
    mal.setBrush(Qt::white);
    mal.drawRect(1,1,maskFilled.width()-2,maskFilled.height()-2);
    mal.end();
    int val;
    for (int count=0; count<3; count++)
    {
        for (int x=1; x<maskImg2.width();x++)
            for (int y=1; y<maskImg2.height();y++)
            {
                val = qGray(maskImg2.pixel(x,y));
                if (val==0 && maskFilled.pixel(x,y-1)==qRgb(0,0,0) && maskFilled.pixel(x,y)!=qRgb(0,0,0))
                {
                    maskFilled.setPixel(x,y,qRgb(0,0,0));
                }
            }
        //maskFilled.save("/home/xpit/Yannic/Bild1.png");
        for (int y=1; y<maskImg2.height();y++)
            for (int x=maskImg2.width()-2;x>0;x--)
            {
                val = qGray(maskImg2.pixel(x,y));
                if (val==0 && maskFilled.pixel(x+1,y)==qRgb(0,0,0) && maskFilled.pixel(x,y)!=qRgb(0,0,0))
                {
                    maskFilled.setPixel(x,y,qRgb(0,0,0));
                }
            }
        for (int x=maskImg2.width()-1; x>0;x--)
            for (int y=maskImg2.height()-2; y>0;y--)
            {
                val = qGray(maskImg2.pixel(x,y));
                if (val==0 && maskFilled.pixel(x,y+1)==qRgb(0,0,0) && maskFilled.pixel(x,y)!=qRgb(0,0,0))
                {
                    maskFilled.setPixel(x,y,qRgb(0,0,0));
                }
            }
        for (int y=1; y<maskImg2.height();y++)
            for (int x=1; x<maskImg2.width();x++)
            {
                val = qGray(maskImg2.pixel(x,y));
                if (val==0 && maskFilled.pixel(x-1,y)==qRgb(0,0,0) && maskFilled.pixel(x,y)!=qRgb(0,0,0))
                {
                    maskFilled.setPixel(x,y,qRgb(0,0,0));
                }
             }
    }
     maskFilled=maskFilled.copy(QRect(1,1,maskFilled.width()-2,maskFilled.height()-2));
     QImage copyMaskFilled(maskFilled);
     //ui->histolabel->setPixmap(QPixmap::fromImage(copyMaskFilled));
     int padding;
     if (pic == 0)
         padding = 5;
     else
         padding = 5;
     bool found=false;
     for(int y=0; y<maskFilled.height();y++)
         for(int x=0; x<maskFilled.width();x++)
             if (maskFilled.pixel(x,y)==qRgb(0,0,0))
             {
                found = false;
                for(int j=max(0,y-padding); j<min(maskFilled.height(),y+padding+1) && !found; j++)
                {
                    for(int i=max(0,x-padding); i<min(maskFilled.width(),x+padding+1) && !found;i++)
                    {
                       if (maskFilled.pixel(i,j)==qRgb(255,255,255))
                           found =true;
                    }
                }
                if(found)
                    copyMaskFilled.setPixel(x,y,qRgb(255,255,255));
             }
    if (pic == 0)
    {
     maskFilled.save(pRef->temp+"fixedmask.png");
     copyMaskFilled.save(pRef->temp+"fixedmaskpadded.png");
     pRef->saveMHDall(copyMaskFilled,pRef->temp + "fixedMask.raw",1.0f,1.0f,BIN);
    }
    else
    {
     maskFilled.save(pRef->temp+"movingmask.png");
     copyMaskFilled.save(pRef->temp+"movingmaskpadded.png");
     pRef->saveMHDall(copyMaskFilled,pRef->temp + "movingMask.raw",1.0f,1.0f,BIN);
    }

}

QImage previewDialog::regionGrowing(QImage rg, int THR, long areaTHR)
{
    //scaledown
    QImage original=rg;
    //rg=rg.scaled(rg.width()/5,rg.height()/5,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    rg=rg.convertToFormat(QImage::Format_Grayscale8);

    QImage result(rg.width(),rg.height(),QImage::Format_RGB888);
    result.fill(qRgb(255,255,255));
    QVector<QPoint> data;
    QVector<QPoint> tempP;

    int f=0;
    int r=0;
    int g=0;

    //long px=0;
   // long maxpx=rg.width()*rg.height();


    for (long x=0; x<rg.width()-1; x++)
    {
        for (long y=0; y<rg.height()-1; y++)
        {
          // px= x*rg.height()+y;
          // ui->progressBar->setValue((float)px/(float)maxpx*100);
         //  qApp->processEvents();
           if (qGray(rg.pixel(x,y)) > THR && result.pixel(x,y)==qRgb(255,255,255))
            {
                data.append(QPoint(x,y));
                tempP.append(QPoint(x,y));
                result.setPixel(x,y,qRgb(f,r,g));

                while (!data.empty())
                {
                    QPoint currP= data.takeFirst();
                    // unten
                    if (currP.y()>0 &&
                            result.pixel(currP.x(),currP.y()-1)==qRgb(255,255,255) &&
                            qGray(rg.pixel(currP.x(),currP.y()-1)) > THR)
                    {
                        data.append(QPoint(currP.x(),currP.y()-1));
                        tempP.append(QPoint(currP.x(),currP.y()-1));
                        result.setPixel(QPoint(currP.x(),currP.y()-1),qRgb(f,r,g));
                    }
                    //links
                    if (currP.x()>0 &&
                            result.pixel(currP.x()-1,currP.y())==qRgb(255,255,255) &&
                            qGray(rg.pixel(currP.x()-1,currP.y())) > THR)
                    {
                        data.append(QPoint(currP.x()-1,currP.y()));
                        tempP.append(QPoint(currP.x()-1,currP.y()));
                        result.setPixel(QPoint(currP.x()-1,currP.y()),qRgb(f,r,g));
                    }
                    //rechts
                    if (currP.x()<rg.width()-1 &&
                            result.pixel(currP.x()+1,currP.y())==qRgb(255,255,255) &&
                            qGray(rg.pixel(currP.x()+1,currP.y())) > THR)
                    {
                        data.append(QPoint(currP.x()+1,currP.y()));
                        tempP.append(QPoint(currP.x()+1,currP.y()));
                        result.setPixel(QPoint(currP.x()+1,currP.y()),qRgb(f,r,g));
                    }
                    // oben
                    if (currP.y()<rg.height()-1 &&
                            result.pixel(currP.x(),currP.y()+1)==qRgb(255,255,255) &&
                            qGray(rg.pixel(currP.x(),currP.y()+1)) > THR)
                    {
                        data.append(QPoint(currP.x(),currP.y()+1));
                        tempP.append(QPoint(currP.x(),currP.y()+1));
                        result.setPixel(QPoint(currP.x(),currP.y()+1),qRgb(f,r,g));
                    }
                    // oben links
                    if (currP.y()<rg.height()-1 && currP.x()>0 &&
                            result.pixel(currP.x()-1,currP.y()+1)==qRgb(255,255,255) &&
                            qGray(rg.pixel(currP.x()-1,currP.y()+1)) > THR)
                    {
                        data.append(QPoint(currP.x()-1,currP.y()+1));
                        tempP.append(QPoint(currP.x()-1,currP.y()+1));
                        result.setPixel(QPoint(currP.x()-1,currP.y()+1),qRgb(f,r,g));
                    }
                    // oben rechts
                    if (currP.y()<rg.height()-1 && currP.x()<rg.width()-1 &&
                            result.pixel(currP.x()+1,currP.y()+1)==qRgb(255,255,255) &&
                            qGray(rg.pixel(currP.x()+1,currP.y()+1)) > THR)
                    {
                        data.append(QPoint(currP.x()+1,currP.y()+1));
                        tempP.append(QPoint(currP.x()+1,currP.y()+1));
                        result.setPixel(QPoint(currP.x()+1,currP.y()+1),qRgb(f,r,g));
                    }
                    // unten links
                    if (currP.y()>0 && currP.x()>0 &&
                            result.pixel(currP.x()-1,currP.y()-1)==qRgb(255,255,255) &&
                            qGray(rg.pixel(currP.x()-1,currP.y()-1)) > THR)
                    {
                        data.append(QPoint(currP.x()-1,currP.y()-1));
                        tempP.append(QPoint(currP.x()-1,currP.y()-1));
                        result.setPixel(QPoint(currP.x()-1,currP.y()-1),qRgb(f,r,g));
                    }
                    // unten rechts
                    if (currP.y()>0 && currP.x()<rg.width()-1 &&
                            result.pixel(currP.x()+1,currP.y()-1)==qRgb(255,255,255) &&
                            qGray(rg.pixel(currP.x()+1,currP.y()-1)) > THR)
                    {
                        data.append(QPoint(currP.x()+1,currP.y()-1));
                        tempP.append(QPoint(currP.x()+1,currP.y()-1));
                        result.setPixel(QPoint(currP.x()+1,currP.y()-1),qRgb(f,r,g));
                    }

                }

// color all regions white that are below color threshhold!
                   if (areaTHR > tempP.count())
                   {
                       for (long long i=0; i<tempP.count(); i++)
                       {
                           result.setPixel(tempP.at(i).x(),tempP.at(i).y(),qRgb(250,250,250));
                       }
                   }
            }
// reset temporary variables
                   tempP.clear();
        }
    }
    //rg=rg.scaled(original.width(),original.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
   // rg=rg.convertToFormat(QImage::Format_RGB888);

  //  result=result.scaled(original.width(),original.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
 //   result=result.convertToFormat(QImage::Format_RGB888);

//display result
    for (long i=0; i<rg.width();i++)
        for (long j=0; j<rg.height();j++)
        {
            if (result.pixel(i,j)!= qRgb(0,0,0))
            {
                original.setPixel(QPoint(i,j),qRgb(0,0,0));
            }
        }
   // ui->progressBar->setValue(100);

    //scaleup

   // showImg(original,0);

    return original;
}

inline QVector3D previewDialog::matmul(const QMatrix2x2& m,const QVector3D& p)
{
    QVector3D res;
    res.setX(m(0,0)*p[0]+m(0,1)*p[1]);
    res.setY(m(1,0)*p[0]+m(1,1)*p[1]);
    res.setZ(p[2]);
    return res;
}

void previewDialog::doFourierMellin(QImage i1, QImage i2, int all)
{
    // autocorrelation 1 (fixed image)
    i1=pFouMel->Corr(i1, i1, pFouMel->mx, pFouMel->my, pFouMel->ofx, pFouMel->ofy);
    i1.save(pRef->temp+"autocorr1.png");
    // autocorrelation 2 (moving image)
    i2 = pFouMel->Corr(i2, i2, pFouMel->mx, pFouMel->my, pFouMel->ofx, pFouMel->ofy);
    i2.save(pRef->temp+"autocorr2.png");
    // polarCoordinates 1 (fixed image)
    i1 = pFouMel->polarImage(i1, i1.width(), i1.height());
    i1.save(pRef->temp+"polar1.png");
    // polarCoordinates 2 (moving image)
    i2 = pFouMel->polarImage(i2,i2.width(),i2.height());
    i2.save(pRef->temp+"polar2.png");
    // Correlation of polar Images
    i2 = pFouMel->Corr(i1,i2, pFouMel->mx, pFouMel->my, pFouMel->ofx, pFouMel->ofy);
    i2.save(pRef->temp+"polarcorr.png");
    std::cout << "...... ofx " << pFouMel->ofx << ", ofy " << pFouMel->ofy;
    // detect rotation angle
    pFouMel->rot= +(360.0f/(float)i2.height()*(float)pFouMel->ofy);
    if (fabs(pFouMel->rot)>180)
    {
        pFouMel->rot < 0 ? pFouMel->rot+=360 : pFouMel->rot-=360;
    }
    std::cout << "Rot angle: " << pFouMel->rot << endl;
    // rotate image - FMmoving:
    QImage FMMoving= rotateImage(QImage(pRef->temp+"MovingFM.png"),pFouMel->rot);
    //FMMoving.save(temp+"rotatedMovingFM.png");
    pFouMel->movingPreKernFM = rotateImage(pFouMel->movingPreKernFM,pFouMel->rot);




    // translate images:
    QImage FMFixed(pRef->temp+"FixedFM.png");
    pFouMel->Corr(FMFixed,FMMoving,pFouMel->mx,pFouMel->my, pFouMel->ofx, pFouMel->ofy);
    // translate FM moving
    std::cout << "offset x: " << pFouMel->ofx << endl;
    std::cout << "offset y: " << pFouMel->ofy << endl;
    QImage translatedFMMoving = translateImage(FMMoving,pFouMel->ofx,pFouMel->ofy,1.0f);
    //translatedFMMoving.save("translatedRotatedMovingFM.png");
    pFouMel->movingPreKernFM = translateImage(pFouMel->movingPreKernFM,pFouMel->ofx,pFouMel->ofy,1.0f);
    dispPixie(pFouMel->movingPreKernFM,1);


    if(all==1)
    {
        //rotate image - scaled moving:
        QImage scaledMoving=loadMHD(pRef->temp+"scaledMoving");
        scaledMoving= rotateImage(scaledMoving,pFouMel->rot);
        scaledMoving.save(pRef->temp+"rotatedScaledMoving.png");

        // calc scaling factor for scaled image
        float scalingFactorS=0.0f;
        if(scaledMoving.width()>scaledMoving.height())
            scalingFactorS=(float)scaledMoving.width()/512.0f;
        else
            scalingFactorS=(float)scaledMoving.height()/512.0f;
        std::cout << "scaleF: " << scalingFactorS << endl;
        // translate Scaled Moving
        QImage translatedScaledMoving = translateImage(scaledMoving,pFouMel->ofx,pFouMel->ofy,scalingFactorS);
        pRef->saveMHDall(translatedScaledMoving, pRef->temp + "scaledMoving.raw",1.0f,1.0f);
        translatedScaledMoving.save(pRef->temp + "translatedScaledMoving.png");
        // rotate & translate the original image if color!!!
        QImage movOrg=loadMHD(pRef->temp + "movingImageOrg");

        if(movOrg.format()==QImage::Format_RGB888)
        {
            QList<QImage> movingChannels=pRef->split(movOrg);
            QList<QImage> transformedMovingChannels;
            for (int i=0;i<movingChannels.size();i++)
            {
                       QImage img= rotateImage(movingChannels.at(i),pFouMel->rot);
                       img=translateImage(img,pFouMel->ofx,pFouMel->ofy,scalingFactorS*5.0f);
                       img.save(pRef->temp + "channel_"+QString("%1").arg(i)+".png");
                       transformedMovingChannels.append(img);
            }
            QImage final(transformedMovingChannels.at(0).width(),transformedMovingChannels.at(0).height(),QImage::Format_RGB888);
            for (int x=0; x<final.width();x++)
                for(int y=0; y<final.height();y++)
                    final.setPixel(x,y,qRgb(transformedMovingChannels.at(0).pixel(x,y),transformedMovingChannels.at(1).pixel(x,y),transformedMovingChannels.at(2).pixel(x,y)));
            final.save(pRef->temp + "transformedMovingImage.png");
        }
        else
        {
            QImage img=rotateImage(movOrg,pFouMel->rot);
            img=translateImage(img,pFouMel->ofx,pFouMel->ofy,scalingFactorS*5.0f);
            img.save(pRef->temp + "transformedMovingImage.png");
        }
    }
}

QImage previewDialog::rotateImage(QImage input, float rot)
{
    float angleInRad=-rot/180.0f*M_PI;
    if (input.format()!=QImage::Format_Grayscale16)
        input=input.convertToFormat(QImage::Format_Grayscale16);
    QImage output(input.width(),input.height(),QImage::Format_Grayscale16);
    if (fabs(rot)>0.0001)
    {
        output.fill(65535);
        QMatrix2x2 rotationMatrix;
        rotationMatrix(0,0)=cos(angleInRad);
        rotationMatrix(0,1)=-sin(angleInRad);
        rotationMatrix(1,0)=sin(angleInRad);
        rotationMatrix(1,1)=cos(angleInRad);
        QVector3D point(0,0,1);
        float cx=output.width()/2.0f, cy=output.height()/2.0f;
        QVector3D center(cx,cy,0);
        float swx, swy, ewx,ewy, sumBuffer, sumWeight, weight;
        for(int x=0; x<output.width(); x++)
            for(int y=0; y<output.height(); y++)
            {
                point.setX(x-cx);
                point.setY(y-cy);
                point = matmul(rotationMatrix,point);
                point+=center;
                swx=max(0.0f,min((float)output.width(),floor(point.x())));
                swy=max(0.0f,min((float)output.height(),floor(point.y())));
                ewx=max(0.0f,min((float)output.width(),ceil(point.x())+1));
                ewy=max(0.0f,min((float)output.height(),ceil(point.y())+1));
                sumBuffer =0.0f;
                sumWeight =0.0f;
                for(int lx=swx; lx<ewx;lx++)
                    for(int ly=swy; ly<ewy;ly++)
                    {
                        weight = 1.0/(sqrt(pow(point.x()-lx,2)+pow(point.y()-ly,2))+0.1);
                        sumWeight+=weight;
                        sumBuffer+=weight*qGray(input.pixel(lx,ly));
                    }
                sumBuffer/=sumWeight;
                output.setPixel(x,y, qRgb(sumBuffer,sumBuffer,sumBuffer));
            }
        return output;
    }
    else
        return input;
}

QImage previewDialog::translateImage(QImage input, long ofx, long ofy, float scalingFactor)
{
    ofx=-(float)ofx*scalingFactor;
    ofy=-(float)ofy*scalingFactor;

    QImage output(input.width(),input.height(), QImage::Format_Grayscale16);
        output.fill(qRgb(0,0,0));
        for(int wi=0; wi<input.width();wi++)
            for(int hi=0; hi<input.height();hi++)
            {
               if((wi-ofx)>-1 && (wi-ofx)<input.width() && (hi-ofy)>-1 && (hi-ofy)<input.height())
                   output.setPixel(wi-ofx,hi-ofy,input.pixel(wi,hi));
            }
    return output;
}

void previewDialog::prepareImagesFM(QImage& fixed, QImage& moving)
{
    QImage fixedFM = fixed.convertToFormat(QImage::Format_Grayscale8);
    QImage nfixedFM(512,512,QImage::Format_Grayscale8);
    nfixedFM.fill(fixedFM.pixel(0,0));
    if (fixedFM.width()>fixedFM.height())
    {
        fixedFM=fixedFM.scaledToWidth(512,Qt::SmoothTransformation);
        QPainter pain(&nfixedFM);
        pain.drawImage(QPoint(0,(512-fixedFM.height())/2),fixedFM);
        pain.end();
    }
    else
    {
        fixedFM=fixedFM.scaledToHeight(512,Qt::SmoothTransformation);
        QPainter pain(&nfixedFM);
        pain.drawImage(QPoint((512-fixedFM.width())/2,0),fixedFM);
        pain.end();
    }
    pFouMel->fixedPreKern = nfixedFM;
    dispPixie(pFouMel->fixedPreKern,2);
    QImage mask(":/img/FM_mask.png");
    float  val=0;
    for(int x=0; x<mask.width();x++)
    {
        for(int y=0; y<mask.height();y++)
        {
           val = qGray(nfixedFM.pixel(x,y))* (qGray((float)mask.pixel(x,y))/255.0f);
           nfixedFM.setPixel(x,y,qRgb(val,val,val));
        }
    }
    nfixedFM=nfixedFM.convertToFormat(QImage::Format_Grayscale16);
    nfixedFM.save(pRef->temp+"FixedFM.png");


//-------------------MOVING IMAGE----------------------
    QImage smov = moving.convertToFormat(QImage::Format_Grayscale8);

    int dark=0, bright=0;
    for (int x=0;x<smov.width();x++)
    {
        if (qGray(smov.pixel(x,0))<125)
            dark++;
        else
            bright++;
    }
    if (bright>dark)
        smov.invertPixels();

    QImage movFM =smov;
    movFM =movFM.convertToFormat(QImage::Format_Grayscale8);

    if (ui->cb_FM->isChecked())
    {
        QImage nmovFM(512,512,QImage::Format_Grayscale8);
        nmovFM.fill(fixedFM.pixel(0,0));
        if (movFM.width()>movFM.height())
        {
            movFM=movFM.scaledToWidth(512,Qt::SmoothTransformation);
            QPainter pain(&nmovFM);
            pain.drawImage(QPoint(0,(512-movFM.height())/2),movFM);
            pain.end();
        }
        else
        {
            movFM=movFM.scaledToHeight(512,Qt::SmoothTransformation);
            QPainter pain(&nmovFM);
            pain.drawImage(QPoint((512-movFM.width())/2,0),movFM);
            pain.end();
        }
        pFouMel->movingPreKern = nmovFM;
        pFouMel->movingPreKernFM = nmovFM;
        dispPixie(nmovFM,0);
        QImage mask(":/img/FM_mask.png");
        float  val=0;
        for(int x=0; x<mask.width();x++)
        {
            for(int y=0; y<mask.height();y++)
            {
               val = qGray(nmovFM.pixel(x,y))* (qGray((float)mask.pixel(x,y))/255.0f);
               nmovFM.setPixel(x,y,qRgb(val,val,val));
            }
        }
        nmovFM=nmovFM.convertToFormat(QImage::Format_Grayscale16);
        nmovFM.save(pRef->temp+"MovingFM.png");
    }
}

void previewDialog::on_cb_FM_stateChanged(int arg1)
{
    if(arg1==0)
    {
        ui->pushButton_PreviewFM->setDisabled(true);
        pRef->fouriermellin = false;
    }
    else
    {
        ui->pushButton_PreviewFM->setDisabled(false);
        pRef->fouriermellin = true;
    }
}

void previewDialog::OverlayImages()
{
    QImage Overlay(512,512,QImage::Format_RGB888);
    Overlay.fill(Qt::black);
    for (int i=0; i<Overlay.width(); i++)
    {
        for (int j=0; j<Overlay.height(); j++)
        {
            Overlay.setPixel(i,j, qRgb(pFouMel->fixedPreKern.pixel(i,j),pFouMel->movingPreKern.pixel(i,j),pFouMel->movingPreKernFM.pixel(i,j)));
        }
    }
    dispPixie(Overlay,3);
}

void previewDialog::on_cb_regiongrowing_fixed_stateChanged(int arg1)
{
    if(arg1==0)
        ui->le_regiongrowing_fixed->setDisabled(true);
    else
        ui->le_regiongrowing_fixed->setDisabled(false);
}

void previewDialog::on_cb_regiongrowing_moving_stateChanged(int arg1)
{
    if(arg1==0)
        ui->le_regiongrowing_moving->setDisabled(true);
    else
        ui->le_regiongrowing_moving->setDisabled(false);
}

void previewDialog::on_cb_fixedmask_stateChanged(int arg1)
{
    if(arg1==0)
    {
        pRef->usemask=false;
        ui->lineEdit_fixedmask->setDisabled(true);
    }
    else
    {
        pRef->usemask=true;
        ui->lineEdit_fixedmask->setDisabled(false);
    }
}

void previewDialog::on_cb_rigiditypenalty_stateChanged(int arg1)
{
    if(arg1==0)
        ui->lineEdit_rigiditypenalty->setDisabled(true);
    else
        ui->lineEdit_rigiditypenalty->setDisabled(false);
}
