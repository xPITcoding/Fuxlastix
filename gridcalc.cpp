#include "gridcalc.h"
#include "ui_gridcalc.h"
#include <QImage>
#include <QPainter>
#include <QColor>
#include "fuxlastix.h"
#include <QTextStream>
#include <QMessageBox>
#include <math.h>
#include <QList>
#include <QMap>
#include <QThread>
#include <iostream>
using namespace std;



gridcalc::gridcalc(fuxlastix *ref) :
    QDialog(),
    ui(new Ui::gridcalc)
{  
    ui->setupUi(this);
    pRef = ref;
    loadSettings();
    temp=pRef->resPath;
    runTransformix = new QProcess();
    runTransformix2 = new QProcess();
    connect(runTransformix,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(transformixDone()));
    connect(runTransformix2,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(transformixDone2()));
    connect(ui->pshowColCir, SIGNAL(clicked()),this, SLOT(createColorCircle()));
    ui->transformButton->setDisabled(true);
}

gridcalc::~gridcalc()
{
    delete ui;
}

QImage gridcalc::read_points(QImage input)
// reads the outputpoints.txt file created by Transformix (with point list as input) and creates a comparison image of before and after
// output: drawPoints.png
{
    QList<QPointF> in;
    QList<QPointF> out;
    QList<QPointF> def;

    QFile f(temp+"outputpoints.txt");
    if (f.open(QFile::ReadOnly))
    {
        QTextStream t(&f);
        while(!t.atEnd())
        {
            QString line;
            line = t.readLine();

            QPoint inP;
            QPoint outP;
            QPointF vec;
                QString s=line.section(";",1,1).section("[",1,1).simplified();
                inP.setX(s.section(" ",0,0).toFloat());
                inP.setY(s.section(" ",1,1).toFloat());
                in.append(inP);
                QString s1=line.section(";",3,3).section("[",1,1).simplified();
                outP.setX(s1.section(" ",0,0).toFloat());
                outP.setY(s1.section(" ",1,1).toFloat());
                out.append(outP);
                QString s2=line.section(";",5,5).section("[",1,1).simplified();
                vec.setX(s2.section(" ",0,0).toFloat());
                vec.setY(s2.section(" ",1,1).toFloat());
                def.append(vec);

        }
    }
    QList<QPointF> end;
    float x,y=0;
    for (int i=0; i<in.size();i++)
    {
        x= -(def.at(i).x()*10);
        y= -(def.at(i).y()*10);
        end.append(QPointF(in.at(i).x()+x,in.at(i).y()+y));
    }
    QImage draw (input.width(),input.height(),QImage::Format_RGB888);
    draw.fill(qRgb(255,255,255));
    QPainter pain(&draw);
    QPen pen;
    pen.setWidth(20);
    pen.setColor(qRgb(0,0,0));
    pain.setPen(pen);

    for (int i=0; i<in.size();i++)
        pain.drawPoint(in.at(i));
    pen.setWidth(20);
    pen.setColor(qRgb(255,69,0));
    pain.setPen(pen);
    for (int i=0; i<out.size();i++)
        pain.drawPoint(end.at(i));

    pen.setWidth(10);
    pen.setColor(qRgb(255,0,0));
    pain.setPen(pen);

    for (int i=0; i<def.size();i++)
        pain.drawLine(in.at(i),end.at(i));

    pain.end();

    return draw;

}

void gridcalc::point_list(QImage input)
// creates a list of points, according to the specified spacing
// output: inputPoints.txt
{
    long gridsize=ui->grsizeBox->value();
    int pointNr = ((input.width()/gridsize)-1)*((input.height()/gridsize)-1);
    QFile f(temp+"inputPoints.txt");
    if (f.open(QFile::WriteOnly))
    {
        QTextStream t(&f);

        t << "index" << Qt::endl;
        t <<  pointNr << Qt::endl;

        for (int y=1; y<(input.height()/gridsize); y++)
            for (int x=1; x<(input.width()/gridsize); x++)
            {
                t << input.width()%gridsize/2+gridsize*x << " "<< input.height()%gridsize/2+gridsize*y << Qt::endl;
            }

    }
    f.close();
}

