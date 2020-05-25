#include "fuxlastix.h"
#include "ui_fuxlastix.h"
#include <QFileDialog>
#include <QProcess>
#include <QTextCodec>
#include <QTextStream>
#include "tiffio.h"
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
    temp=ui->lineTemp->text()+"/";

      //show logo fux
     QPixmap fox (":/img/fuxlastix_logo.png");
     fox = fox.scaledToWidth(100);

     ui->fuxlabel->setPixmap(fox);

     //show logo xpit
    QPixmap xpit (":/img/logo.png");
    xpit = xpit.scaledToHeight(40);
    ui->xpitlabel->setPixmap(xpit);

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

        t << ui->lineFixedImage->text() << endl;
        t << ui->lineMovingImage->text() << endl;
        t << ui->lineMovingColour->text() << endl;
        t << ui->lineOutputDir->text() << endl;
        t << ui->lineParameterFile->text() << endl;
        t << ui->lineTemp->text() << endl;

        f.close();
    }
}

//loads the UI strings
void fuxlastix::loadSettings()
{
    QFile f("elastix_gui.txt");
    if (f.open(QFile::ReadOnly))
    {
        QTextStream t(&f);

        ui->lineFixedImage->setText(t.readLine());
        ui->lineMovingImage->setText(t.readLine());
        ui->lineMovingColour->setText(t.readLine());
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

void fuxlastix::on_pickMovingColourButton_clicked()
{
    QString s=QFileDialog::getOpenFileName(0,"Select colour image",ui->lineMovingColour->text());
    if (!s.isEmpty())
    {
        ui->lineMovingColour->setText(s);
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
    transformixCount=0;
    saveSettings();
    QImage fixedImage = loadTIFFImage(ui->lineFixedImage->text());
    QImage movingImage =loadTIFFImage(ui->lineMovingImage->text());
    check1 = fixedImage;
    QPixmap fix;

    fix.convertFromImage(fixedImage);
    if (fix.width()>fix.height())
    fix=fix.scaledToWidth(500);
    else
    fix=fix.scaledToHeight(500);
    ui-> ctlabel->setPixmap(fix);

    fixedImage = scale(fixedImage);
    movingImage = scale (movingImage);

    saveMHDall(fixedImage, temp + "scaledFixed.raw");
    saveMHDall(movingImage, temp + "scaledMoving.raw");
    run_elastix();


}

void fuxlastix::run_elastix()
{

    QString const program = "/bin/elastix";
    QStringList arguments;
    //arguments << "--help",
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
    ui->textBrowser->append("FERTIG");
    change_nr(temp +"TransformParameters.0.txt");
    QImage colorImage =loadTIFFImage(ui->lineMovingColour->text());
    QList<QImage> splitList;
    splitList=split(colorImage);
    saveMHDall(splitList[0], temp+"red.raw");
    saveMHDall(splitList[1], temp+"green.raw");
    saveMHDall(splitList[2], temp+"blue.raw");
    run_transformix(temp+"red.mhd");
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
        break;

   case 1:
            rename("greenTransform");
            transformixCount++;
            run_transformix(temp+"blue.mhd"); break;
   case 2:
            rename("blueTransform");
            ui->textBrowser->append("TRANFORMIX FERTIG!!");
            transformixCount=0;
            QImage final = merge(temp);
            saveMHDall(final, temp + "finalImage.raw");
            check2 = final;
            QPixmap p;
            p.convertFromImage(final);
            if (p.width()>p.height())
            p=p.scaledToWidth(500);
            else
            p=p.scaledToHeight(500);
            ui-> histolabel->setPixmap(p);
            copy_remove();

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
QImage fuxlastix::loadTIFFImage(const QString& fname)
{
    QImage img;

    emit MSG(QString("Loading ... %1").arg(fname));
    const char* _fileName = fname.toLatin1().constData();
    TIFF *tif = TIFFOpen(_fileName,"r");
    if (tif)
    {
        uint32 imageWidth, imageLength;
        short bitsPerPixel;
        uint32 row;
        uint64 offset;
        tdata_t buf;    
        short type;
        long sizeOfDatatype;


        TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&imageWidth);
        TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&imageLength);
        TIFFGetField(tif,TIFFTAG_PHOTOMETRIC,&type);
        TIFFGetField(tif,TIFFTAG_BITSPERSAMPLE,&bitsPerPixel);
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
        offset = (uint64)pBuffer;
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

        //quint16 *pU16Buffer =(quint16*)_TIFFmalloc(imageWidth*imageLength*2);
        quint16 *pU16Buffer = new quint16[imageWidth*imageLength*2];


            switch (dt) {
            case bit32:      for (unsigned long i=0;i<_size;++i)
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
                        val = hbyte << 8 | lbyte;
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
               /*uchar * colorbuffer = new uchar[imageWidth*imageLength*3];
                            memcpy(colorbuffer,pBuffer,imageWidth*imageLength*3);
                            img = QImage(colorbuffer,imageWidth,imageLength,QImage::Format_RGB888);*/

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

// scales both images by 0.1 for the elastix
QImage fuxlastix::scale(QImage img)
{
    int info= img.format();
    ui->textBrowser->append(QString ("%1").arg(info));

    QImage Img = img;
    QImage scaledImg = Img.scaled(Img.width()/10, Img.height()/10, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    scaledImg = scaledImg.convertToFormat(QImage::Format_Grayscale16);
    int info2= scaledImg.format();
    ui->textBrowser->append(QString ("%1").arg(info2));

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


void fuxlastix::saveMHDscaled(const QImage& img, const QString& fname)
{
    QFile f(fname);
    if (f.open(QFile::WriteOnly))
    {
        QDataStream stream (&f);
        stream.writeRawData((const char*) img.bits(), img.sizeInBytes());
        f.close();
    }

    QFileInfo info(fname);
    QFile ft(info.absolutePath()+"/"+info.baseName()+".mhd");
    if (ft.open(QFile::WriteOnly))
    {
        QTextStream t(&ft);
        t << "ObjectType = Image" << endl;
        t << "NDims = 2" << endl;
        t << "BinaryData = True" << endl;
        t << "BinaryDataByteOrderMSB = True" << endl;
        t << "DimSize = " <<img.width() <<" " <<img.height() << endl;
        t << "ElementSize = 1.0 1.0" << endl;
        t << "ElementType = MET_SHORT" << endl;
        t << "ElementDataFile = " <<info.fileName() <<endl;
        ft.close();
    }
}
// saves QImage as .mhd
// NOTUSED
void fuxlastix::saveMHD(const QImage& l , const QString& fname)
{
    QFile f(fname);
    if (f.open(QFile::WriteOnly))
    {
        QDataStream stream (&f);
        stream.writeRawData((const char*) l.bits(), l.sizeInBytes());
        f.close();
    }

    QFileInfo info(fname);
    QFile ft(info.absolutePath()+"/"+info.baseName()+".mhd");
    if (ft.open(QFile::WriteOnly))
    {
        QTextStream t(&ft);
        t << "ObjectType = Image" << endl;
        t << "NDims = 2" << endl;
        t << "BinaryData = True" << endl;
        t << "BinaryDataByteOrderMSB = True" << endl;
        t << "DimSize = " <<l.width() <<" " <<l.height() << endl;
        t << "ElementSize = 0.1 0.1" << endl;
        t << "ElementType = MET_UCHAR" << endl;
        t << "ElementDataFile = " <<info.fileName() <<endl;
        ft.close();
    }
}

void fuxlastix::run_transformix(const QString& image)
{
    QString const program = "/bin/transformix";
    QStringList arguments;
    arguments << "-in" <<image <<"-tp" <<temp+"newTransformParameters.0.txt" <<"-out" <<temp;
    runTransformix->setProgram(program);
    runTransformix->setArguments(arguments);
    runTransformix->setProcessChannelMode(QProcess::MergedChannels);
    runTransformix->start(program,arguments);
    runTransformix->waitForStarted(5000);
}

// Changes "Size" And "Spacing" of the TransformParameters.txt
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
                     ts << "(Size " << dimx*10 << " " << dimy*10 << ")" << endl;
                 }
                 break;
             case 1:
                 {
                     s = s.left(s.length()-1);
                     float esx = s.section(" ",1,1).toFloat();
                     float esy = s.section(" ",2,2).toFloat();
                     ts << "(Spacing " << esx/10.0f << " " << esy/10.0f << ")" << endl;
                 }
                 break;
             default:
                 ts << s << endl;
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
void fuxlastix::saveMHDall(const QImage& img, const QString& filename)
{
    int info= img.format();
    ui->textBrowser->append(QString ("%1").arg(info));

    if (img.format()== QImage::Format_Grayscale16)
        dt=bit16;
    else if(img.format()==QImage::Format_RGB888)
        dt=RGB24;
    else
        dt=bit8;

    switch(dt) {
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
            t << "ObjectType = Image" << endl;
            t << "NDims = 2" << endl;
            t << "BinaryData = True" << endl;
            t << "BinaryDataByteOrderMSB = True" << endl;
            t << "DimSize = " <<img.width() <<" " <<img.height() << endl;
            t << "ElementSize = 1.0 1.0" << endl;
            t << "ElementType = MET_USHORT" << endl;
            t << "ElementDataFile = " <<info.fileName() <<endl;
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
            t << "ObjectType = Image" << endl;
            t << "NDims = 2" << endl;
            t << "BinaryData = True" << endl;
            t << "BinaryDataByteOrderMSB = True" << endl;
            t << "DimSize = " <<img.width() <<" " <<img.height() << endl;
            t << "ElementSize = 0.1 0.1" << endl;
            t << "ElementType = MET_UCHAR" << endl;
            t << "ElementDataFile = " <<info.fileName() <<endl;
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
                t << "ObjectType = Image" << endl;
                t << "NDims = 2" << endl;
                t << "BinaryData = True" << endl;
                t << "BinaryDataByteOrderMSB = True" << endl;
                t << "DimSize = " <<img.width() <<" " <<img.height() << endl;
                t << "ElementSize = 0.1 0.1" << endl;
                t << "ElementNumberOfChannels = 3" << endl;
                t << "ElementType = MET_UCHAR_ARRAY" << endl;
                t << "ElementDataFile = " <<info.fileName() <<endl;
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
        ui->textBrowser->append(output);
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
    if (check1.width() > 0 && check2.width()>0)
    {

        check1=check1.convertToFormat(QImage::Format_RGB888);
        check2=check2.convertToFormat(QImage::Format_RGB888);
        check2=check2.scaledToWidth(check1.width(),Qt::SmoothTransformation);
        QImage result (check1.width(), check1.height(), QImage::Format_RGB888);

        result = makeCheck(result);

    result2=result;

       QPixmap checkpix;

       checkpix.convertFromImage(result);
       if (result.width() > result.height())
       checkpix=checkpix.scaledToWidth(500);
       else
       checkpix=checkpix.scaledToHeight(500);


       ui->ctlabel->setPixmap(checkpix);
       //
    }
    else
        QMessageBox::warning(0,"Warning","Run Elastix first");
}


QImage fuxlastix::makeCheck( QImage result)
{

    int chsize = ui->spinBox->value();
    result=check1;
    QPainter pain(&result);
    int countx=check1.width()/chsize;
    int county=check1.height()/chsize;

    for (int y=0;y<county;++y)
        for (int x=0;x<countx;++x)
            if ((x+y)%2==0)
                pain.drawImage(QRectF(x*chsize,y*chsize,chsize,chsize),check2,QRectF(x*chsize,y*chsize,chsize,chsize));

    pain.end();
    return result;
}

void fuxlastix::on_saveCheckerButton_clicked()
{
    saveMHDall(result2, ui->lineOutputDir->text()+"/checker.raw");
}
