#ifndef TOOL_FUNCTIONS_H
#define TOOL_FUNCTIONS_H
#include <QImage>
#include <QString>
#include <QFileDialog>
enum mhdFormats
{
    MET_UCHAR = 0,
    MET_USHORT= 1,
    MET_FLOAT = 2,
    MET_UCHAR_ARRAY =3
};

enum dataType
{
    bit32,
    bit16,
    RGB24,
    bit8,
    BIN,
    INVALID
};

QImage loadMHD(QString name);
#endif // TOOL_FUNCTIONS_H