void gridcalc::on_closeButton_clicked()
{
    close();
}

void gridcalc::DispPixie(QImage Input)
{
    QPixmap bild;
    bild.convertFromImage(Input);
    if (bild.width()<bild.height())
    {
        bild=bild.scaledToWidth(350);
        ui->showgridlabel->setPixmap(bild);
    }
    else
    {
        bild=bild.scaledToWidth(430);
        ui->showgridlabel_2->setPixmap(bild);
    }
}

void gridcalc::DispColorWheel(QImage Input)
{
    QPixmap bild;
    bild.convertFromImage(Input);
    bild=bild.scaledToWidth(150);
    ui->showcolorcircle->setPixmap(bild);
}

QImage gridcalc::DrawPoints(QImage Input)
// creates an image with points with the specified spacing
{
    long grsize=ui->grsizeBox->value();
    QImage grid(Input.width(), Input.height(), QImage::Format_Grayscale8);
    grid.fill(Qt::white);
    QPainter pain(&grid);
    QPen stift;
    stift.setWidth(30);
    stift.setColor(Qt::black);
    pain.setPen(stift);

    for (int y=1; y<(grid.height()/grsize); y++)
        for (int x=1; x<(grid.width()/grsize); x++)
        {
            pain.drawPoint((grid.width()%grsize/2+grsize*(x)),(grid.height()%grsize/2+grsize*(y)));
        }
    pain.end();
    DispPixie(grid);
    grid.save(temp+"points.png");
    return grid;
}

QImage gridcalc::DrawGrid(QImage Input)
{
    long grsize=ui->grsizeBox->value();
    QImage grid(Input.width(), Input.height(), QImage::Format_Grayscale8);
    grid.fill(Qt::white);
    QPainter pain(&grid);
    QPen stift;
    stift.setWidth(10);
    pain.setPen(stift);
    QLineF hor;
    QLineF vert;

    for (int y=1; y<(grid.height()/grsize); y++)
        {
        hor.setP1(QPointF(0,(grid.height()%grsize/2+grsize*(y))));
        hor.setP2(QPointF(grid.width(),(grid.height()%grsize/2+grsize*(y))));
        pain.drawLine(hor);
        }

    for (int x=1; x<(grid.width()/grsize); x++)
        {
        vert.setP1(QPointF((grid.width()%grsize/2+grsize*(x)),0));
        vert.setP2(QPointF((grid.width()%grsize/2+grsize*(x)),grid.height()));
        pain.drawLine(hor);
        pain.drawLine(vert);
        }
    pain.end();
    grid.save(temp+"grid.png");
    saveMHDall(grid,temp+"grid.raw");
    DispPixie(grid);
    return grid;
}

QImage gridcalc::makeBinary(QImage Input)
{
    QImage Output(Input.width(), Input.height(), QImage::Format_Grayscale8);
    Output.fill(Qt::white);
    Input = Input.convertToFormat(QImage::Format_Grayscale8);
    for (int x=0; x<Input.width(); x++)
        for (int y=0; y<Input.height(); y++)
        {
            int gv = qGray(Input.pixel(x,y));
            if (gv<100)
                Output.setPixel(x,y,Qt::black);
        }

    return Output;
}

QImage gridcalc::MergeGrid(QImage Img, QImage Grid)
{
    bool colorInput=true;
    if(Grid.format()==QImage::Format_Grayscale8)
        colorInput=false;

     QImage Output(Img.width(), Img.height(), QImage::Format_RGB888);
     Img = Img.convertToFormat(QImage::Format_Grayscale8);

     for (int x=0; x<Img.width(); x++)
        for (int y=0; y<Img.height(); y++)
        {
            if (Grid.pixel(x,y)!= qRgb(255,255,255))
                if(colorInput)
                    Output.setPixel(x,y,Grid.pixel(x,y));
                else
                    Output.setPixel(x,y,qRgb(255,0,0));
            else
            {
                int val=qGray(Img.pixel(x,y));
                Output.setPixel(x,y,qRgb(val,val,val));
            }
        }
     DispPixie(Output);
     return Output;
}

