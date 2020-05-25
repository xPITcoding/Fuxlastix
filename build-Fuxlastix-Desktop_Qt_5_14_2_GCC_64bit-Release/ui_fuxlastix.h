/********************************************************************************
** Form generated from reading UI file 'fuxlastix.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FUXLASTIX_H
#define UI_FUXLASTIX_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_fuxlastix
{
public:
    QLabel *ctlabel;
    QTextBrowser *textBrowser;
    QPushButton *pushRunButton;
    QPushButton *pushQuitButton;
    QLabel *xpitlabel;
    QLabel *fuxlabel;
    QLabel *label_3;
    QLabel *histolabel;
    QLabel *label;
    QWidget *widget;
    QGridLayout *gridLayout;
    QLabel *labelFixedImage;
    QLineEdit *lineFixedImage;
    QPushButton *pickFixedImageButton;
    QLabel *labelMovingImage;
    QLineEdit *lineMovingImage;
    QPushButton *pickMovingImageButton;
    QLabel *labelMovingColour;
    QLineEdit *lineMovingColour;
    QPushButton *pickMovingColourButton;
    QLabel *labelOutputdir;
    QLineEdit *lineOutputDir;
    QPushButton *pickDirectoryButton;
    QLabel *labelParameterFile;
    QLineEdit *lineParameterFile;
    QPushButton *pickParameterFileButton;
    QLabel *label_2;
    QLineEdit *lineTemp;
    QPushButton *pickTempButton;
    QWidget *widget1;
    QHBoxLayout *horizontalLayout;
    QPushButton *checkerButton;
    QLabel *label_4;
    QSpinBox *spinBox;
    QPushButton *saveCheckerButton;

    void setupUi(QDialog *fuxlastix)
    {
        if (fuxlastix->objectName().isEmpty())
            fuxlastix->setObjectName(QString::fromUtf8("fuxlastix"));
        fuxlastix->resize(1520, 651);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(fuxlastix->sizePolicy().hasHeightForWidth());
        fuxlastix->setSizePolicy(sizePolicy);
        ctlabel = new QLabel(fuxlastix);
        ctlabel->setObjectName(QString::fromUtf8("ctlabel"));
        ctlabel->setGeometry(QRect(510, 60, 500, 500));
        sizePolicy.setHeightForWidth(ctlabel->sizePolicy().hasHeightForWidth());
        ctlabel->setSizePolicy(sizePolicy);
        textBrowser = new QTextBrowser(fuxlastix);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        textBrowser->setGeometry(QRect(10, 320, 471, 251));
        pushRunButton = new QPushButton(fuxlastix);
        pushRunButton->setObjectName(QString::fromUtf8("pushRunButton"));
        pushRunButton->setGeometry(QRect(320, 620, 80, 24));
        pushQuitButton = new QPushButton(fuxlastix);
        pushQuitButton->setObjectName(QString::fromUtf8("pushQuitButton"));
        pushQuitButton->setGeometry(QRect(400, 620, 80, 24));
        xpitlabel = new QLabel(fuxlastix);
        xpitlabel->setObjectName(QString::fromUtf8("xpitlabel"));
        xpitlabel->setGeometry(QRect(10, 580, 261, 40));
        fuxlabel = new QLabel(fuxlastix);
        fuxlabel->setObjectName(QString::fromUtf8("fuxlabel"));
        fuxlabel->setGeometry(QRect(20, 10, 100, 100));
        label_3 = new QLabel(fuxlastix);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(120, 50, 371, 61));
        QFont font;
        font.setFamily(QString::fromUtf8("Tlwg Typewriter"));
        font.setPointSize(50);
        font.setBold(false);
        font.setItalic(false);
        font.setWeight(50);
        label_3->setFont(font);
        histolabel = new QLabel(fuxlastix);
        histolabel->setObjectName(QString::fromUtf8("histolabel"));
        histolabel->setGeometry(QRect(1000, 60, 500, 500));
        label = new QLabel(fuxlastix);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 630, 221, 16));
        widget = new QWidget(fuxlastix);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(10, 130, 471, 176));
        gridLayout = new QGridLayout(widget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        labelFixedImage = new QLabel(widget);
        labelFixedImage->setObjectName(QString::fromUtf8("labelFixedImage"));

        gridLayout->addWidget(labelFixedImage, 0, 0, 1, 1);

        lineFixedImage = new QLineEdit(widget);
        lineFixedImage->setObjectName(QString::fromUtf8("lineFixedImage"));

        gridLayout->addWidget(lineFixedImage, 0, 1, 1, 1);

        pickFixedImageButton = new QPushButton(widget);
        pickFixedImageButton->setObjectName(QString::fromUtf8("pickFixedImageButton"));
        pickFixedImageButton->setEnabled(true);
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(pickFixedImageButton->sizePolicy().hasHeightForWidth());
        pickFixedImageButton->setSizePolicy(sizePolicy1);
        pickFixedImageButton->setBaseSize(QSize(0, 0));

        gridLayout->addWidget(pickFixedImageButton, 0, 2, 1, 1);

        labelMovingImage = new QLabel(widget);
        labelMovingImage->setObjectName(QString::fromUtf8("labelMovingImage"));

        gridLayout->addWidget(labelMovingImage, 1, 0, 1, 1);

        lineMovingImage = new QLineEdit(widget);
        lineMovingImage->setObjectName(QString::fromUtf8("lineMovingImage"));

        gridLayout->addWidget(lineMovingImage, 1, 1, 1, 1);

        pickMovingImageButton = new QPushButton(widget);
        pickMovingImageButton->setObjectName(QString::fromUtf8("pickMovingImageButton"));
        pickMovingImageButton->setEnabled(true);
        sizePolicy1.setHeightForWidth(pickMovingImageButton->sizePolicy().hasHeightForWidth());
        pickMovingImageButton->setSizePolicy(sizePolicy1);
        pickMovingImageButton->setBaseSize(QSize(0, 0));

        gridLayout->addWidget(pickMovingImageButton, 1, 2, 1, 1);

        labelMovingColour = new QLabel(widget);
        labelMovingColour->setObjectName(QString::fromUtf8("labelMovingColour"));

        gridLayout->addWidget(labelMovingColour, 2, 0, 1, 1);

        lineMovingColour = new QLineEdit(widget);
        lineMovingColour->setObjectName(QString::fromUtf8("lineMovingColour"));

        gridLayout->addWidget(lineMovingColour, 2, 1, 1, 1);

        pickMovingColourButton = new QPushButton(widget);
        pickMovingColourButton->setObjectName(QString::fromUtf8("pickMovingColourButton"));
        pickMovingColourButton->setEnabled(true);
        sizePolicy1.setHeightForWidth(pickMovingColourButton->sizePolicy().hasHeightForWidth());
        pickMovingColourButton->setSizePolicy(sizePolicy1);
        pickMovingColourButton->setBaseSize(QSize(0, 0));

        gridLayout->addWidget(pickMovingColourButton, 2, 2, 1, 1);

        labelOutputdir = new QLabel(widget);
        labelOutputdir->setObjectName(QString::fromUtf8("labelOutputdir"));

        gridLayout->addWidget(labelOutputdir, 3, 0, 1, 1);

        lineOutputDir = new QLineEdit(widget);
        lineOutputDir->setObjectName(QString::fromUtf8("lineOutputDir"));

        gridLayout->addWidget(lineOutputDir, 3, 1, 1, 1);

        pickDirectoryButton = new QPushButton(widget);
        pickDirectoryButton->setObjectName(QString::fromUtf8("pickDirectoryButton"));
        pickDirectoryButton->setEnabled(true);
        sizePolicy1.setHeightForWidth(pickDirectoryButton->sizePolicy().hasHeightForWidth());
        pickDirectoryButton->setSizePolicy(sizePolicy1);
        pickDirectoryButton->setBaseSize(QSize(0, 0));

        gridLayout->addWidget(pickDirectoryButton, 3, 2, 1, 1);

        labelParameterFile = new QLabel(widget);
        labelParameterFile->setObjectName(QString::fromUtf8("labelParameterFile"));

        gridLayout->addWidget(labelParameterFile, 4, 0, 1, 1);

        lineParameterFile = new QLineEdit(widget);
        lineParameterFile->setObjectName(QString::fromUtf8("lineParameterFile"));

        gridLayout->addWidget(lineParameterFile, 4, 1, 1, 1);

        pickParameterFileButton = new QPushButton(widget);
        pickParameterFileButton->setObjectName(QString::fromUtf8("pickParameterFileButton"));
        pickParameterFileButton->setEnabled(true);
        sizePolicy1.setHeightForWidth(pickParameterFileButton->sizePolicy().hasHeightForWidth());
        pickParameterFileButton->setSizePolicy(sizePolicy1);
        pickParameterFileButton->setBaseSize(QSize(0, 0));

        gridLayout->addWidget(pickParameterFileButton, 4, 2, 1, 1);

        label_2 = new QLabel(widget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 5, 0, 1, 1);

        lineTemp = new QLineEdit(widget);
        lineTemp->setObjectName(QString::fromUtf8("lineTemp"));

        gridLayout->addWidget(lineTemp, 5, 1, 1, 1);

        pickTempButton = new QPushButton(widget);
        pickTempButton->setObjectName(QString::fromUtf8("pickTempButton"));
        pickTempButton->setEnabled(true);
        sizePolicy1.setHeightForWidth(pickTempButton->sizePolicy().hasHeightForWidth());
        pickTempButton->setSizePolicy(sizePolicy1);
        pickTempButton->setBaseSize(QSize(0, 0));

        gridLayout->addWidget(pickTempButton, 5, 2, 1, 1);

        widget1 = new QWidget(fuxlastix);
        widget1->setObjectName(QString::fromUtf8("widget1"));
        widget1->setGeometry(QRect(1210, 620, 297, 27));
        horizontalLayout = new QHBoxLayout(widget1);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        checkerButton = new QPushButton(widget1);
        checkerButton->setObjectName(QString::fromUtf8("checkerButton"));

        horizontalLayout->addWidget(checkerButton);

        label_4 = new QLabel(widget1);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout->addWidget(label_4);

        spinBox = new QSpinBox(widget1);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setMinimum(50);
        spinBox->setMaximum(500);
        spinBox->setSingleStep(50);
        spinBox->setValue(250);

        horizontalLayout->addWidget(spinBox);

        saveCheckerButton = new QPushButton(widget1);
        saveCheckerButton->setObjectName(QString::fromUtf8("saveCheckerButton"));

        horizontalLayout->addWidget(saveCheckerButton);


        retranslateUi(fuxlastix);

        QMetaObject::connectSlotsByName(fuxlastix);
    } // setupUi

    void retranslateUi(QDialog *fuxlastix)
    {
        fuxlastix->setWindowTitle(QCoreApplication::translate("fuxlastix", "fuxlastix", nullptr));
        ctlabel->setText(QCoreApplication::translate("fuxlastix", "FIXED IMAGE", nullptr));
        pushRunButton->setText(QCoreApplication::translate("fuxlastix", "Run", nullptr));
        pushQuitButton->setText(QCoreApplication::translate("fuxlastix", "&Quit", nullptr));
        xpitlabel->setText(QCoreApplication::translate("fuxlastix", "TextLabel", nullptr));
        fuxlabel->setText(QCoreApplication::translate("fuxlastix", "TextLabel", nullptr));
        label_3->setText(QCoreApplication::translate("fuxlastix", "FuxLastix", nullptr));
        histolabel->setText(QCoreApplication::translate("fuxlastix", "MOVING IMAGE", nullptr));
        label->setText(QCoreApplication::translate("fuxlastix", "\302\251 xPit 2020  by  AS, JA, CD", nullptr));
        labelFixedImage->setText(QCoreApplication::translate("fuxlastix", "Fixed Image", nullptr));
        pickFixedImageButton->setText(QCoreApplication::translate("fuxlastix", "...", nullptr));
        labelMovingImage->setText(QCoreApplication::translate("fuxlastix", "Moving Image", nullptr));
        pickMovingImageButton->setText(QCoreApplication::translate("fuxlastix", "...", nullptr));
        labelMovingColour->setText(QCoreApplication::translate("fuxlastix", "Moving Colour", nullptr));
        pickMovingColourButton->setText(QCoreApplication::translate("fuxlastix", "...", nullptr));
        labelOutputdir->setText(QCoreApplication::translate("fuxlastix", "Output directory", nullptr));
        pickDirectoryButton->setText(QCoreApplication::translate("fuxlastix", "...", nullptr));
        labelParameterFile->setText(QCoreApplication::translate("fuxlastix", "Parameter file", nullptr));
        pickParameterFileButton->setText(QCoreApplication::translate("fuxlastix", "...", nullptr));
        label_2->setText(QCoreApplication::translate("fuxlastix", "Temp folder", nullptr));
        pickTempButton->setText(QCoreApplication::translate("fuxlastix", "...", nullptr));
        checkerButton->setText(QCoreApplication::translate("fuxlastix", "Compare", nullptr));
        label_4->setText(QCoreApplication::translate("fuxlastix", "Square size:", nullptr));
        saveCheckerButton->setText(QCoreApplication::translate("fuxlastix", "save", nullptr));
    } // retranslateUi

};

namespace Ui {
    class fuxlastix: public Ui_fuxlastix {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FUXLASTIX_H
