#include "tool_functions.h"

QImage loadMHD (QString name)
{
    QFile Mhd (name+".mhd");
    long long width =0;
    long long height=0;
    bool _big = false;
    QString mhdFormat;
    if(Mhd.open(QFile::ReadOnly))
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
    Mhd.close();
    QFile raw (name+".raw");
    const char* pattern[]={"MET_UCHAR","MET_USHORT","MET_FLOAT","MET_UCHAR_ARRAY"};
    int formatId=-1;
    for (int i=0;i<4;++i)
        if (mhdFormat.contains(pattern[i])) formatId=i;
    if(raw.open(QFile::ReadOnly))
    {
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
    }
    return QImage();
}
