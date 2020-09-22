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



gridcalc::gridcalc(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::gridcalc)
{
    ui->setupUi(this);
    loadSettings();
    temp=temp+"/";
    runTransformix = new QProcess();
}

gridcalc::~gridcalc()
{
    delete ui;
}

void gridcalc::read_points()
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
    QImage draw (4000,4000,QImage::Format_RGB888);
    draw.fill(qRgb(255,255,255));
    QPainter pain(&draw);
    QPen pen;
    pen.setWidth(35);
    pen.setColor(qRgb(0,0,0));
    pain.setPen(pen);

    for (int i=0; i<in.size();i++)
        pain.drawPoint(in.at(i));
    pen.setWidth(35);
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

    QPixmap pix;
    pix=pix.fromImage(draw);
    pix=pix.scaledToWidth(500);
    ui->showgridlabel->setPixmap(pix);
    draw.save(temp+"drawPoints.png");


}

void gridcalc::point_list()
// creates a list of points, according to the specified spacing
// output: inputPoints.txt
{
    long gridsize=ui->grsizeBox->value();
    int pointNr = ((4000/gridsize)-1)*((4000/gridsize)-1);
    QFile f(temp+"inputPoints.txt");
    if (f.open(QFile::WriteOnly))
    {
        QTextStream t(&f);

        t << "index" << endl;
        t <<  pointNr << endl;

        for (int y=1; y<(4000/gridsize); y++)
            for (int x=1; x<(4000/gridsize); x++)
            {
                t << x*gridsize << " "<< y*gridsize << endl;
            }

    }
    f.close();
}

void gridcalc::on_closeButton_clicked()
{
    close();
}

void gridcalc::on_makeGridButton_clicked()
// creates a grid according to the specified spacing

{
    long grsize=ui->grsizeBox->value();
    QImage grid(4000, 4000, QImage::Format_Grayscale8);
    grid.fill(Qt::white);
    QPainter pain(&grid);
    QPen stift;
    stift.setWidth(5);
    pain.setPen(stift);
    QLineF hor;
    QLineF vert;

    for (int y=1; y<(grid.height()/grsize); y++)
        {
        hor.setP1(QPointF(0,grsize*y));
        hor.setP2(QPointF(grid.width(),grsize*y));
        pain.drawLine(hor);
        }

    for (int x=1; x<(grid.width()/grsize); x++)
        {
        vert.setP1(QPointF(grsize*x,0));
        vert.setP2(QPointF(grsize*x,grid.height()));
        pain.drawLine(hor);
        pain.drawLine(vert);
        }
    pain.end();

    saveMHDall(grid,temp+"grid.raw");

    QPixmap pixgrid;
    pixgrid.convertFromImage(grid);
    pixgrid=pixgrid.scaledToWidth(500);
    ui->showgridlabel->setPixmap(pixgrid);
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
            t << "ObjectType = Image" << endl;
            t << "NDims = 2" << endl;
            t << "BinaryData = True" << endl;
            t << "BinaryDataByteOrderMSB = False" << endl;
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

void gridcalc::run_transformix(const QString& image)
//runs transformix with an .mhd image as input
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

void gridcalc::run_transformixP(const QString& input)
//runs transformix with a point list as the input
{
    QString const program = "/bin/transformix";
    QStringList arguments;
    arguments << "-def" <<input <<"-out" <<temp <<"-tp" << temp+"newTransformParameters.0.txt";
    runTransformix->setProgram(program);
    runTransformix->setArguments(arguments);
    runTransformix->setProcessChannelMode(QProcess::MergedChannels);
    runTransformix->start(program,arguments);
    runTransformix->waitForStarted(5000);
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

        long fsize = width*height*sizeof(float);
        float* buf=(float*)malloc(fsize);
        uchar* buf8=(uchar*)malloc(width*height);

        r.readRawData((char*)buf,raw.size());


        float _minVal = 650000.0f;
        float _maxVal = -650000.0f;


            for (long long i=0;i<fsize/sizeof (float);++i)
            {
                _minVal = std::min(_minVal,buf[i]);
                _maxVal = std::max(_maxVal,buf[i]);
            }


        for (long long y=0; y<height*width;++y)

             buf8[y]=(buf[y]-_minVal)/(_maxVal-_minVal)*255.0f;

        QImage cross2(buf8,width,height,QImage::Format_Grayscale8);


        raw.close();
        free(buf);

    return cross2;
    }
    return QImage();
}


void gridcalc::on_pointButton_clicked()
// creates an image with points with the specified spacing
// output gridP.mhd/gridP.png
{
    long grsize=ui->grsizeBox->value();
    QImage grid(4000, 4000, QImage::Format_Grayscale16);
    grid.fill(Qt::white);
    int c=1000;
    QPainter pain(&grid);
    QPen stift;
    stift.setWidth(20);
    stift.setColor(qRgba64(c,c,c,65535));
    pain.setPen(stift);

    for (int y=1; y<(grid.height()/grsize); y++)
        for (int x=1; x<(grid.width()/grsize); x++)
        {
            pain.drawPoint((grid.width()/(grid.width()/grsize))*y,(grid.height()/(grid.height()/grsize)*x));
            c+=50;
            stift.setColor(qRgba64(c,c,c,65535));
            pain.setPen(stift);
        }

    pain.end();


    float* buf = (float*)malloc(grid.width()*grid.height()*sizeof(float));
    memset(buf,0,grid.width()*grid.height()*sizeof(float));
    for (long x=0; x<grid.width(); x++)
        for (long y=0; y<grid.height(); y++)
        {
        buf[x+y*grid.width()]=((quint16*)grid.scanLine(y))[x];
        }

    QFile f(temp+"gridP.raw");
    if (f.open(QFile::WriteOnly))
    {
        QDataStream s(&f);
        s.writeRawData((char*)buf,grid.width()*grid.height()*sizeof(float));
        f.close();
    }


    QFileInfo info(temp+"gridP");
    QFile ft(info.absolutePath()+"/"+info.baseName()+".mhd");
    if (ft.open(QFile::WriteOnly))
    {
        QTextStream t(&ft);
        t << "ObjectType = Image" << endl;
        t << "NDims = 2" << endl;
        t << "BinaryData = True" << endl;
        t << "BinaryDataByteOrderMSB = False" << endl;
        t << "DimSize = " <<grid.width() <<" " <<grid.height() << endl;
        t << "ElementSize = 1.0 1.0" << endl;
        t << "ElementType = MET_FLOAT" << endl;
        t << "ElementDataFile = " <<info.fileName() << ".raw" <<endl;
        ft.close();
    }
    grid.save(temp+"gridP.png");
    QImage grid2;
    grid2 = grid.convertToFormat(QImage::Format_Grayscale8);
    saveMHDall(grid2,temp+"grid2.raw");

    QPixmap pixgrid;
    pixgrid.convertFromImage(grid);
    pixgrid=pixgrid.scaledToWidth(500);
    ui->showgridlabel->setPixmap(pixgrid);

    point_list();
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



void gridcalc::on_readPButton_clicked()
{
    read_points();
}

void gridcalc::on_preTriangleButton_clicked()
{
     triangleCalc("pre");
}

void gridcalc::on_postTriangleButton_clicked()
{
     triangleCalc("post");
}

void gridcalc::on_compAreaButton_clicked()
{
    
}

void gridcalc::on_pushButton_clicked()
{
    run_transformixP(temp+"inputPoints.txt");
}
