#include "matchimage.h"
#include "ui_matchimage.h"

using namespace cv;
using namespace std;
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
matchImage::matchImage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::matchImage)
{
    ui->setupUi(this);
}

matchImage::~matchImage()
{
    delete ui;
}

void matchImage::updateImage(Mat img,int viewImage)
{
    // Change Mat image to Qpixmap image
    QPixmap imgIn = cvMatToQPixmap(img);

    //------------------------------------ Display Image----------------------------------------------
    if(viewImage==1)
    {
        ui->viewImage1->setPixmap(imgIn);
        ui->viewImage1->setAlignment(Qt::AlignCenter);
        ui->viewImage1->show();
    }
    else if(viewImage==2)
    {
        ui->viewImage2->setPixmap(imgIn);
        ui->viewImage2->setAlignment(Qt::AlignCenter);
        ui->viewImage2->show();
    }
    else if(viewImage==3)
    {
        ui->viewImage3->setPixmap(imgIn);
        ui->viewImage3->setAlignment(Qt::AlignCenter);
        ui->viewImage3->show();
    }

}
void matchImage::on_btn_load_clicked()
{
    //-----------------------------Open & Select Source Image ---------------------------------------
    QString filename = QFileDialog::getOpenFileName(
                    this,
                    tr("Open File"),
                    "D://Internship/MatchCorrelAndShape/Picture",                        //your picture path
                    "All file (*.*);;JPEG(*.jpg);;Bitmap(*.bmp);;PNG(*.png)"
            );
    std::string file1 = filename.toStdString();
    imageSrc = imread(file1, 1);
    imageSrc.copyTo( imageDisplay );
    imageSrc.copyTo(  imageOri );
    imageSrc = imread(file1, 0);
    updateImage(imageSrc,1);
    if(file1=="")
    {
        chkImage1 = 0;
    }
    else
    {
        chkImage1 = 1;
        wSrc = imageSrc.cols;
        hSrc = imageSrc.rows;

    }
}
void matchImage::on_btn_viewImg1_clicked()
{
    if(chkImage1 == 1)
    {
        imshow("Input image",imageDisplay);
    }
    else
    {
        QMessageBox::information(this,tr("Error!"),tr("Please select input image before."));
    }
}
void matchImage::on_btn_load2_clicked()
{
    if(ui->chk_newImg->isChecked())
    {
        //-----------------------------Open & Select Template Image ---------------------------------------
        QString filename = QFileDialog::getOpenFileName(
                        this,
                        tr("Open File"),
                        "D://Internship/MatchCorrelAndShape/Picture",                        //---------**your picture path
                        "All file (*.*);;JPEG(*.jpg);;Bitmap(*.bmp);;PNG(*.png)"
            );
         std::string file2 = filename.toStdString();
         imageTemp = imread(file2, 0);
         updateImage(imageTemp,2);
         if(file2=="")
              chkImage2 = 0;
         else
         {
              chkImage2 = 1;
              wTemp = imageTemp.cols;
              hTemp = imageTemp.rows;
         }
    }
    else if(ui->chk_crop->isChecked())
    {
        //---------------------------- Select Crop Image From Source Image --------------------------------------
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

            imageTemp = imageSrc(roi);
            updateImage(imageTemp,2);

            chkImage2 = 1;
            wTemp = imageTemp.cols;
            hTemp = imageTemp.rows;
        }
        else
        {
             QMessageBox::information(this,tr("Error!"),tr("Please select input image before."));
        }
    }
}

void matchImage::on_btn_match1_clicked()
{
    countMatch = 0;
    //-------------------------------------- Check Pre-Image -----------------------------------------------
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
    //-------------------------------------------------------------------------------------------------------------------

    inputCorrel = intput.toDouble();
    QString str = QString::number(inputCorrel);

    ui->result->clear();
    updateImage(imageSrc,1);

    //---------------------------------------------Resize Image---------------------------------------------------------
    if(wSrc>=500)
        resizeImageFn(1,0.5);
    else
        resizeImageFn(0,0);
    //------------------------------------------------------------------------------------------------------------------
    findInterestedLocation();
}

