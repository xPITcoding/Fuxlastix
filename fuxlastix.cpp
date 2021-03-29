#include "fuxlastix.h"
#include "ui_fuxlastix.h"
#include <QFileDialog>
#include <QProcess>
//#include <QTextCodec>
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
#include "definepathdlg.h"

using namespace std;
fuxlastix::fuxlastix(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::fuxlastix)
{
    ui->setupUi(this);
    runElastix = new QProcess();
    runTransformix = new QProcess();

    connect(runElastix,SIGNAL(readyReadStandardOutput()),this,SLOT(outputSlot()));
    connect(runTransformix,SIGNAL(readyReadStandardOutput()),this,SLOT(outputSlot()));
    connect(runElastix,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(fertigSlot()));
    connect(runTransformix,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(ganzfertigSlot()));

    loadSettings();

      //show logo fux
     QPixmap fox (":/img/fuxlastix_logo.png");
     fox = fox.scaledToWidth(100);

     ui->fuxlabel->setPixmap(fox);

     //show logo xpit
    QPixmap xpit (":/img/logo.png");
    xpit = xpit.scaledToHeight(40);
    ui->xpitlabel->setPixmap(xpit);

    ui->gridButton->hide();
}

fuxlastix::~fuxlastix()
{
    delete ui;
}

//saves the UI strings
void fuxlastix::saveSettings()
{
    QFile f("elastix_gui.txt");
    if (f.open(QFile::WriteOnly))
    {
        QTextStream t(&f);
        t << elastixpath << Qt::endl;
        t << transformixpath << Qt::endl;
        t << ui->lineFixedImage->text() << Qt::endl;
        t << ui->lineMovingImage->text() << Qt::endl;
        t << ui->lineOutputDir->text() << Qt::endl;
        t << ui->lineParameterFile->text() << Qt::endl;
        t << ui->lineTemp->text() << Qt::endl;

        f.close();
    }       
}

//loads the UI strings
void fuxlastix::loadSettings()
{
    QFile f("elastix_gui.txt");
    if (!f.exists())
    {
        // werte
        DefinePathDlg dlg;
        dlg.exec();
        elastixpath = dlg.elastixPath();
        transformixpath = dlg.transformixPath();
        saveSettings();
    }

    if (f.open(QFile::ReadOnly))
    {
        QTextStream t(&f);
        elastixpath = t.readLine();
        transformixpath = t.readLine();
        ui->lineFixedImage->setText(t.readLine());
        ui->lineMovingImage->setText(t.readLine());
        ui->lineOutputDir->setText(t.readLine());
        ui->lineParameterFile->setText(t.readLine());
        ui->lineTemp->setText(t.readLine());
        f.close();
    }
}

void fuxlastix::on_pickFixedImageButton_clicked()
{
    QString s=QFileDialog::getOpenFileName(0,"Select fixed image",ui->lineFixedImage->text());
    if (!s.isEmpty())
    {
        ui->lineFixedImage->setText(s);
    }
}

void fuxlastix::on_pickMovingImageButton_clicked()
{
    QString s=QFileDialog::getOpenFileName(0,"Select moving image",ui->lineMovingImage->text());
    if (!s.isEmpty())
    {
        ui->lineMovingImage->setText(s);
    }
}

void fuxlastix::on_pickDirectoryButton_clicked()
{
    QString s=QFileDialog::getExistingDirectory(0,"Select output",ui->lineOutputDir->text());
    if (!s.isEmpty())
    {
        ui->lineOutputDir->setText(s);
    }
}

void fuxlastix::on_pickParameterFileButton_clicked()
{
    QString s=QFileDialog::getOpenFileName(0,"Select colour image",ui->lineParameterFile->text(), "*.txt");
    if (!s.isEmpty())
    {
        ui->lineParameterFile->setText(s);
    }
}

void fuxlastix::on_pushQuitButton_clicked()
{
    close();
}

void fuxlastix::on_pushRunButton_clicked()
{
    saveSettings();

    temp=ui->lineTemp->text()+"/";
    resPath=ui->lineOutputDir->text()+"/";
    transformixCount=0;
    saveSettings();
    QImage fixedImage = loadAllImages(ui->lineFixedImage->text());

    if (fixedImage.format() == QImage::Format_RGB32)
        fixedImage=fixedImage.convertToFormat(QImage::Format_RGB888);
    saveMHDall(fixedImage, temp+"fixedImageOrg.raw");
    fixFormat=fixedImage.format();

    QImage movingImage =loadAllImages(ui->lineMovingImage->text());
    if (movingImage.format() == QImage::Format_RGB32)
        movingImage=movingImage.convertToFormat(QImage::Format_RGB888);
    /*    if(ui->cbRegionGrowing->isChecked())
        movingImage=regionGrowing(movingImage);
    */
    saveMHDall(movingImage, temp+"movingImageOrg.raw");
    movFormat=movingImage.format();
    saveSettings();

    dispPixie(fixedImage,(int)0);
    dispPixie(movingImage,(int)1);

    ScaledFixed800 = scale(fixedImage);
    ScaledMoving800 = scale(movingImage);

    saveMHDall(ScaledFixed800, temp + "scaledFixed.raw", 1.0f, 1.0f);
    saveMHDall(ScaledMoving800, temp + "scaledMoving.raw", 1.0f, 1.0f);

    previewDialog* previewDlgWindow = new previewDialog(this);
    connect(previewDlgWindow,SIGNAL(accepted()),this,SLOT(callElastix()));
    previewDlgWindow->open();
}