void gridcalc::on_pointButton_clicked()
{
    QImage final = loadMHD(temp+"finalImage");
    if(ui->SliderMode->value()==0)
    {
        QImage Grid = DrawPoints(final);
        if (ui->CBDisplayPicture->isChecked())
           QImage Overlay = MergeGrid(final,Grid);
        point_list(final);
    }
    else
    {
        QImage Grid = DrawGrid(final);
        if (ui->CBDisplayPicture->isChecked())
            QImage Overlay = MergeGrid(final,Grid);
    }
    ui->transformButton->setDisabled(false);
}

void gridcalc::transformixDone()
{
    //QMessageBox::warning(0,"done","transformix done");
    if(ui->SliderMode->value()==0)
    {
        QImage final = loadMHD(temp+"finalImage");
        QImage draw = read_points(final);
        if(ui->CBDisplayPicture->isChecked())
            draw=MergeGrid(final,draw);
        DispPixie(draw);
        draw.save(temp+"TransformedPoints.png");
    }
    else
    {
        QImage final = loadMHD(temp+"finalImage");
        QImage draw = loadMHD(temp+"result");
        draw=makeBinary(draw);
        if(ui->CBDisplayPicture->isChecked())
            draw=MergeGrid(final,draw);
        DispPixie(draw);
        draw.save(temp+"TranformedGrid.png");
    }
}

void gridcalc::transformixDone2()
// takes a pointgrid with spacing 10 transformed by transformix and creates a color-coded picture of the displacement-vectors
{
    ui->textBrowser->append(QString("Transformix Done"));
    qApp->processEvents();
    QImage pic = loadMHD(temp+"finalImage");
    int wid = pic.width();
    int pwid;
    if (wid%10==0)
         pwid = wid/10+1;
    else
         pwid = floor(wid/10)+2;
    int heig = pic.height();
    int tot = wid*heig;
    QList<QColor> spectrum = createColorCircle();
    QList<QList<QPointF>> list = CreateDeformationList();
    QImage output(wid,heig,QImage::Format_RGBA8888);
    output.fill(Qt::white);
    QPointF trafo;
    float maxi = getmaxmagnitude(list[1]);
    QPointF p1, p2, p3, p4;
    QPointF t1, t2, t3, t4;
    float xi, yi, d1, d2, d3, d4, dtot;
    int fx, cx, fy, cy;
    int per;
    for (int i=0; i<tot; i++)
    {
        xi = i%wid;
        yi = floor(i/wid);

        if((int)xi%10 == 0 && (int)yi%10 == 0)
           trafo = list[1][yi+pwid+xi];
        else
        {
            fx = floor(xi/10)*10;
            cx = ceil(xi/10)*10;
            fy = floor(yi/10)*10*pwid;
            cy = ceil(yi/10)*10*pwid;

            p1 = list[0][(fy+fx)/10];
            t1 = list[1][(fy+fx)/10];
            p2 = list[0][(fy+cx)/10];
            t2 = list[1][(fy+cx)/10];
            p3 = list[0][(cy+fx)/10];
            t3 = list[1][(cy+fx)/10];
            p4 = list[0][(cy+cx)/10];
            t4 = list[1][(cy+cx)/10];

            d1 = 1.0f/(sqrt(pow(xi-p1.x(),2)+pow(yi-p1.y(),2))+1);
            d2 = 1.0f/(sqrt(pow(xi-p2.x(),2)+pow(yi-p2.y(),2))+1);
            d3 = 1.0f/(sqrt(pow(xi-p3.x(),2)+pow(yi-p3.y(),2))+1);
            d4 = 1.0f/(sqrt(pow(xi-p4.x(),2)+pow(yi-p4.y(),2))+1);
            dtot = d1+d2+d3+d4;
        }
        if(i%(tot/10)==0)
        {
            per = ((float)i/(float)tot)*100;
            ui->textBrowser->append(QString("%1 % done").arg(per));
            qApp->processEvents();
        }
        trafo = (t1*d1+t2*d2+t3*d3+t4*d4)/dtot;
        output.setPixelColor(xi,yi,getRGBA(trafo, spectrum, maxi));
    }
    ui->textBrowser->append(QString("Finished"));
    DispPixie(output);
    output.save(temp+"DirectionImage.png");
}

