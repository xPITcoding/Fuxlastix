#include "foumel.h"
#include "ui_foumel.h"
#include <QMessageBox>
#include <stdlib.h>


using namespace std;
using namespace alglib;

fouMel::fouMel(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fouMel)
{
    ui->setupUi(this);
}

fouMel::~fouMel()
{
    delete ui;
}


QMatrix3x3 fouMel::dyad (XYZ v1, XYZ v2)
{
    QMatrix3x3 diad;
    diad(0,0) = v1.x*v2.x;
    diad(1,0) = v1.y*v2.x;
    diad(2,0) = v1.z*v2.x;
    diad(0,1) = v1.x*v2.y;
    diad(0,2) = v1.x*v2.z;
    diad(2,0) = v1.z*v2.x;
    diad(2,1) = v1.z*v2.y;
    diad(1,2) = v1.y*v2.z;
    diad(2,2) = v1.z*v2.z;
    return diad;
}

complex_2d_array* fouMel::makeFFT(QImage src)
{
    QVector <complex_1d_array> vec_fx;
    QVector <complex_1d_array> vec_fy;

    for (long ly=0;ly<src.height();++ly)
    {
        complex_1d_array fx;
        real_1d_array x;
        x.setlength(src.width());
        for (long lx=0;lx<src.width();++lx)
            x[lx] = ((unsigned short*)src.constBits())[lx+ly*src.width()];
        fftr1d(x, src.width(), fx);
        vec_fx.append(fx);
    }

    for (long lx=0;lx<src.width();++lx)
    {
        complex_1d_array x;
        xparams xparam;
        x.setlength(src.height());
        for (long ly=0;ly<src.height();++ly)
            x[ly] = vec_fx[ly][lx];
        fftc1d(x, src.height(), xparam);
        vec_fy.append(x);
    }

    complex_2d_array* buffer = new complex_2d_array;
    buffer->setlength(src.height(),src.width());
    for (long lx=0;lx<src.width();++lx)
        for (long ly=0;ly<src.height();++ly)
            (*buffer)[ly][lx]=vec_fy[ly][lx];

    return buffer;
}

real_2d_array* fouMel::invFFT(complex_2d_array* data,const long& width,const long& height)
{
    real_2d_array* buffer = new real_2d_array;
    buffer->setlength(width,height);
    for (long lx=0;lx<width;++lx) {
        complex_1d_array column;
        column.setlength(height);
        for (long ly=0;ly<height;++ly)
            column(ly)=(*data)(lx,ly);
        fftc1dinv(column,height);

        for (long ly=0;ly<height;++ly )
            (*data)(lx,ly)=column(ly);
    }

    for (long ly=0;ly<height;++ly ) {
        complex_1d_array row;
        row.setlength(height);
        for (long lx=0;lx<width;++lx)
            row(lx)=(*data)(lx,ly);
        fftc1dinv(row,width);

        for (long lx=0;lx<width;++lx )
            (*data)(lx,ly)=row(lx);
    }

    for (long lx=0;lx<width;++lx ) {
        for (long ly=0;ly<height;++ly ) {
            (*buffer)(lx,ly)=abscomplex((*data)(lx,ly));
        }
    }

    return buffer;
}

QImage fouMel::flipImg (QImage in)
{
    QImage out(in.width(),in.height(),QImage::Format_Grayscale16);
    out = in.mirrored(true,true);
    return out;
}

QImage fouMel::createImg (real_2d_array *data, const long& width, const long& height, long& max_pos_x, long& max_pos_y, long& offsetx, long& offsety)
{

    unsigned short* buffer = (unsigned short*)malloc(width*height*sizeof(unsigned short));
    memset(buffer,0,width*height*sizeof(short));

    // get min max
    float _minGVal = (*data)(0,0);
    float _maxGVal = (*data)(0,0);

    max_pos_x = 0;
    max_pos_y = 0;

    long cx = width / 2;
    long cy = height / 2;


    for (long ly=0;ly<height;++ly) {
        for (long lx=0;lx<width;++lx) {
            _minGVal = std::min(_minGVal,(float)(*data)(lx,ly));

            if ((float)(*data)(lx,ly)> _maxGVal)
            {
                max_pos_x = lx;
                max_pos_y = ly;
                _maxGVal = (*data)(lx,ly);
            }
        }
    }

    for (long ly=0;ly<height;++ly) {
        for (long lx=0;lx<width;++lx) {
            buffer[((lx+cx) % width) + ((ly+cy) % height)*width] = ((float)(*data)(lx,ly)-_minGVal)/(_maxGVal-_minGVal)*65535.0f;
        }
    }
    QImage res((uchar*)buffer,width,height,QImage::Format_Grayscale16);
    /*long maxres=0;
    long posx=0;
    long posy=0;
    for (long i=0; i<width; i++)
        for (long j=0; j<height; j++)
            if(buffer[i+j*width]>maxres)
            {
                posx=i;
                posy=j;
                maxres=buffer[i+j*width];
            }
    */
    max_pos_x>width/2 ? offsetx = max_pos_x-width : offsetx = max_pos_x;
    max_pos_y>height/2 ? offsety = max_pos_y-height  : offsety = max_pos_y;
    cout << "offset from centre x " <<offsetx <<endl;
    cout << "offset  from centre y " << offsety <<endl;
    //cout <<"new x" <<posx <<endl;
    //cout << "new y" <<posy <<endl;
    cout << "christians max" <<_maxGVal <<endl;
    //cout << "maxres " << maxres <<endl;
    cout <<"max x " <<max_pos_x <<endl;
    cout <<"max y " <<max_pos_y << endl;

    return res;
}