void matchImage::on_btn_openResult_clicked()
{
    //-----------------------------Open & Select Result Image ---------------------------------------
    QString filename = QFileDialog::getOpenFileName(
                    this,
                    tr("Open File"),
                    "D://Internship/MatchCorrelAndShape/Picture",                        //---------**your picture path
                    "All file (*.*);;JPEG(*.jpg);;Bitmap(*.bmp);;PNG(*.png)"
     );
     std::string file3 = filename.toStdString();
     imageResult = imread(file3, 0);
     updateImage(imageResult,3);
     if(file3=="")
     {
         chkImage3 = 0;
     }
     else
     {
         chkImage3 = 1;
         row = imageResult.rows;
         col = imageResult.cols;
         hResult = col;
         wResult = row;
     }
}
void matchImage::on_btn_match2_clicked()
{
    if(checkFind == 0)
    {
        QMessageBox::information(this,tr("Can not matching!"),tr("Please find interested location before!"));
        return;
    }
    if(chkImage3 == 0)
    {
        QMessageBox::information(this,tr("Can not matching!"),tr("Please select result image!"));
        return;
    }
    checkFind = 0;
    //-----------------------------------Labeling Location-----------------------------------------
    labelingLocation();
    //------------------------------------- Show Result --------------------------------------------
    if(ui->chk_shape->isChecked())
    {
         matchingImage(0);
    }
    else if(ui->chk_correlation->isChecked())
    {


    }
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
void matchImage::matchingImage(int medthode)
{
    firstShapeBase=0;
    int wSlide= min(wTemp,hTemp);
    testM = 0;
    /*Rect roi2;
    roi2.x = 0;
    roi2.y = 0;
    roi2.width = wSlide;
    roi2.height = wSlide;
    imageTemp  =  imageTemp (roi2);*/

    Rect roi;
    roi.width = wSlide;
    roi.height = wSlide;

    int count = 0,indexComponent=0;
    for (int y = 0;y < row; y++)
    {
         for (int x = 0; x < col; x++)
         {

                //printf(">>%d %d",component[countCom],countCom);
                //if(output[y][x] == 1 && maskImage[y][x]!=-1)
                indexComponent=(y*col)+x;
                if(component[indexComponent]!=0)
                {
                    //if(component[indexComponent]==4)printf("Hi\n");
                    if(isReSize==0)
                    {
                        incX=x;incY=y;
                    }
                    else
                    {
                        incX=2*x,incY=2*y;
                    }
                    roi.x = incX;
                    roi.y = incY;

                    cropChkImg = imageSrc(roi);
                    //if(xx==0)
                    if(testM<5)
                    {
                        shapeBaseMatch(cropChkImg,imageTemp,imageDisplay);
                        if(xx==1)  // Match
                        {
                             printf("Match) X:%d  Y:%d\n",incX,incY);
                             int c = component[indexComponent];
                             int countCom2=0;
                             for (int x = 0; x < row; x++)
                              {
                                  for (int y = 0; y < col; y++,countCom2++)
                                  {
                                      if(component[countCom2]==c)component[countCom2]=0;
                                  }
                              }
                             x+=newWTemp/2;
                             xx=0;
                        }
                        if(xx==-1)  // Not Match
                        {
                            int c = component[indexComponent];
                            int countCom2=0;
                            for (int x = 0; x < row; x++)
                             {
                                 for (int y = 0; y < col; y++,countCom2++)
                                 {
                                     if(component[countCom2]==c)component[countCom2]=0;
                                 }
                             }
                            xx=-1;
                            x+=newWTemp/2;
                        }
                    }
                    count++;
                }

        }
     }
    printf("Count %d\n ",count);
    updateImage(imageDisplay,1);

}
void matchImage::shapeBaseMatch(Mat image1,Mat image2,Mat drawImg)
{
    //printf(">>) X:%d  Y:%d",incX,incY);
    Mat canny_output,canny_output2;
    vector<vector<Point> > contours,contours2;
    vector<Vec4i> hierarchy;
    int thresValue = 127;

    Canny( image1, canny_output, thresValue, thresValue*2, 3 );
    findContours( canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

    if(firstShapeBase==0)
    {
        Canny( image2, canny_output2,thresValue, thresValue*2, 3 );
        findContours( canny_output2, contours2, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
        cnt2 = contours2[0];
        imshow("canny Output Temp",canny_output2);
        firstShapeBase=1;
    }

    //printf("**end**\n");
    vector<Point> cnt1 = contours[0];
    double ret = matchShapes(cnt1,cnt2,1,0.0);
    Scalar color = Scalar( 255, 0, 0);

    /*if(component[(incY/2*217)+(incX/2)]==5)
    {
        imshow("canny Output1",canny_output);
        printf("Canny 5 : %f\n",ret);
    }*/
    if(ret<0.05)
    {
        //printf("Ret %f\n",ret);
        for( size_t i = 0; i< contours.size(); i++ )
        {
             //Scalar color = Scalar( rng.uniform(255, 255), rng.uniform(0,0), rng.uniform(0,0) );
             drawContours( drawImg, contours, (int)i, color, 2, 8, hierarchy, 0, Point(incX,incY) );  //430 190
        }
        //rectangle( imageSrc,Point( incX , incY ),Point( incX+wTemp-1, incY+hTemp-1 ), Scalar(255, 0 ,0),CV_FILLED);
        xx=1;
        testM++;
        imageDisplay = drawImg.clone();
    }
    else if(ret>5)
    {
        //rectangle( imageSrc,Point( incX , incY ),Point( incX+wTemp-1, incY+hTemp-1 ), Scalar(255, 0 ,0),CV_FILLED);
        xx=-1;
    }


}
void matchImage::labelingLocation()
{
    int output[row][col];
    component = new int[row*col];
    input = new int[row*col];

    int count=0,countInput=0;
    for (int y = 0;y < row; y++)
    {
        for (int x = 0; x < col; x++)
        {
            output[y][x] = imageResult.at<uchar>(y,x);

            if(output[y][x] > 160)
            {
                input[countInput++]=output[y][x]=1;
            }
            else
            {
                input[countInput++]=output[y][x]=0;
            }
        }
    }

    //-------------------------------------------------------------------------------
    int label[10000];
    int label_count = 0;
    for (int i = 0; i < row*col; i++)
         component[i] = i;
    for (int x = 0; x < wResult; x++)
    {
        for (int y = 0; y < hResult; y++)
        {
            unionCoords(x, y, x-1, y+1);
            unionCoords(x, y, x+1, y);
            unionCoords(x, y, x, y+1);
            unionCoords(x, y, x+1, y+1);
        }
    }

    // print the array
       for (int x = 0; x < wResult; x++)
       {
            for (int y = 0; y < hResult; y++)
            {
                if (input[x*hResult+y] == 0)
                {
                    component[x*hResult+y] = 0;
                    continue;
                }
                int c = x*hResult + y;
                while (component[c] != c) c = component[c];
                component[x*hResult+y] = c;
            }
       }
       for (int x = 0; x < wResult; x++)
       {
            for (int y = 0; y < hResult; y++)
            {
                int dt = component[x*hResult+y];
                if (dt == 0) continue;
                if (isHasLabel(label,label_count,dt)!=-1) continue;
                else label[label_count++] = dt;
            }
       }

       for (int x = 0; x < wResult; x++)
       {
               for (int y = 0; y < hResult; y++)
               {
                   int dt = component[x*hResult+y];
                   if (dt == 0) continue;
                   component[x*hResult+y] = isHasLabel(label,label_count,dt)+1;

               }
        }
       printf("Find interest OBJ: %d \n",label_count);
       /*for (int x = 0; x < row; x++)
       {
          for (int y = 0; y < col; y++)
          {
               printf("%d", component[x*hResult+y] );
          }
          printf("\n");
       }*/
}
void matchImage::resizeImageFn(int i,float reFac)
{
    if(i==1)
    {
        newWSrc = wSrc * reFac;
        newHSrc = hSrc * reFac;
        newWTemp =  wTemp * reFac;
        newHTemp =  hTemp * reFac;

        cv::resize(imageSrc, resizeSrcImage,Size(), reFac, reFac);
        cv::resize(imageTemp, resizeTempImage,Size(), reFac, reFac);

        imshow("Resize Src Img",resizeSrcImage);
        imshow("Resize Temp Img",resizeTempImage);

        isReSize=1;
    }
    else if(i==0)
    {
        resizeSrcImage = imageSrc.clone();
        resizeTempImage = imageTemp.clone();

        newWSrc = wSrc;
        newHSrc = hSrc;
        newWTemp =  wTemp;
        newHTemp =  hTemp;

        isReSize=0;
    }

}
void matchImage::findInterestedLocation()
{
    int result_cols =  newWSrc - newWTemp + 1,result_rows = newHSrc - newHTemp + 1;
    imageResult.create( result_rows, result_cols, CV_8UC1 );//CV_32FC1  //CV_8UC1
    matchTemplate( resizeSrcImage, resizeTempImage, imageResult, TM_CCORR_NORMED);
    normalize( imageResult,imageResult, 0, 1, NORM_MINMAX, -1, Mat() );
    imshow("SAVE THIS RESULT.",imageResult); // manual save .
    checkFind = 1;
}
void matchImage::AddRoot(QString name,QString description,int index)
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
       QString an = QString::number(a[index]);
       AddChild(itm,"Correlation",cor);
       AddChild(itm,"X",xstr);
       AddChild(itm,"Y",ystr);
       AddChild(itm,"Angle(CW)",an);
       AddChild(itm,"Width",wstr);
       AddChild(itm,"Height",hstr);
    }
}
void matchImage::AddChild(QTreeWidgetItem *parent,QString name,QString description)
{
    QTreeWidgetItem *itm= new QTreeWidgetItem();
    itm->setText(0,name);
    itm->setText(1,description);
    parent->addChild(itm);
}
void matchImage::doUnion(int a, int b)
{
    // get the root component of a and b, and set the one's parent to the other
    while (component[a] != a)
        a = component[a];
    while (component[b] != b)
        b = component[b];
    component[b] = a;
}
void matchImage::unionCoords(int x, int y, int x2, int y2)
{
    if (x2 >= 0 && y2 >= 0 && y2 < hResult && x2 < wResult && input[x*hResult+y] && input[x2*hResult+y2])
        doUnion(x*hResult + y, x2*hResult + y2);
}
int  matchImage::isHasLabel(int *label, int count,int dt)
{
    for(int i = 0 ; i < count ; i++)
    {
        if(label[i] == dt)
            return i;

    }
    return -1;
}