void gridcalc::saveMHDall(const QImage& img, const QString& filename)
// saves a QImage as an .mhd file
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
            t << "ObjectType = Image" << Qt::endl;
            t << "NDims = 2" << Qt::endl;
            t << "BinaryData = True" << Qt::endl;
            t << "BinaryDataByteOrderMSB = False" << Qt::endl;
            t << "DimSize = " <<img.width() <<" " <<img.height() << Qt::endl;
            t << "ElementSize = 1.0 1.0" << Qt::endl;
            t << "ElementType = MET_USHORT" << Qt::endl;
            t << "ElementDataFile = " <<info.fileName() <<Qt::endl;
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
            t << "ElementSize = 0.1 0.1" << Qt::endl;
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
                t << "ElementSize = 0.1 0.1" << Qt::endl;
                t << "ElementNumberOfChannels = 3" << Qt::endl;
                t << "ElementType = MET_UCHAR_ARRAY" << Qt::endl;
                t << "ElementDataFile = " <<info.fileName() <<Qt::endl;
                ft.close();
            }; break;

        }
    }

}

void gridcalc::run_transformix(const QString& image)
//runs transformix with an .mhd image as input
{
    QString const program = pRef->transformixpath;
    QStringList arguments;
    arguments << "-in" <<image <<"-tp" <<temp+"newTransformParameters.0.txt" <<"-out" <<temp;
    runTransformix->setProgram(program);
    runTransformix->setArguments(arguments);
    runTransformix->setProcessChannelMode(QProcess::MergedChannels);
    runTransformix->start(program,arguments);
    runTransformix->waitForStarted(5000);
}

void gridcalc::run_transformixP(const QString& input)
//runs transformix with a point list as the input
{
    QString const program = pRef->transformixpath;
    QStringList arguments;
    arguments << "-def" <<input <<"-out" <<temp <<"-tp" << temp+"newTransformParameters.0.txt";
    runTransformix->setProgram(program);
    runTransformix->setArguments(arguments);
    runTransformix->setProcessChannelMode(QProcess::MergedChannels);
    runTransformix->start(program,arguments);
    runTransformix->waitForStarted(5000);
}

void gridcalc::run_transformixP2(const QString& input)
//runs transformix with a point list as the input -> triggers transformixDone2 -> only for trafo-field visualisation
{
    QString const program = pRef->transformixpath;
    QStringList arguments;
    arguments << "-def" <<input <<"-out" <<temp <<"-tp" << temp+"newTransformParameters.0.txt";
    runTransformix2->setProgram(program);
    runTransformix2->setArguments(arguments);
    runTransformix2->setProcessChannelMode(QProcess::MergedChannels);
    runTransformix2->start(program,arguments);
    runTransformix2->waitForStarted(5000);
}

void gridcalc::loadSettings()
{
    QString tt;
    QFile f("elastix_gui.txt");
    if (f.open(QFile::ReadOnly))
    {
        QTextStream t(&f);
        for (int x=0; x<3; x++)
         tt=t.readLine();
     }
        temp=tt;
        f.close();
}

QImage gridcalc::loadMHD(QString name)
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

QImage gridcalc::loadJacobianMHD(QString name)
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
        QDataStream r(&raw);

        //32bit grayvalue;
        float _minGVal,_maxGVal, maxNegative, maxPositive;
        float bufsize= width*height*sizeof(float);
        QImage out(width, height, QImage::Format_RGBA8888);

        float* buf=(float*)malloc(bufsize);
        r.readRawData((char*)buf,raw.size());

        _minGVal=_maxGVal=buf[0];

        for (long x=0;x<width*height;++x)
        {
            _minGVal = std::min(_minGVal,buf[x]);
            _maxGVal = std::max(_maxGVal,buf[x]);
        }
        maxNegative= 1.0f-_minGVal;
        maxPositive= _maxGVal-1.0f;
        float maxmax=std::max(maxNegative,maxPositive);
        float val;
        for(long y=0;y<out.height();y++)
            for (long x=0;x<out.width();x++)
        {
            val=buf[x+y*out.width()];
            out.setPixel(x,y,qRgba(std::max((val-1.0f)/maxmax*255.0f,0.0f),0,std::max(-(val-1.0f)/maxmax*255.0f,0.0f),fabs(val-1.0f)/maxmax*255.0f));
        }
            raw.close();
            free(buf);
            return out;
       }
}

