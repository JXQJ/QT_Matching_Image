#include "dialog.h"
#include "ui_dialog.h"
#include <QMessageBox>
#include <Qtcore>
#include <QtGui>
#include <QFileDialog>
#include <algorithm>

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

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

        ui->result->setColumnCount(2);
        ui->correlation_methode->addItem("1:SQDIFF");
        ui->correlation_methode->addItem("2:SQDIFF NORMED");
        ui->correlation_methode->addItem("3:TM CCORR");
        ui->correlation_methode->addItem("4:TM CCORR NORMED");
        ui->correlation_methode->addItem("5:TM COEFF NORMED");
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
    QString filename = QFileDialog::getOpenFileName(
                this,
                tr("Open File"),
                "D://Internship/MatchingImage/Picture",                        //---------** only C:// or D://
                "All file (*.*);;JPEG(*.jpg);;Bitmap(*.bmp);;PNG(*.png)"
    );
   std::string file1 = filename.toStdString();
   image = imread(file1, 1);
   updateImage1();
   chkImage1 = 1;

}

void Dialog::on_btn_load2_clicked()
{

    if(ui->chk_newImg->isChecked())
    {
        QString filename = QFileDialog::getOpenFileName(
                    this,
                    tr("Open File"),
                    "D://Internship/MatchingImage/Picture",                        //---------** only C:// or D://
                    "All file (*.*);;JPEG(*.jpg);;Bitmap(*.bmp);;PNG(*.png)"
        );
       std::string file2 = filename.toStdString();
       image2 = imread(file2, 1);
       updateImage2();
       chkImage2 = 1;
    }
}


int match_method;
int max_Trackbar = 5;

void Dialog::correlation()
{

     cv::Mat corr;
     Mat newCrop,newImg2;
     int minW =min(crop.size().width,image2.size().width);
     int minH =min(crop.size().height,image2.size().height);

     cv::Rect roi;
     roi.x = 0;
     roi.y = 0;
     roi.width = minW;
     roi.height = minH;
     newCrop = crop(roi);
     newImg2 = image2(roi);

     cv::matchTemplate(newCrop, newImg2, corr, cv::TM_CCORR_NORMED);
     correl = corr.at<float>(0,0);  // corr only has one pixel

}
void Dialog::MatchingMethod( int, void* )
{
      img = image.clone();
      templ = image2.clone();
      Mat img_display;
      img.copyTo( img_display );
      while(1){

          int result_cols =  img.cols - templ.cols + 1;
          int result_rows = img.rows - templ.rows + 1;
          result.create( result_rows, result_cols, CV_32FC1 );
          bool method_accepts_mask = (CV_TM_SQDIFF == match_method || match_method == CV_TM_CCORR_NORMED);
          if (use_mask && method_accepts_mask)
          {
              matchTemplate( img, templ, result, match_method, mask);
          }
          else
            {
              matchTemplate( img, templ, result, match_method);
          }
          normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );
          double minVal; double maxVal; Point minLoc; Point maxLoc;
          Point matchLoc;
          minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
          if( match_method  == TM_SQDIFF || match_method == TM_SQDIFF_NORMED )
          {
              matchLoc = minLoc;
          }
          else
          {
              matchLoc = maxLoc;
          }

          w[countMatch] = (matchLoc.x + templ.cols)-(matchLoc.x);
          h[countMatch] = (matchLoc.y + templ.rows)-(matchLoc.y);
          x[countMatch] = matchLoc.x;
          y[countMatch++] = matchLoc.y;

          Rect roi;
          roi.x = matchLoc.x;
          roi.y =  matchLoc.y;
          roi.width =  (matchLoc.x + templ.cols)-(matchLoc.x);
          roi.height =  (matchLoc.y + templ.rows)-(matchLoc.y);
          crop = img(roi);

          //imshow("crop", crop);
          correlation();

          if(correl>0.95){
                rectangle( img_display, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar(255, 0 ,0), 2, 8, 0 );
          }
          else
          {
               countMatch--;
               return;
          }

          QPixmap imgIn = cvMatToQPixmap(img_display);
          ui->label->setPixmap(imgIn);
          ui->label->setScaledContents(true);
          ui->label->show();

          cv::rectangle( img, cv::Point2f(  x[countMatch-1], y[countMatch-1] ), cv::Point2f( matchLoc.x + templ.cols,  matchLoc.y + templ.rows), cv::Scalar( 255, 255, 255 ),CV_FILLED);
          //imshow("Draw Image", img);
      }
}
void Dialog::on_btn_match_clicked()
{


    if(chkImage1==0&&chkImage2==0)
    {
        QMessageBox::information(this,tr("Can not matching!"),tr("Please select input image and sample image."));
        return;
    }
    else if(chkImage2==0)
    {
        QMessageBox::information(this,tr("Can not matching!"),tr("Please select sample image."));
        return;
    }
    else if(chkImage1==0)
    {
        QMessageBox::information(this,tr("Can not matching!"),tr("Please select input image."));
        return;
    }

    ui->result->clear();
    updateImage1();

    if(ui->chk_shape->isChecked())
    {
        QMessageBox::information(this,tr("matching!"),tr("Bla Bla Bla Bla Bla Bla."));
    }
    else if(ui->chk_correlation->isChecked())
    {

        MatchingMethod( 0, 0 );
        QString qstr = QString::number(correl);

        AddRoot("Correlation",qstr,-1);
        for(int i=0;i<countMatch;i++)
        {
            QString str = QString::number(i+1);
            AddRoot("Matching ", str ,i);
        }
    }
}
void Dialog::AddRoot(QString name,QString description,int index)
{
    QTreeWidgetItem *itm= new QTreeWidgetItem(ui->result);
    itm->setText(0,name);
    itm->setText(1,description);
    ui->result->addTopLevelItem(itm);

    if(index != -1)
    {
       QString xstr = QString::number(x[index]);
       QString ystr = QString::number(y[index]);
       QString wstr = QString::number(w[index]);
       QString hstr = QString::number(h[index]);

       AddChild(itm,"X",xstr);
       AddChild(itm,"Y",ystr);
       AddChild(itm,"Width",wstr);
       AddChild(itm,"Height",hstr);
    }
}
void Dialog::AddChild(QTreeWidgetItem *parent,QString name,QString description)
{
    QTreeWidgetItem *itm= new QTreeWidgetItem();
    itm->setText(0,name);
    itm->setText(1,description);
    parent->addChild(itm);
}