void fuxlastix::callElastix(){
    //display adjusted images and call elastix
    ScaledFixed800 = loadMHD(temp+"scaledFixed");
    ScaledMoving800 = loadMHD(temp+"scaledMoving");
    dispPixie(ScaledFixed800,0);
    dispPixie(ScaledMoving800,1);
    run_elastix();
    ui->newsBrowser->append("Start Elastix");
}

void fuxlastix::run_elastix()
{
    // windows call
    //QString const program = "C:/Users/jonas/Documents/CODING/FuxWindowsII/elastix.exe";
    QString const program = elastixpath;

    QStringList arguments;
    //arguments << "--help",
    if (usemask)
        arguments <<"-f" << temp+"scaledFixed.mhd" <<"-m" << temp+"scaledMoving.mhd" << "-fMask" << temp+"fixedMask.mhd" <<"-out" <<temp <<"-p" <<ui->lineParameterFile->text() ;
    else
        arguments <<"-f" << temp+"scaledFixed.mhd" <<"-m" << temp+"scaledMoving.mhd" <<"-out" <<temp <<"-p" <<ui->lineParameterFile->text();

    runElastix->setProgram(program);
    runElastix->setArguments(arguments);
    runElastix->setProcessChannelMode(QProcess::MergedChannels);
    runElastix->start(program,arguments);
    runElastix->waitForStarted(5000);
}

//Appends elastix and transformix output to textbrowser
void fuxlastix::outputSlot()
{
    if (runElastix && runElastix->state() == QProcess::Running)
        ui->textBrowser->append(runElastix->readAllStandardOutput());
    if (runTransformix && runTransformix->state() == QProcess::Running)
        ui->textBrowser->append(runTransformix->readAllStandardOutput());
}

// receives SIGNAL for elastix
void fuxlastix::fertigSlot()
{
    if (QFile(temp +"TransformParameters.0.txt").exists())
            ui->newsBrowser->append("Elastix performed successfully");
    else
        ui->newsBrowser->append("Elastix Error - No TransformParameters file was created");
    change_nr(temp +"TransformParameters.0.txt");
    QImage colorImage;
    if(fouriermellin)
       {
       colorImage.load(temp+"transformedMovingImage.png");
       colorImage=colorImage.convertToFormat(QImage::Format_RGB888);
       }
    else
        colorImage=loadMHD(temp+"movingImageOrg");

    QList<QImage> splitList;
    splitList=split(colorImage);

    saveMHDall(splitList[0], temp+"red.raw");
    saveMHDall(splitList[1], temp+"green.raw");
    saveMHDall(splitList[2], temp+"blue.raw");
    run_transformix(temp+"red.mhd");
    ui->newsBrowser->append("Transformix started (red)");
}

// receives SIGNAL for TRANSFORMIX fertig
// increases transformixCount by 1 after each transformix run
void fuxlastix::ganzfertigSlot()
{
    switch (transformixCount)
    {
    case 0:
        rename("redTransform");
        transformixCount++;
        run_transformix(temp+"green.mhd");
        ui->newsBrowser->append("Transformix started (green)");
        break;

   case 1:
            rename("greenTransform");
            transformixCount++;
            run_transformix(temp+"blue.mhd");
            ui->newsBrowser->append("Transformix started (blue)");
            break;
   case 2:
            rename("blueTransform");
            ui->newsBrowser->append("Transformix performed successfully");
            transformixCount=0;
            QImage final = merge(temp);
            QImage disp1= loadMHD(temp+"scaledFixed");
            saveMHDall(final, temp + "finalImage.raw");
            final.save(temp+"finalImage.png");
            dispPixie(disp1,0);
            qApp->processEvents();
            dispPixie(final,1);
            qApp->processEvents();
            copy_remove();
            ui->gridButton->show();
            break;
    }
}

void myImageCleanupHandler(void *info)
{
    if (info!=NULL)
    {
        free(info);
        info=NULL;
    }
}