QList<int> gridcalc::getWH(QString name)
// gets with and height of and .mhd file
// returns a QList with w and h
{
    QFile Mhd (temp+name+".mhd");
    int width =0;
    int height=0;
    QList<int>list;
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
               list.append(width);
               list.append(height);
            }
        }
    }
    return list;
}

void gridcalc::triangleCalc(QString arg)
{
    QList<QPointF> inPoints;
    int row=0;
    if (arg=="pre")
    {
        int lc=0;
        QFile f(temp+"inputPoints.txt");
        if (f.open(QFile::ReadOnly))
        {
            QTextStream t(&f);
            while(!t.atEnd())
            {
                QString line;
                line = t.readLine();
                lc++;
                if (lc==2)
                {
                    row= line.toInt();
                    row=sqrt(row);
                }
                if (lc>2)
                {
                 QString x=line.section(" ",0,0);
                 QString y=line.section(" ",1,1);

                 inPoints.append(QPointF(x.toFloat(),y.toFloat()));
                }
            }
            for (int i=0; i<inPoints.size(); i++)
            {
            ui->textBrowser->append(QString("X"+QString("%1").arg(i)+": "+QString("%1").arg(inPoints.at(i).x())+"  Y"+QString("%1").arg(i)+ ": "+QString("%1").arg(inPoints.at(i).y())));
            }
        }
        f.close();
    }

    if (arg=="post")
    {
        QList<QPointF> in;
        QList<QPointF> def;

        QFile f(temp+"outputpoints.txt");
        if (f.open(QFile::ReadOnly))
        {
            QTextStream t(&f);
            while(!t.atEnd())
            {
                QString line;
                line = t.readLine();

                QPoint inP;
                QPointF vec;
                    QString s=line.section(";",1,1).section("[",1,1).simplified();

                    inP.setX(s.section(" ",0,0).toFloat());
                    inP.setY(s.section(" ",1,1).toFloat());
                    in.append(inP);

                    QString s2=line.section(";",5,5).section("[",1,1).simplified();
                    vec.setX(s2.section(" ",0,0).toFloat());
                    vec.setY(s2.section(" ",1,1).toFloat());
                    def.append(vec);
                    row++;
            }
        }
        row=sqrt(row);
        float x,y=0;
        for (int i=0; i<in.size();i++)
        {
            x= -(def.at(i).x()*10);
            y= -(def.at(i).y()*10);
            ui->textBrowser->append(QString("X"+QString("%1").arg(i)+": "+QString("%1").arg(in.at(i).x()+x)+"  Y"+QString("%1").arg(i)+ ": "+QString("%1").arg(in.at(i).y()+y)));
            inPoints.append(QPointF(in.at(i).x()+x,in.at(i).y()+y));
        }
    }


    double area=0, qubuf=0, buf=0, tric=0;

    for(int r=0; r<row-1;r++)
        for(int c=0; c<row-1;c++)
        {
           QPointF p1(inPoints.at(c+1+r*row).x()-inPoints.at(c+r*row).x(),inPoints.at(c+1+r*row).y()-inPoints.at(c+r*row).y());
           QPointF p2(inPoints.at(c+row+r*row).x()-inPoints.at(c).x(),inPoints.at(c+row+r*row).y()-inPoints.at(c+r*row).y());
           area=(fabs(p1.x()*p2.y()-p2.x()*p1.y()))/2;
           qubuf+=area*area;
           buf+=area;
           tric++;

           QPointF p3(inPoints.at(c+1+r*row).x()-inPoints.at(c+1+row+r*row).x(),inPoints.at(c+1+r*row).y()-inPoints.at(c+1+row+r*row).y());
           QPointF p4(inPoints.at(c+row+r*row).x()-inPoints.at(c+1+row+r*row).x(),inPoints.at(c+row+r*row).y()-inPoints.at(c+1+row+r*row).y());

           area=(fabs(p3.x()*p4.y()-p4.x()*p3.y()))/2;
           qubuf+=area*area;
           buf+=area;
           tric++;
        }
    double stabW = sqrt(qubuf/tric-pow(buf/tric,2.0));

    if (arg == "pre")
    {
        ui->textBrowser->append("pre Δ area:  "+QString("%1").arg(buf,0,'f',0));
        ui->textBrowser->append("pre Δ stddev:  "+QString("%1").arg(stabW,0,'f',0));
    }
    else
    {
        ui->textBrowser->append("post Δ area:  "+QString("%1").arg(buf,0,'f',0));
        ui->textBrowser->append("post Δ stddev:  "+QString("%1").arg(stabW,0,'f',0));
    }
}