QImage fouMel::polarImage(const QImage& src,const int& width,const int& height)

{
    unsigned short* buffer = (unsigned short*)malloc(width*height*sizeof(unsigned short));
    memset(buffer,0,width*height*sizeof(unsigned short));
    float rr = src.width();
    float cc = src.height();

    // generate a width x height grid and calculate the scaling factors
    float r = width;
    float c = height;
    float fac_r = rr/r;
    float fac_c = cc/c;
    // center
    float MR = r/2.0f;
    float MC = c/2.0f;
    float max_radius = std::min(MC*.9f,MR*.9f);
    float x1,x2,y1,y2,radius1,radius2,angle1,angle2;
    float total_weight;
    float total_sum_I;
    float weight;

    // calculate the value for each pixel in the polar grid
    for (long lr=1;lr<r;++lr) {
            for (long lc=1;lc<c;++lc) {
            // calculate the position of the pixel in the cartesian grid
            angle1 = 2.0f*3.14159f/(float)(c-1)*float(lc);
            radius1 = ((float)(lr)/(float)(r-1))*max_radius;

            x1 = (MC+cos(angle1)*radius1)*fac_c;
            y1 = (MR+sin(angle1)*radius1)*fac_r;

           // perform a simple linear interpolation
            total_weight = 0.0f;
            total_sum_I = 0.0f;
            // respect the size of the cartesian image
            if (x1>1 && x1<(cc-1) && y1>1 && y1<(rr-1))
            {
                for (long lly=floor(y1);lly<ceil(y1)+1;++lly)
                    for (long llx=floor(x1);llx<ceil(x1)+1;++llx)
                    {
                        weight = sqrt(pow(llx-x1,2)+ pow(lly-y1,2));
                        total_weight += 1.0f/weight;
                        total_sum_I += ((float)((unsigned short*)src.constBits())[(long)(llx+lly*rr)])/weight;
                    }

                // assign the interpolated value to the polar grid
                if (total_weight>0)
                    buffer[(long)(lr+lc*r)]=total_sum_I/total_weight;
              }
        }
    }

    float _minVal=buffer[0];
    float _maxVal=buffer[0];

    for (long x=0;x<width;++x) {
        for (long y = 0; y < height; ++y) {
            _minVal = std::min(_minVal,(float)buffer[x+y*width]);
            _maxVal = std::max(_maxVal,(float)buffer[x+y*width]);
        }
    }



    for (long x=0;x<width;++x) {

        for (long y = 0; y < height; ++y) {

            buffer[x+y*width]=((float)buffer[x+y*width]-_minVal)/(_maxVal-_minVal)*65535.0f;
        }
    }

    QImage res((uchar*)buffer,width,height,QImage::Format_Grayscale16);
    return res;
}

QImage fouMel::Corr(QImage in1, QImage in2, long& mx, long& my, long& ofx, long& ofy)
{
    int w=min(in1.width(),in2.width());
    int h=min(in1.height(),in2.height());
    w % 2==1 ? w-- : w=w;
    h % 2==1 ? h-- : h=h;
    in1=in1.copy(0,0,w,h);
    in2=in2.copy(0,0,w,h);

    complex_2d_array* fft1 =new complex_2d_array;
    fft1 = makeFFT(in1);
    QImage flip = flipImg(in2);
    complex_2d_array* fft2 =new complex_2d_array;
    fft2 = makeFFT(flip);

    for (int i=0; i<fft1->rows();i++)
        for (int j=0; j<fft1->cols();j++)
        {
            (*fft1)[i][j]*=(*fft2)[i][j];
        }
    real_2d_array *result;
    result = invFFT(fft1,fft1->rows(),fft1->cols());


    QImage out = createImg(result,fft1->cols(),fft1->rows(),mx,my, ofx, ofy); // autocorr

    cout <<"corr x "<< mx <<endl;
    cout << "corr y " <<my <<endl;
    cout <<"end function" <<endl;
    return out;
}

