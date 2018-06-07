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
    ui->label->setAlignment(Qt::AlignCenter);
    ui->label->show();
}
void Dialog::updateImage2()
{
    QPixmap imgIn = cvMatToQPixmap(image2);
    ui->label2->setPixmap(imgIn);
    //ui->label2->setScaledContents(true);
    ui->label2->setAlignment(Qt::AlignCenter);
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
   image.copyTo( img_display );
   image.copyTo(  imageOri );
   image = imread(file1, 0);
   updateImage1();
   if(file1=="")
        chkImage1 = 0;
   else chkImage1 = 1;

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
       image2 = imread(file2, 0);
       if(file2=="")
            chkImage2 = 0;
       else chkImage2 = 1;
    }
    else if(ui->chk_crop->isChecked())
    {
        if(chkImage1==1)
        {
            QString tx=ui->tx->text();
            QString ty=ui->ty->text();
            QString bx=ui->bx->text();
            QString by=ui->by->text();

            Rect roi;
            roi.x = tx.toInt();
            roi.y =  ty.toInt();
            roi.width =  (bx.toInt())-(tx.toInt());
            roi.height = (by.toInt())-(ty.toInt());
            image2 = image(roi);

           /* QString str = QString::number(roi.x);
            AddRoot("X",str,-1);*/

            chkImage2 = 1;
        }
        else
        {
             QMessageBox::information(this,tr("Error!"),tr("Please select input image before."));
        }

    }
     updateImage2();

}


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
void Dialog::on_btn_match_2_clicked()
{
    Mat imgSlide;
    int wSlide=min(image2.cols,image2.rows);
    Rect roi2;
    roi2.x = 96;
    roi2.y = 81;
    roi2.width = wSlide;
    roi2.height = wSlide;
    imgSlide = image(roi2);
    imshow("Before Rotate",  imgSlide);

    int maxWH = max(imgSlide.cols,imgSlide.rows);
    Point2f pc(maxWH/2., maxWH/2.);
    Mat r = getRotationMatrix2D(pc, 90, 1.0);
    warpAffine(imgSlide, imgSlide ,r, imgSlide.size());

    imshow("Rotate",  imgSlide);
}
void Dialog::MatchingMethod2()
{
    img = image.clone();
    imageOri.copyTo( img_display );
    Mat clearImg;
    Mat imgSlide,corr;
    int rangeSlide = 5;

    int wSlide=min(image2.cols,image2.rows);

    Rect roi;
    roi.width = wSlide;
    roi.height = wSlide;

    Rect roi2;
    roi2.x = 0;
    roi2.y = 0;
    roi2.width = wSlide;
    roi2.height = wSlide;
    templ = image2(roi2);

    clearImg = image.clone();
     for(int angle=0;angle<=360;angle+=90) // rotate
    {

        for(int ySlide=1;ySlide+wSlide<image.rows;ySlide+=rangeSlide)    // y
        {
            roi.y =  ySlide;
            for(int xSlide=1;xSlide+wSlide<image.cols;xSlide+=rangeSlide)  //x
            {
                roi.x =  xSlide;
                img = clearImg.clone();
                imgSlide = img(roi);

               // if(xSlide==96 && ySlide==81)imshow("Before Rotate",  imgSlide);

                int maxWH = max(imgSlide.cols,imgSlide.rows);
                Point2f pc(maxWH/2., maxWH/2.);
                Mat r = getRotationMatrix2D(pc, angle, 1.0);
                warpAffine(imgSlide, imgSlide ,r, imgSlide.size());

               // if(xSlide==96 && ySlide==81)imshow("Rotate",  imgSlide);


                matchTemplate(imgSlide, templ, corr, TM_CCORR_NORMED);
                correl = corr.at<float>(0,0);  // corr only has one pixel

                if(correl>inputCorrel){
                       rectangle( img_display,Point( xSlide , ySlide ),Point( xSlide+wSlide , ySlide+wSlide ), Scalar(255, 0 ,0));
                       rectangle( img,Point( xSlide , ySlide ),Point( xSlide+wSlide , ySlide+wSlide ), Scalar(255, 0 ,0),CV_FILLED);
                       clearImg = img.clone();
                       w[countMatch] = h[countMatch]= wSlide;
                       x[countMatch] = xSlide;
                       y[countMatch] = ySlide;
                       //a[countMatch] = angle;
                       correlD[countMatch++]=correl;
                       xSlide+=wSlide;
                }
            }

        }
    }
    QPixmap imgIn = cvMatToQPixmap(img_display);
    ui->label->setPixmap(imgIn);
    ui->label->setAlignment(Qt::AlignCenter);
    ui->label->show();
}
void Dialog::MatchingMethod( int, void* )
{

    img = image.clone();
    imageOri.copyTo( img_display );
   //img.copyTo( img_display );
     for(int i=0;i<=90;){

          img = image.clone();
          int maxWH = max(img.cols,img.rows);
          imshow("Templ Img", img);
          Point2f pc(maxWH/2., maxWH/2.);
          int a = i;
          if(a>0)a=-a;
          Mat r = cv::getRotationMatrix2D(pc, a, 1.0);
          warpAffine(img, img, r, img.size());
          imshow("Rotate Img", img);
          templ = image2.clone();
          while(1){

              int result_cols =  img.cols - templ.cols + 1;
              int result_rows = img.rows - templ.rows + 1;
              result.create( result_rows, result_cols, CV_32FC1 );              
              matchTemplate( img, templ, result, match_method);
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
              roi.width =  templ.cols;
              roi.height = templ.rows;

              crop = img(roi);

              imshow("Result", result);
              correlation();

              if(correl>inputCorrel){
                    rectangle( img_display, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar(255, 0 ,0), 2, 8, 0 );
                    correlD[countMatch-1]=correl;
              }
              else
              {

                  countMatch--;
                  break;
              }

              cv::rectangle( img, cv::Point2f(  x[countMatch-1], y[countMatch-1] ), cv::Point2f( matchLoc.x + templ.cols,  matchLoc.y + templ.rows), cv::Scalar( 255, 255, 255 ),CV_FILLED);
              //imshow("Draw Image", img);
          }
        i+=90;
    }
          QPixmap imgIn = cvMatToQPixmap(img_display);
          ui->label->setPixmap(imgIn);
         // ui->label->setScaledContents(true);
          ui->label->setAlignment(Qt::AlignCenter);
          ui->label->show();

}
void Dialog::on_btn_match_clicked()
{
    countMatch = 0;

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

    QString intput=ui->coef->text();
    if(intput=="")
    {
        QMessageBox::information(this,tr("Can not matching!"),tr("Please input coefficeint."));
        return;
    }
    if(intput.toDouble()<0.5)
    {
        QMessageBox::information(this,tr("Can not matching!"),tr("Please input coefficeint more than 0.5!"));
        return;
    }
    inputCorrel = intput.toDouble();
    QString str = QString::number(inputCorrel);

    ui->result->clear();
    updateImage1();

    if(ui->chk_shape->isChecked())
    {
        QMessageBox::information(this,tr("matching!"),tr("Bla Bla Bla Bla Bla Bla."));
    }
    else if(ui->chk_correlation->isChecked())
    {
        MatchingMethod2();
       // MatchingMethod( 0, 0 );
        if(countMatch==0)
            AddRoot("Not match! ", "0" ,-1);
        else
        {
            QString c = QString::number(countMatch);
            AddRoot("Found match picture", c ,-1);
        }
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
       QString cor = QString::number(correlD[index]);

       AddChild(itm,"Correlation",cor);
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

void Dialog::on_view_clicked()
{
    if(chkImage1 == 1)
       imshow("Input image",img_display);
    else
    {
         QMessageBox::information(this,tr("Error!"),tr("Please select input image before."));
    }
}

void Dialog::on_correlation_methode_currentIndexChanged(const QString &arg1)
{
    match_method = ui->correlation_methode->currentIndex();
}