/*
void gridcalc::on_readPButton_clicked()
{

}

void gridcalc::on_preTriangleButton_clicked()
{
     triangleCalc("pre");
}

void gridcalc::on_postTriangleButton_clicked()
{
     triangleCalc("post");
}
*/

void gridcalc::on_transformButton_clicked()
{
    if(ui->SliderMode->value()==0)
        run_transformixP(temp+"inputPoints.txt");
    else
        run_transformix(temp+"grid.mhd");
}

void gridcalc::on_jacobianButton_clicked()
{
    if(ui->CBDisplayPicture->isChecked())
    {
        QImage jacobi = loadJacobianMHD(temp+"spatialJacobian");
        QImage original = loadMHD(temp+"finalImage");
        original = original.convertToFormat(QImage::Format_Grayscale8);
        original = original.convertToFormat(QImage::Format_RGB888);
        QPainter pain(&original);
        pain.drawImage(QPoint(0,0),jacobi);
        DispPixie(original);
    }
    else
    {
        QImage jacobi = loadJacobianMHD(temp+"spatialJacobian");
        jacobi.save(temp+"jacobi.png");
        DispPixie(jacobi);
    }
    //USE QPAINTER COMPOSITION MODE!!!!!
}

QList<QColor> gridcalc::createColorCircle()
// creates a list of 30 QColors colours that form a color wheel
{
    QList<QColor> list;
    QList<int> triplett;
    triplett.append(255);
    triplett.append(0);
    triplett.append(0);

    int active =2;
    int mode=0;


    list.append(QColor(triplett[0],triplett[1],triplett[2],255));
    for (int i=0; i<29; i++)
    {
        if(triplett[0]+triplett[1]+triplett[2]==510 || triplett[0]+triplett[1]+triplett[2]==255)
        {
            mode+=1;
            if (mode>1)
                mode=0;

            active-=1;
            if (active<0)
                active=2;
        }

        if(mode==1)
            triplett[active]+=51;
        else
            triplett[active]-=51;

        list.append(qRgb(triplett[0],triplett[1],triplett[2]));
    }

    list.append(QColor(0,0,0,255));
    return list;
}

void gridcalc::CreateAllPoints(QImage input)
// creates a point grid with spacing 10 starting at 0|0 and always covering all edges
{
    long long pointNr;
    if (input.height()%10==0 && input.width()%10==0)
         pointNr = (input.width()/10+1)*(input.height()/10+1);
    else if (input.height()%10!=0 && input.width()%10!=0)
        pointNr = (input.width()/10+2)*(input.height()/10+2);
    else if (input.height()%10!=0)
        pointNr = (input.width()/10+1)*(input.height()/10+2);
    else
        pointNr = (input.width()/10+2)*(input.height()/10+1);

    QFile f(temp+"inputPoints.txt");
    if (f.open(QFile::WriteOnly))
    {
        QTextStream t(&f);

        t << "index" << Qt::endl;
        t <<  pointNr << Qt::endl;

        for (long y=0; y<input.height(); y+=10)
        {
            for (long x=0; x<input.width(); x+=10)
           {
              t << x << " "<< y << Qt::endl;
           }
           t << (input.width()-1) << " "<< y << Qt::endl;
        }
        for (long x=0; x<input.width(); x+=10)
        {
            t << x << " "<< (input.height()-1) << Qt::endl;
        }
        t << (input.width()-1) << " "<< (input.height()-1) << Qt::endl;
    }
    f.close();
}

