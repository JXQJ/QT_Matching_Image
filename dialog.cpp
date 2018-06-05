#include "dialog.h"
#include "ui_dialog.h"
#include <QMessageBox>
using namespace cv;

inline QImage  cvMatToQImage( const cv::Mat &inMat )
{
    switch ( inMat.type() )
    {
        // 8-bit, 4 channel
        case CV_8UC4:
        {
            QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB32 );

            return image;
        }

            // 8-bit, 3 channel
        case CV_8UC3:
        {
            QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB888 );

            return image.rgbSwapped();
        }

            // 8-bit, 1 channel
        case CV_8UC1:
        {
            static QVector<QRgb>  sColorTable;

            // only create our color table once

            QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_Indexed8 );
            return image;
        }

        default:
        {
           break;
        }

    }

    return QImage();
}

inline QPixmap cvMatToQPixmap( const cv::Mat &inMat )
{
    return QPixmap::fromImage( cvMatToQImage( inMat ) );
}

//
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

void Dialog::updateImage1()
{
    QPixmap imgIn = cvMatToQPixmap(image);
    ui->label->setPixmap(imgIn);
    ui->label->setScaledContents(true);
    ui->label->show();
}
void Dialog::updateImage2()
{
    QPixmap imgIn = cvMatToQPixmap(image2);
    ui->label2->setPixmap(imgIn);
    ui->label2->setScaledContents(true);
    ui->label2->show();
}
Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_btn_load_clicked()
{
    image = imread("D://Internship/MatchingImage/Picture/1.jpg", 1);
    updateImage1();
    chkImage1 = 1;
}

void Dialog::on_btn_load2_clicked()
{
    image2 = imread("D://Internship/MatchingImage/Picture/3.jpg", 1);
    if(ui->chk_newImg->isChecked())
    {
         updateImage2();
         chkImage2 = 1;
    }
}

void Dialog::on_btn_match_clicked()
{
    if(chkImage1==0&&chkImage2==0)
    {
        QMessageBox::information(this,tr("Can not matching!"),tr("Please select image and sample image."));
        return;
    }
    else if(chkImage2==0)
    {
        QMessageBox::information(this,tr("Can not matching!"),tr("Please select sample image."));
        return;
    }
    else if(chkImage1==0)
    {
        QMessageBox::information(this,tr("Can not matching!"),tr("Please select image."));
        return;
    }

    if(ui->chk_shape->isChecked())
    {
            QMessageBox::information(this,tr("matching!"),tr("Bla Bla Bla Bla Bla Bla."));
    }
}