// uses TiFF library to load image
/*QImage fuxlastix::loadTIFFImage(const QString& fname)
{
    QImage img;

    emit MSG(QString("Loading ... %1").arg(fname));
    char* _fileName = (char*)malloc(fname.length()+1);
    std::memset(_fileName,0,fname.length()+1);
    std::memcpy(_fileName,fname.toLocal8Bit().constData(),fname.length());

    QFile f(fname);
    f.open(QFile::ReadOnly);
    QDataStream d(&f);
    quint16 _header;
    bool _little=true;
    d >> _header;
    f.close();
    _little = _header == TIFF_LITTLEENDIAN;


    TIFF *tif = TIFFOpen(_fileName,"r");
    if (tif)
    {
        uint32 imageWidth, imageLength;
        short bitsPerPixel;
        short samplesPerPixel;
        uint32 row;
        quint64 offset;
        tdata_t buf;    
        short type;
        long sizeOfDatatype;

        TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&imageWidth);
        TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&imageLength);
        TIFFGetField(tif,TIFFTAG_PHOTOMETRIC,&type);
        TIFFGetField(tif,TIFFTAG_BITSPERSAMPLE,&bitsPerPixel);
        TIFFGetField(tif,TIFFTAG_SAMPLESPERPIXEL,&samplesPerPixel);

        sizeOfDatatype=bitsPerPixel/8;

        if (type==PHOTOMETRIC_RGB)
        {

            sizeOfDatatype = 3;
            dt=RGB24;

        }
        else
        {
            if (bitsPerPixel==32 )
                dt= bit32;
            else if (bitsPerPixel==8)
                dt =bit8;
            else
                dt=bit16;
        }

        unsigned char* pBuffer = (unsigned char*)_TIFFmalloc(imageWidth*imageLength*sizeOfDatatype);
        offset = (quint64)pBuffer;
        long lineLength = TIFFScanlineSize(tif);
        buf = _TIFFmalloc(TIFFScanlineSize(tif));
        for (row = 0; row < imageLength; row++)
        {
            TIFFReadScanline(tif, buf, row);
            memcpy((void*)offset,buf,imageWidth*sizeOfDatatype);
            offset+=imageWidth*sizeOfDatatype;
        }
        TIFFClose(tif);
        _TIFFfree(buf);


        float _minGVal,_maxGVal;
        float _size = imageWidth*imageLength;

        switch (dt)
        {
        case bit32:
            {
                float* pFBuffer=(float*)(pBuffer);
                _minGVal=_maxGVal=pFBuffer[0];

                for (long x=0;x<_size;++x)
                {
                    _minGVal = std::min(_minGVal,pFBuffer[x]);
                    _maxGVal = std::max(_maxGVal,pFBuffer[x]);
                }
            }
            break;
         case RGB24:
            {

            }
            break;
        case bit8:
        {
            float val;
            _minGVal=256;
            _maxGVal=-1;
            for (long x=0;x<_size;++x)
            {

                 val= (float)pBuffer[x];
                _minGVal= std::min(_minGVal,val);
                _maxGVal= std::max(_maxGVal,val);
            }

         }
            break;
         case bit16:
            break;
        }

        // get min max for conversion to 16bit

        quint16 *pU16Buffer = new quint16[imageWidth*imageLength*2];


            switch (dt) {
            case bit32:
                for (unsigned long i=0;i<_size;++i)
                                pU16Buffer[i]=(((float*)pBuffer)[i]-_minGVal)*65535.0f/(_maxGVal-_minGVal);
                                img = QImage((uchar*)pU16Buffer,imageWidth,imageLength,QImage::Format_Grayscale16,myImageCleanupHandler,pU16Buffer);
                             break;
            case bit16:
            {
                QImage local16(imageWidth, imageLength, QImage::Format_Grayscale16);
                unsigned short lbyte,hbyte;
                unsigned short val;
                for (long y=0;y<imageLength; y++)
                    for (long x =0;x<imageWidth;x++)
                    {
                        hbyte=((unsigned char*)pBuffer)[x*2+y*imageWidth*2];
                        lbyte=((unsigned char*)pBuffer)[x*2+1+y*imageWidth*2];
                        _little ? val = hbyte << 8 | lbyte : val = lbyte << 8 | hbyte;
                        *((unsigned short*)(local16.scanLine(y)+x*2))=val;
                    }
                img=local16;
            }
                break;

            case RGB24:
            {
                QImage local24(imageWidth, imageLength, QImage::Format_RGB888);
                unsigned char val;
                for (long y=0; y<imageLength;y++)
                    for (long x = 0;  x<imageWidth*3; x++)
                    {
                        val = ((unsigned char*)pBuffer)[x+y*imageWidth*3];
                        *((unsigned char*)(local24.scanLine(y)+x))=val;
                    }
                 img=local24;
            }
                break;

            case bit8:
            {
                QImage localImg(imageWidth,imageLength,QImage::Format_Grayscale16);
                unsigned short val;
                for (long y=0;y<imageLength; y++)
                    for (long x =0;x<imageWidth;x++)
                    {
                        val = (((unsigned char*)pBuffer)[x+y*imageWidth]-_minGVal)*65535.0f/(_maxGVal-_minGVal);
                        *((unsigned short*)(localImg.scanLine(y)+x*2))=val;
                    }
                img = localImg;
            }
            break;
            }

        _TIFFfree(pBuffer);
        int info= img.format();
        ui->textBrowser->append(QString ("%1").arg(info));

}
    return img;
}
*/

