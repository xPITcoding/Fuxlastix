#include "colorcheck.h"
#include "ui_colorcheck.h"
#include <QMessageBox>
#include <QThread>

colorCheck::colorCheck(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::colorCheck)
{
    ui->setupUi(this);
    loadSettings();
    if(movFormat != 13)
    {
        ui->MovingChannelBox->setEnabled(false);
    }

    if(fixFormat != 13)
    {
        ui->FixedChannelBox->setEnabled(false);
    }
}


colorCheck::~colorCheck()
{
    delete ui;
}

void colorCheck::loadSettings()
{
    QString tt,fi,mo, fma;
    QFile f("elastix_gui.txt");
    if (f.open(QFile::ReadOnly))
    {
        QTextStream t(&f);
       //for results folder:  for (int x=0; x<4; x++)


        for (int x=0; x<5; x++)
         tt=t.readLine();
         fi=t.readLine();
         mo=t.readLine();
         fma=t.readLine();
    }

    fixFormat=fi.toInt();
    movFormat=mo.toInt();

        temp=tt;
        temp=temp+"/";

        if (fma == "1")
            fouMelActive=true;
        f.close();
}


void colorCheck::on_pushButton_clicked()
{
    QImage sfix = loadMHD("scaledFixed");
    if(ui->FixedInvertCheckBox->isChecked())
        sfix.invertPixels();

    QList <QImage> fixList = fux.split(sfix);

    if (fouMelActive==true)
    {
        sfix=sfix.convertToFormat(QImage::Format_Grayscale8);
        saveMHDcc(sfix, temp+"FixedFM.raw");
    }


    if(fixFormat == 13)
    {



        //Fixed image combo box
        switch (ui->FixedChannelBox->currentIndex())
        {
        case COLOR: //save color image;
            saveMHDcc(sfix, temp+"scaledFixed.raw");
            break;
        case AVG: // save gv image;
            sfix=sfix.convertToFormat(QImage::Format_Grayscale16);
            saveMHDcc(sfix, temp+"scaledFixed.raw");
            break;
        case RED: //save red channel;
            saveMHDcc(fixList[0].convertToFormat(QImage::Format_Grayscale16), temp+"scaledFixed.raw");
            break;
        case GREEN: //save green channel;
            saveMHDcc(fixList[1].convertToFormat(QImage::Format_Grayscale16), temp+"scaledFixed.raw");
            break;
        case BLUE: //save blue channel ;
            saveMHDcc(fixList[2].convertToFormat(QImage::Format_Grayscale16), temp+"scaledFixed.raw");
            break;
        }
    }
//-------------------MOVING IMAGE----------------------
    QImage smov = loadMHD("scaledMoving");
    if (ui->MovingInvertCheckBox->isChecked())
        smov.invertPixels();

    QList <QImage> movList = fux.split(smov);

    if (fouMelActive==true)
    {
        smov=smov.convertToFormat(QImage::Format_Grayscale8);
        saveMHDcc(smov, temp+"MovingFM.raw");
    }

    if(movFormat == 13)
    {
        //Moving image combo box
        switch (ui->MovingChannelBox->currentIndex()+1)
        {
           case AVG: // save gv image;
                smov=smov.convertToFormat(QImage::Format_Grayscale16);
                saveMHDcc(smov, temp+"scaledMoving.raw");
                break;
            case RED: //save red channel;
                saveMHDcc(movList[0].convertToFormat(QImage::Format_Grayscale16), temp+"scaledMoving.raw");
                break;
            case GREEN: //save green channel;
                saveMHDcc(movList[1].convertToFormat(QImage::Format_Grayscale16), temp+"scaledMoving.raw");
                break;
            case BLUE: //save blue channel ;
                saveMHDcc(movList[2].convertToFormat(QImage::Format_Grayscale16), temp+"scaledMoving.raw");
                break;
        }
    }
    accept();
}

QImage colorCheck::loadMHD_RGB888(QString name)
{
    QFile Mhd (temp+name+".mhd");
    long long width =0;
    long long height=0;
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
        }
    }

    QFile raw (temp +name+".raw");
    if (!raw.open(QFile::ReadOnly))
        QMessageBox::warning(this,"","Could  not load raw file");
    else
    {
        QDataStream r(&raw);

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
    }
    return QImage();
}

void colorCheck::saveMHDcc(const QImage& img, const QString& filename)
{
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

QImage colorCheck::loadMHD (QString name)
{
    QFile Mhd (temp+name+".mhd");
    long long width =0;
    long long height=0;
    bool _big = false;
    QString mhdFormat;
    if(!Mhd.open(QFile::ReadOnly))
        QMessageBox::warning(this,"","Could  not load mhd file:" + name);
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

    QFile raw (temp +name+".raw");
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