QList<QList<QPointF>> gridcalc::CreateDeformationList()
{
    QList<QPointF> in;
    QList<QPointF> out;
    QList<QPointF> def;

    QFile f(temp+"outputpoints.txt");
    if (f.open(QFile::ReadOnly))
    {
        QTextStream t(&f);
        while(!t.atEnd())
        {
            QString line;
            line = t.readLine();

            QPoint inP;
            QPoint outP;
            QPointF vec;
                QString s=line.section(";",1,1).section("[",1,1).simplified();

                inP.setX(s.section(" ",0,0).toFloat());
                inP.setY(s.section(" ",1,1).toFloat());
                in.append(inP);
                QString s1=line.section(";",3,3).section("[",1,1).simplified();
                outP.setX(s1.section(" ",0,0).toFloat());
                outP.setY(s1.section(" ",1,1).toFloat());
                out.append(outP);
                QString s2=line.section(";",5,5).section("[",1,1).simplified();
                vec.setX(s2.section(" ",0,0).toFloat());
                vec.setY(s2.section(" ",1,1).toFloat());
                def.append(vec);

        }
    }
    f.close();
    QList<QList<QPointF>> returnlist;
    returnlist.append(in);
    returnlist.append(def);

    return returnlist;
}

float gridcalc::getmaxmagnitude(QList<QPointF> input)
{
    float maximum = sqrt(pow(input.at(0).x(),2)+pow(input.at(0).y(),2));
    float buffer;
    for (long long i=0;i<input.size();i++)
    {
        buffer = sqrt(pow(input.at(i).x(),2)+pow(input.at(i).y(),2));
        maximum = max(buffer,maximum);
    }
    return maximum;
}

QColor gridcalc::getRGBA(QPointF input, QList<QColor> spectrum, float maxi)
// translates a 2D vector into direction+magnitude encoding color
{
    QList<int> colors;
    float value = 0.0f;
    float PI = 3.14159265;
    float alpha = 0.0f;
    if (input.x()>0.0f && input.y()>0.0f)
        value = floor((90.0f-(atan(fabs(input.y())/input.x())*180.0f/PI))/12.0f);

    else if (input.x()<0.0f && input.y()<0.0f)
        value = floor((270.0f-(atan(fabs(input.y()/input.x()))*180.0f/PI))/12.0f);

    else if (input.x()<0.0f)
        value = floor((270.0f+(atan(fabs(input.y()/input.x()))*180.0f/PI))/12.0f);

    else if (input.y()<0.0f)
        value = floor((90.0f+(atan(fabs(input.y()/input.x()))*180.0f/PI))/12.0f);

    else
        value = 30;

     //= spectrum(value)
    alpha = 255*sqrt(pow(input.x(),2)+pow(input.y(),2))/maxi;
    QColor output;
    output = QColor(spectrum.at(value));
    output.setAlpha(alpha);
    return output;
}

QImage gridcalc::drawColorWheelRound(QList<QColor> spectrum)
// creates a picture of a color wheel from a list of 30 QColors
{
    QImage output(150,150,QImage::Format_RGB888);
    output.fill(Qt::white);
    QPainter pain(&output);
    QRectF rectangle(0.0f,0.0f,150.0f,150.0f);
    int startAngle, spanAngle;
    for (int i=0; i<30; i++)
    {
        pain.setBrush(spectrum.at(29-i));
        pain.setPen(spectrum.at(29-i));
        startAngle = 16*(90+i*12);
        spanAngle = 16*12;
        pain.drawPie(rectangle,startAngle,spanAngle);
    }
    return output;
}

void gridcalc::on_TrafoPicButton_clicked()
{
    QImage pic = loadMHD(temp+"finalImage");
    CreateAllPoints(pic);
    run_transformixP2(temp+"inputPoints.txt");
}

void gridcalc::on_pshowColCir_clicked()
{
    QList<QColor> Colors = createColorCircle();
    QImage output = drawColorWheelRound(Colors);
    DispColorWheel(output);
}