// scales both images by 0.1 for the elastix
QImage fuxlastix::scale(QImage img)
{
    //int info= img.format();
    //ui->textBrowser->append(QString ("%1").arg(info));

    QImage Img = img;
    int type = img.format();
    QImage scaledImg = Img.scaled(Img.width()/5, Img.height()/5, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    scaledImg = scaledImg.convertToFormat(QImage::Format(type));
    //int info2= scaledImg.format();
    //ui->textBrowser->append(QString ("%1").arg(info2));

   return scaledImg;
}

// merges the 3 tranformixed color channels
QImage fuxlastix::merge(QString& temp)
{
    QFile redMhd (temp+"redTransform.mhd");
    long long width =0;
    long long height=0;
    if(!redMhd.open(QFile::ReadOnly))
        QMessageBox::warning(this,"","Could  not load mhd file");
    else
    {
        QTextStream text(&redMhd);
        while(!redMhd.atEnd())
        {
            QString line;
            line = redMhd.readLine();
            if(line.contains("DimSize = "))
            {
                width = line.section(" = ",1,1).section(" ",0,0).toInt();
               height = line.section(" = ",1,1).section(" ",1,1).toInt();

            }
        }
    }
    QImage finalColour(width, height, QImage::Format_RGB888);

    QFile redRaw (temp + "redTransform.raw");
    QFile greenRaw (temp + "greenTransform.raw");
    QFile blueRaw (temp + "blueTransform.raw");

    if (redRaw.open(QFile::ReadOnly) && greenRaw.open(QFile::ReadOnly) && blueRaw.open(QFile::ReadOnly))
    {
        QDataStream dr(&redRaw);
        QDataStream dg(&greenRaw);
        QDataStream db(&blueRaw);

        QVector <float*> buf;

        long fsize = width*height*sizeof(float);
        buf.append(new float [fsize]);
        buf.append(new float [fsize]);
        buf.append(new float [fsize]);

        dr.readRawData((char*)(buf.at(0)),redRaw.size());
        dg.readRawData((char*)(buf.at(1)),greenRaw.size());
        db.readRawData((char*)(buf.at(2)),blueRaw.size());

        float _minVal[3];
        float _maxVal[3];

        _minVal[0]=_minVal[1]=_minVal[2]=650000.0f;
        _maxVal[0]=_maxVal[1]=_maxVal[2]=-650000.0f;

        for (int channel=0;channel<3;channel++)
            for (long long i=0;i<fsize/sizeof (float);++i)
            {
                _minVal[channel] = std::min(_minVal[channel],buf.at(channel)[i]);
                _maxVal[channel] = std::max(_maxVal[channel],buf.at(channel)[i]);
            }

        _minVal[0]=_minVal[1]=_minVal[2]=std::min(_minVal[0],std::min(_minVal[1],_minVal[2]));
        _maxVal[0]=_maxVal[1]=_maxVal[2]=std::max(_maxVal[0],std::max(_maxVal[1],_maxVal[2]));

        for (long y=0;y<height;++y)
            for (long x=0;x<width;++x)
            {

                finalColour.setPixel(x,y,
                                     qRgb(
                                        (buf[0][x+y*width]-_minVal[0])*255.0f/(_maxVal[0]-_minVal[0]),
                                        (buf[1][x+y*width]-_minVal[1])*255.0f/(_maxVal[1]-_minVal[1]),
                                        (buf[2][x+y*width]-_minVal[2])*255.0f/(_maxVal[2]-_minVal[2])));
            }

        redRaw.close();
        greenRaw.close();
        blueRaw.close();

        for (int i=0;i<3;++i) free(buf[i]);
    }
    return finalColour;
}

//splits the color image
QList<QImage> fuxlastix::split(QImage img)
{
    QImage bunt = img;
    QImage red(bunt.width(),bunt.height(),QImage::Format_Grayscale8);
    QImage green(bunt.width(),bunt.height(),QImage::Format_Grayscale8);
    QImage blue(bunt.width(),bunt.height(),QImage::Format_Grayscale8);

    for (int x=0; x<bunt.width(); x++)
        for (int y=0; y<bunt.height(); y++)
        {
            QRgb farbe = bunt.pixel(x,y);
            red.setPixel(x,y,qRgb(qRed(farbe),qRed(farbe),qRed(farbe)));
            green.setPixel(x,y,qRgb(qGreen(farbe),qGreen(farbe),qGreen(farbe)));
            blue.setPixel(x,y,qRgb(qBlue(farbe),qBlue(farbe),qBlue(farbe)));

        }
    QList<QImage> l;
    l.append(red);
    l.append(green);
    l.append(blue);
 return l;
}

void fuxlastix::run_transformix(const QString& image)
{
    // windows call
    //QString const program = "C:/Users/jonas/Documents/CODING/FuxWindowsII/transformix.exe";
    QString const program = transformixpath;
    QStringList arguments;
    arguments << "-in" <<image <<"-tp" <<temp+"newTransformParameters.0.txt" <<"-jac" <<"all" <<"-def" <<"all" <<"-out" <<temp;
    runTransformix->setProgram(program);
    runTransformix->setArguments(arguments);
    runTransformix->setProcessChannelMode(QProcess::MergedChannels);
    runTransformix->start(program,arguments);
    runTransformix->waitForStarted(5000);
}

// Changes "Size" And "Spacing" of the TransformParameters.txt
//removed spacing /10 for windows/elastix 5.0.1 // replaced with 0.1
 void fuxlastix::change_nr(const QString& fileName)
 {

     QFileInfo info(fileName);

     QFile f(fileName);
     QFile fs(info.absolutePath()+"/new"+info.fileName());

     QString savename=info.absolutePath()+"/new"+info.fileName();
     std::cout << savename.toLatin1().constData();

     if (f.open(QFile::ReadOnly) && fs.open(QFile::WriteOnly))
     {
         QTextStream t(&f);
         QTextStream ts(&fs);

         const char* pattern[] = {"(Size ","(Spacing "};

         while (!t.atEnd())
         {
             QString s;
             s = t.readLine();

             int cNr = -1;
             for (int i=0;i<2;++i)
             {
                 if (s.contains(pattern[i])) cNr=i;
             }

             switch (cNr)
             {
             case 0 :
                 {
                     s = s.left(s.length()-1);
                     int dimx = s.section(" ",1,1).toInt();
                     int dimy = s.section(" ",2,2).toInt();
                     ts << "(Size " << dimx*10 << " " << dimy*10 << ")" << Qt::endl;
                 }
                 break;
             case 1:
                 {
                     s = s.left(s.length()-1);
                     float esx = s.section(" ",1,1).toFloat();
                     float esy = s.section(" ",2,2).toFloat();
                     ts << "(Spacing " << 0.1 << " " << 0.1 << ")" << Qt::endl;
                 }
                 break;
             default:
                 ts << s << Qt::endl;
                 break;
             }
         }
         f.close();
         fs.close();

         QFile::remove(fileName);
         QFile::copy(savename,fileName);
     }

 }

//saves as .mhd
//used
void fuxlastix::saveMHDall(const QImage& img, const QString& filename,const float& rx, const float& ry,dataType localDt)
{
    if (localDt==INVALID)
    {
        //int info= img.format();
        //ui->textBrowser->append(QString ("%1").arg(info));

        if (img.format()== QImage::Format_Grayscale16)
            dt=bit16;
        else if(img.format()==QImage::Format_RGB888)
            dt=RGB24;
        else
            dt=bit8;
        if (img.format()==QImage::Format_Mono) dt=BIN;

        localDt=dt;
    }

    switch(localDt) {
    case BIN:
    {
        QFile f(filename);
        if (f.open(QFile::WriteOnly))
        {
            QDataStream stream (&f);
            for (long long y=0; y<img.height(); ++y)
            {
                uchar* lineBuffer=(uchar*)img.scanLine(y);
                for (long long x=0;x<img.width();++x) {
                    lineBuffer[x]=(lineBuffer[x]!=0)*255;
                }

                stream.writeRawData((const char*) lineBuffer, img.width());
            }
            f.close();
        }

        QFileInfo info(filename);
        QFile ft(info.absolutePath()+"/"+info.baseName()+".mhd");
        if (ft.open(QFile::WriteOnly))
        {
            QTextStream t(&ft);
            t << "ObjectType = Image" << Qt::endl;
            t << "NDims = 2" << Qt::endl;
            t << "BinaryData = True" << Qt::endl;
            t << "BinaryDataByteOrderMSB = True" << Qt::endl;
            t << "DimSize = " << img.width() <<" " << img.height() << Qt::endl;
            t << "ElementSize = " << rx << " " << ry << Qt::endl;
            t << "ElementType = MET_UCHAR" << Qt::endl;
            t << "ElementDataFile = " <<info.fileName() <<Qt::endl;
            ft.close();
        }
     }
        break;
    case bit16:
    {
        QFile f(filename);
        if (f.open(QFile::WriteOnly))
        {
            QDataStream stream (&f);
            for (long long y=0; y<img.height(); ++y)
            {
                stream.writeRawData((const char*) img.scanLine(y), img.width()*2);
            }


            f.close();
        }

        QFileInfo info(filename);
        QFile ft(info.absolutePath()+"/"+info.baseName()+".mhd");
        if (ft.open(QFile::WriteOnly))
        {
            QTextStream t(&ft);
            t << "ObjectType = Image" << Qt::endl;
            t << "NDims = 2" << Qt::endl;
            t << "BinaryData = True" << Qt::endl;
            t << "BinaryDataByteOrderMSB = False" << Qt::endl; //ENDIAN
            t << "DimSize = " <<img.width() <<" " <<img.height() <<Qt::endl;
            t << "ElementSize = " << rx << " " << ry << Qt::endl;
            t << "ElementType = MET_USHORT" << Qt::endl;
            t << "ElementDataFile = " <<info.fileName() << Qt::endl;
            ft.close();
        }
       break;
    };
    case bit8:
    {
        QFile f(filename);
        if (f.open(QFile::WriteOnly))
        {
            QDataStream stream (&f);
            for (long long y=0; y<img.height(); ++y)
            {
                stream.writeRawData((const char*) img.scanLine(y), img.width()*1);
            }

        }

        QFileInfo info(filename);
        QFile ft(info.absolutePath()+"/"+info.baseName()+".mhd");
        if (ft.open(QFile::WriteOnly))
        {
            QTextStream t(&ft);
            t << "ObjectType = Image" << Qt::endl;
            t << "NDims = 2" << Qt::endl;
            t << "BinaryData = True" << Qt::endl;
            t << "BinaryDataByteOrderMSB = True" << Qt::endl;
            t << "DimSize = " <<img.width() <<" " <<img.height() << Qt::endl;
            t << "ElementSize = " << rx << " " << ry << Qt::endl;
            t << "ElementType = MET_UCHAR" << Qt::endl;
            t << "ElementDataFile = " <<info.fileName() <<Qt::endl;
            ft.close();
        }; break;
     }

    case RGB24:
        {
            QFile f(filename);
            if (f.open(QFile::WriteOnly))
            {
                QDataStream stream (&f);
                for (long long y=0; y<img.height(); ++y)
                {
                    stream.writeRawData((const char*) img.scanLine(y), img.width()*3);
                }

            }

            QFileInfo info(filename);
            QFile ft(info.absolutePath()+"/"+info.baseName()+".mhd");
            if (ft.open(QFile::WriteOnly))
            {
                QTextStream t(&ft);
                t << "ObjectType = Image" << Qt::endl;
                t << "NDims = 2" << Qt::endl;
                t << "BinaryData = True" << Qt::endl;
                t << "BinaryDataByteOrderMSB = True" << Qt::endl;
                t << "DimSize = " <<img.width() <<" " <<img.height() << Qt::endl;
                t << "ElementSize = " << rx << " " << ry << Qt::endl;
                t << "ElementNumberOfChannels = 3" << Qt::endl;
                t << "ElementType = MET_UCHAR_ARRAY" << Qt::endl;
                t << "ElementDataFile = " <<info.fileName() << Qt::endl;
                ft.close();
            }; break;
        }


    }


}

// Copies files from temp to output dir
void fuxlastix::copy_remove()
{
    QDir tempDir(temp);
    QDir destDir(ui->lineOutputDir->text());
    if (!destDir.exists())
    {
        QString output =  "Creating directory.";
        ui->newsBrowser->append(output);
        destDir.mkdir(ui->lineOutputDir->text());
    }
    tempDir.setFilter(QDir::Files);
    QList<QFileInfo> lst=tempDir.entryInfoList();

    for (int i=0;i<lst.count();++i)
    {
        QFile::remove(destDir.path()+"/"+lst.at(i).fileName());
        QFile::copy(lst.at(i).absoluteFilePath(),destDir.path()+"/"+lst.at(i).fileName());
        QFile::remove(lst.at(i).absoluteFilePath());
    }

}

//rename renames transformix results from "result.raw" to "...Transform.raw"
void fuxlastix::rename(QString colour)
{
   QFile::rename(temp+"result.mhd", temp+colour+".mhd");
   QFile channelMHD(temp+colour+".mhd");
    if(channelMHD.open(QFile::ReadWrite))
    {
        QTextStream text(&channelMHD);

        QString all = text.readAll();
        all.replace("result",colour);
        text.device()->seek(0);
        text << all;
        channelMHD.close();

    }

    QFile channelRaw(temp+"result.raw");
        if(channelRaw.exists())
        {
            channelRaw.rename((temp+"result.raw", temp+colour+".raw"));
        }

}

void fuxlastix::on_pickTempButton_clicked()
{
    QString s=QFileDialog::getExistingDirectory(0,"Select Temporary Folder",ui->lineTemp->text());
    if (!s.isEmpty())
    {
        ui->lineTemp->setText(s);
    }
}

//creates checkerboard pattern
void fuxlastix::on_checkerButton_clicked()
{
    resPath=ui->lineOutputDir->text()+"/";
    QImage check1 = loadMHD(resPath+"fixedImageOrg");
    check1.invertPixels();
    QImage check2 = loadMHD(resPath+"finalImage");
    int chsize = ui->spinBox->value();

    if (check1.width() > 0 && check2.width()>0)
    {

        check1=check1.convertToFormat(QImage::Format_RGB888);
        check2=check2.convertToFormat(QImage::Format_RGB888);
        check2=check2.scaledToWidth(check1.width(),Qt::SmoothTransformation);
        QImage result (check1.width(), check1.height(), QImage::Format_RGB888);

        result = check1;
        QPainter pain(&result);
        int countx=check1.width()/chsize;
        int county=check1.height()/chsize;

        for (int y=0;y<county;++y)
            for (int x=0;x<countx;++x)
                if ((x+y)%2==0)
                    pain.drawImage(QRectF(x*chsize,y*chsize,chsize,chsize),check2,QRectF(x*chsize,y*chsize,chsize,chsize));

        pain.end();
        result.save(resPath+"check.png");

       dispPixie(result,0);
    }
    else
        QMessageBox::warning(0,"Warning","Run Elastix first");
}

void fuxlastix::on_saveCheckerButton_clicked()
{
    saveMHDall(result2, ui->lineOutputDir->text()+"checker.raw");
}

void fuxlastix::on_gridButton_clicked()
{
    gridcalc* pGridCalc = new gridcalc(this);
    pGridCalc->open();
}

QImage fuxlastix::loadMHD (QString name)
{
    QFile Mhd (name+".mhd");
    long long width =0;
    long long height=0;
    bool _big = false;
    QString mhdFormat;
    if(!Mhd.open(QFile::ReadOnly))
        QMessageBox::warning(this,"","Could  not load mhd file");
    else
    {
        QTextStream text(&Mhd);
        while(!Mhd.atEnd())
        {
            QString line;
            line = Mhd.readLine();
            if(line.contains("DimSize = "))
            {
               width = line.section(" = ",1,1).section(" ",0,0).toInt();
               height = line.section(" = ",1,1).section(" ",1,1).toInt();
            }
            if (line.contains("ElementType"))
            {
                mhdFormat = line.section(" = ",1,1);
            }
            if (line.contains("BinaryDataByteOrderMSB"))
            {
                _big = line.section(" = ",1,1).toLower().contains("true");
            }
        }
    }

    QFile raw (name+".raw");
    if (!raw.open(QFile::ReadOnly))
        QMessageBox::warning(this,"","Could  not load raw file");
    else
    {
        const char* pattern[]={"MET_UCHAR","MET_USHORT","MET_FLOAT","MET_UCHAR_ARRAY"};
        int formatId=-1;
        for (int i=0;i<4;++i)
            if (mhdFormat.contains(pattern[i])) formatId=i;

        QDataStream r(&raw);
        r.setByteOrder(_big ? QDataStream::BigEndian : QDataStream::LittleEndian);

        switch (formatId) {
        case MET_UCHAR:
            //8bit grayvalue;
            {
                uchar* buf=(uchar*)malloc(width*height);
                r.readRawData((char*)buf,raw.size());
                    QImage out(width, height, QImage::Format_Grayscale8);
                unsigned char val;
                for (long y=0; y<height;y++)
                    for (long x = 0;  x<width; x++)
                    {
                        val = (buf)[x+y*width];
                        *((uchar*)(out.scanLine(y)+x))=val;
                    }

                raw.close();
                free(buf);
                return out;
            }
            break;

        case MET_USHORT:
            //16bit grayvalue;
            {
                ushort* buf=(ushort*)malloc(width*height*2);
                for (long i=0;i<width*height;++i)
                    r >> buf[i];

                QImage out(width, height, QImage::Format_Grayscale16);
                unsigned short val;
                for (long y=0; y<height;y++)
                    for (long x = 0;  x<width; x++)
                    {
                        val = (buf)[x+y*width];
                        *((ushort*)(out.scanLine(y)+x*2))=val;
                    }

                raw.close();
                free(buf);
                return out;
            }
            break;

        case MET_FLOAT:
            //32bit grayvalue;
            {
                float _minGVal,_maxGVal;
                float bufsize= width*height*sizeof(float);
                QImage out(width, height, QImage::Format_Grayscale16);
                ushort *buf16= (ushort*)malloc(width*height*2);
                float* buf=(float*)malloc(bufsize);
                r.readRawData((char*)buf,raw.size());

                _minGVal=_maxGVal=buf[0];

                for (long x=0;x<width*height;++x)
                {
                    _minGVal = std::min(_minGVal,buf[x]);
                    _maxGVal = std::max(_maxGVal,buf[x]);
                }

                for (long long i=0;i<width*height;++i)
                    buf16[i]=(((float*)buf)[i]-_minGVal)*65535.0f/(_maxGVal-_minGVal);
                    out = QImage((uchar*)buf16,width,height,QImage::Format_Grayscale16);

                    raw.close();
                    free(buf);
                    return out;
            }
            break;
        case MET_UCHAR_ARRAY:
            //RGB COLOR (8bit per channel);
            {
                uchar* buf=(uchar*)malloc(width*height*3);
                r.readRawData((char*)buf,raw.size());

                QImage out(width, height, QImage::Format_RGB888);
                unsigned char val;
                for (long y=0; y<height;y++)
                    for (long x = 0;  x<width*3; x++)
                    {
                        val = (buf)[x+y*width*3];
                        *((uchar*)(out.scanLine(y)+x))=val;
                    }

                raw.close();
                free(buf);
                return out;
                break;
            }
        }
    return QImage();
    }
}

/*void fuxlastix::doFourierMellin()
{
    //QImage i2=loadMHD(temp+"FixedFM");
    //QImage i1=loadMHD(temp+"MovingFM");
    QImage i2,i1;
    i2.load(temp+"FixedFM.png");
    i2=i2.convertToFormat(QImage::Format_Grayscale16);
    i1.load(temp+"MovingFM.png");
    i1=i1.convertToFormat(QImage::Format_Grayscale16);
    QImage i2fm=i2;
    QImage i1fm=i2;
    long mx, my;
    long ofx, ofy;

    i1=  callFM.Corr(i1, i1, mx, my, ofx, ofy);
    i2 = callFM.Corr(i2, i2, mx, my, ofx, ofy);
    i1 = callFM.polarImage(i1, i1.width(), i1.height());
    i2 = callFM.polarImage(i2,i2.width(),i2.height());
    i2 = callFM.Corr(i1,i2, mx, my, ofx, ofy);
    float rot= +(360.0f/(float)i2.width()*(float)ofx);
    if (fabs(rot)>0.0001)
    {
        ui->newsBrowser->append("rotation angle detected: "+QString ("%1").arg(rot));
        QTransform t;
        i2fm=i2fm.transformed(t.rotate(rot),Qt::SmoothTransformation);
        for (int w =0; w<i2fm.width(); w++)
            for(int h=0; h<i2fm.height();h++)
            {
                if((i2fm.pixelColor(w,h)).alpha()==0)
                    i2fm.setPixel(w,h,qRgb(255,255,255));
            }
        i2fm=i2fm.convertToFormat(QImage::Format_Grayscale16);
        i2fm=i2fm.copy(i2fm.width()/2-i1fm.width()/2,i2fm.height()/2-i1fm.height()/2, i1fm.width(), i1fm.height());
    }
    //saveMHDall(i2fm, temp + "scaledMoving.raw");

    i1fm=callFM.Corr(i1fm,i2fm,mx,my, ofx, ofy);
    QImage tra(i2fm.width(),i2fm.height(), QImage::Format_Grayscale16);
    tra.fill(qRgb(255,255,255));
    for(int wi=0; wi<i2fm.width();wi++)
        for(int hi=0; hi<i2fm.height();hi++)
        {
           if((wi+ofx)>-1 && (wi+ofx)<tra.width() && (hi+ofy)>-1 && (hi+ofy)<tra.height())
               tra.setPixel(wi+ofx,hi+ofy,i2fm.pixel(wi,hi));
        }
    saveMHDall(tra, temp + "scaledFixed.raw");
    dispPixie(tra,0);

    //and rotate and transform the original big image
    QImage kFI = loadMHD(temp+"FixedImageOrg");
    if (fabs(rot)>0.0001)
    {
        QTransform t;
        kFI=kFI.transformed(t.rotate(rot),Qt::SmoothTransformation);
        for (int w =0; w<kFI.width(); w++)
            for(int h=0; h<kFI.height();h++)
            {
                if((kFI.pixelColor(w,h)).alpha()==0)
                    kFI.setPixel(w,h,qRgb(255,255,255));
            }
        kFI=kFI.convertToFormat(QImage::Format_Grayscale16);
        kFI=kFI.copy(kFI.width()/2-i1fm.width()*10/2,kFI.height()/2-i1fm.height()*10/2, i1fm.width()*10, i1fm.height()*10);

    }

    QImage tra2(kFI.width(),kFI.height(), QImage::Format_Grayscale16);
    tra2.fill(qRgb(255,255,255));
    for(int wi=0; wi<kFI.width();wi++)
        for(int hi=0; hi<kFI.height();hi++)
        {
           if((wi+ofx*10)>-1 && (wi+ofx*10)<tra2.width() && (hi+ofy*10)>-1 && (hi+ofy*10)<tra2.height())
               tra2.setPixel(wi+ofx*10,hi+ofy*10,kFI.pixel(wi,hi));
        }
    dispPixie(tra2,1);
    saveMHDall(tra2, temp + "fixedImageOrg.raw");

}

void fuxlastix::dispPixie(QImage input, int side)
{
  QPixmap pix;
  pix.convertFromImage(input);
  if (input.width()>input.height())
      pix=pix.scaledToWidth(500);
  else
      pix=pix.scaledToHeight(500);

  if (side == 0)
      ui->ctlabel->setPixmap(pix);
  else if (side == 1)
      ui->histolabel->setPixmap(pix);
  else
      QMessageBox::warning(0,"WRONG","wrong side!");

}*/

QImage fuxlastix::loadAllImages(QString fname)
{       
        QImageReader reader;
        reader.setAllocationLimit(250);
        reader.setFileName(fname);

        QImage img=reader.read();

        if (ui->scaleCheckBox->isChecked())
        {
            int type= img.format();
            if(img.width()>img.height())
               img=img.scaledToWidth(ui->scaleSB->value(),Qt::SmoothTransformation);
            else
               img=img.scaledToHeight(ui->scaleSB->value(),Qt::SmoothTransformation);
            img=img.convertToFormat(QImage::Format(type));
        }

        return img;
}

void fuxlastix::dispPixie(QImage input, int side)
{
  QPixmap pix;
  pix.convertFromImage(input);
  if (input.width()>input.height())
      pix=pix.scaledToWidth(500);
  else
      pix=pix.scaledToHeight(500);

  if (side == 0)
      ui->ctlabel->setPixmap(pix);
  else if (side == 1)
      ui->histolabel->setPixmap(pix);
  else
      QMessageBox::warning(0,"WRONG","wrong side!");
}
