#include <QCoreApplication>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
//--------------------------------------------- OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgcodecs.hpp"
using namespace cv;
using namespace std;

int row,col,incX,incY,decX,decY;
int wSlide;
int medthode = 0;

float correl;
cv::Mat corr, preImgRotate,r,imageDisplay,imageSrc,imageTemp,imageResult,imageTemp2,imageOri;

int output[1000][1000];
float imageValueMatch[1000][1000];
int imageAngleMatch[1000][1000];
int imageFlipMatch[1000][1000];
int flagMatch[1000][1000];

void findInterestedLocation(int angle);
void correlationMatch(Mat image1,Mat image2,int angle,int x,int y,int isFlip);
void drawTargetImage(int x,int y,int w,int h);
void shapeMatch(Mat image1,Mat image2,int x,int y,int angle,int isFlip);
void shapeMatch2(Mat image1,Mat image2,Mat image3);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    imageSrc = imread("D://Internship/MatchingImage/Picture/99.jpg", 0);     //shiba   s1   9
    imageOri = imread("D://Internship/MatchingImage/Picture/99.jpg", 1);     //shiba   s1   9
    imageTemp = imread("D://Internship/MatchingImage/Picture/i.jpg", 0); //minishiBa  s2  i

    imageDisplay =  imageOri.clone();
    imshow("SrcImg",imageSrc);


    int WH = max(imageTemp .cols,imageTemp.rows);
    int bright = imageTemp.at<uchar>(0,0);
    Point2f pc(WH/2., WH/2.);
    imageTemp2 = imageTemp.clone();

    if(medthode == 0)
    {
            //findInterestedLocation(0);
            shapeMatch2(imageSrc,imageTemp,imageOri );
    }
    if(medthode == 1)
    {
        for(int angle=0;angle<=359;angle+=45)
        {
            r = getRotationMatrix2D(pc, -angle, 1.0);
            warpAffine(imageTemp, imageTemp ,r, imageTemp.size(),INTER_LINEAR,BORDER_CONSTANT,Scalar(bright ,bright ,bright ));
            findInterestedLocation(angle);
            imageTemp = imageTemp2.clone();
        }

        for (int y = 0;y < row; y++)
        {
            for (int x = 0; x < col; x++)
            {
                if(imageValueMatch[y][x]>0.97)
                {
                    if(imageSrc.cols>=500)
                    {
                          incY=y*2;
                          incX=x*2;
                    }
                    else
                    {
                          incY=y;
                          incX=x;
                    }
                   drawTargetImage(incX,incY,imageTemp.cols,imageTemp.rows);
                   printf("Match|(%3d,%3d)|Angle:%3d|Flip:%d|Correl: %f\n",incX,incY,imageAngleMatch[y][x],imageFlipMatch[y][x],imageValueMatch[y][x]);
                }
            }
       }
         imshow("Display",imageDisplay);
    }


    return a.exec();
}
void findInterestedLocation(int angle)
{
    int count =0;

    int result_cols =  imageSrc.cols - imageTemp.cols+ 1,result_rows = imageSrc.rows - imageTemp.rows + 1;
    imageResult.create( result_rows, result_cols, CV_8UC1 );//CV_32FC1  //CV_8UC1
    matchTemplate( imageSrc, imageTemp, imageResult, TM_CCORR_NORMED);
    normalize( imageResult,imageResult, 0, 1, NORM_MINMAX, -1, Mat() );
    imageResult.convertTo(imageResult,CV_8UC1,255);

    imshow("Result",imageResult);
     imshow("Temp R",imageTemp);
    Mat testImage,testTempImage;

    row = imageResult.rows;
    col = imageResult.cols;

    wSlide= min(imageTemp.cols,imageTemp.rows);
    Rect roi;
    roi.width = wSlide;
    roi.height = wSlide;
    roi.x = 0;
    roi.y = 0;
    //imageTemp = imageTemp(roi);

    testTempImage = imageTemp.clone();
    int num;
    for(int mode=0;mode<=medthode;mode++)
    {
        if(mode==1)
        {
            testTempImage = imageTemp.clone();
            // Find Flip Image 1 = Horizontal  0 = Vertical
            flip(testTempImage, testTempImage, +1);
            matchTemplate( imageSrc, testTempImage, imageResult, TM_CCORR_NORMED);
            normalize( imageResult,imageResult, 0, 1, NORM_MINMAX, -1, Mat() );
            imageResult.convertTo(imageResult,CV_8UC1,255);
        }
        for (int y = 0;y < row; y++)
        {
            for (int x = 0; x < col; x++)
            {
                num = imageResult.at<uchar>(y,x);
                if(num >= 255 || (num>=200&&medthode==0))  // find brightness
                {
                    count++;
                    output[y][x]=1;
                    if(imageSrc.cols>=500)
                    {
                          decY=y/2;decX=x/2;
                    }
                    else
                    {
                          decY=y;decX=x;
                    }
                    roi.x = x;
                    roi.y = y;

                    if(medthode==1)
                    {
                        testImage = imageSrc(roi);
                        //testTempImage = imageTemp.clone();
                        correlationMatch(testImage,testTempImage,angle,decX,decY,mode);
                    }
                    if(medthode==0)
                    {
                        roi.width = wSlide;
                        roi.height = wSlide+5;
                        testImage = imageSrc(roi);
                        shapeMatch(testImage,testTempImage,x,y,0,0);
                    }
                }
            }
        }
         printf("Mode %d|Count(%d): %d\n",mode,angle,count);
    }
}
void correlationMatch(Mat image1,Mat image2,int angle,int x,int y,int isFlip)
{ 
     matchTemplate( image1, image2 ,corr, TM_CCORR_NORMED);
     correl = corr.at<float>(0,0);
     if(imageValueMatch[y][x]<correl)
     {
         imageValueMatch[y][x] = correl;
         imageAngleMatch[y][x] = angle;
         imageFlipMatch[y][x] = isFlip;
         if(correl>=0.97)
         {
             rectangle(imageSrc,Point( x*2 , y*2 ),Point( (x*2)+imageTemp.cols-1, (y*2)+imageTemp.rows-1 ), Scalar(255, 255 ,255),FILLED);
         }
     }

}
void shapeMatch(Mat image1,Mat image2,int x,int y,int angle,int isFlip)
{

    if((flagMatch[y/2][x/2]!=-1 && flagMatch[(y+imageTemp.rows)/2][x/2]!=-1 && flagMatch[y/2][(x+imageTemp.cols)/2]!=-1 && flagMatch[(y+imageTemp.rows)/2][(x+imageTemp.cols)/2]!=-1))
    {

        Mat canny_output,canny_output2;
        vector<vector<Point> > contours,contours2;
        vector<Vec4i> hierarchy, hierarchy2;
        vector<Point> cnt1,cnt2;
        int thresValue = 130;

        Canny( image1, canny_output, thresValue, thresValue*2, 3 );
        findContours( canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
        cnt1 = contours[0];

        Canny( image2, canny_output2,thresValue, thresValue*2, 3 );
        findContours( canny_output2, contours2, hierarchy2, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
        cnt2 = contours2[0];
        double ret = matchShapes(cnt1,cnt2,1,0.0);
        printf("x: %d  y:%d Ret:%f\n",x,y,ret);

        if(ret<0.05)
        {
            printf("Find: (%3d,%3d) |Angle: %3d | Ret: %f | Flip: %d\n",x,y,angle,ret,isFlip);
            for(int i = y/2;i<=(y+imageTemp.rows)/2;i++)
            {
                    for(int j = x/2;j<=(x+imageTemp.cols)/2;j++)
                    {
                        flagMatch[i][j]=-1;
                    }
            }

            for( size_t i = 0; i< contours.size(); i++ )
            {
                  drawContours( imageDisplay, contours, (int)i, Scalar(255, 0 ,0), 2, 8, hierarchy, 0, Point(x,y) );
            }
            rectangle(imageSrc,Point( x , y ),Point( x+imageTemp.cols-1, y+imageTemp.rows-1 ), Scalar(255, 255 ,255),FILLED);
        }
        else if(ret>5)
        {
            for(int i = y/2;i<=(y+imageTemp.rows)/2;i++)
            {
                    for(int j = x/2;j<=(x+imageTemp.cols)/2;j++)
                    {
                        flagMatch[i][j]=-1;
                    }
            }
        }

    }
    //rectangle(imageSrc,Point( x , y ),Point( x+imageTemp.cols-1, y+imageTemp.rows-1 ), Scalar(255, 255 ,255),FILLED);
}
void drawTargetImage(int x,int y,int w,int h)
{
    rectangle(imageDisplay,Point( x , y ),Point( x+w-1, y+h-1 ), Scalar(0, 250 ,0));
}
void shapeMatch2(Mat image1,Mat image2,Mat image3)
{
       Mat scene = image1.clone();
       Mat scene2 = image3.clone();
       Mat Template = image2.clone();
       Mat imagegray1, imagegray2, imageresult1, imageresult2;
       int thresh=80;

       imagegray1 = scene .clone();
       imagegray2 = image2.clone();

       // Preprocess pictures
       /*cvtColor(scene, imagegray1,CV_BGR2GRAY);
       cvtColor(Template,imagegray2,CV_BGR2GRAY);*/

       GaussianBlur(imagegray1,imagegray1, Size(5,5),2);
       GaussianBlur(imagegray2,imagegray2, Size(5,5),2);

       Canny(scene, imageresult1,thresh, thresh*2);
       Canny(Template, imageresult2,thresh, thresh*2);
       vector<vector <Point> > contours1;
       vector<vector <Point> >  contours2;
       vector<Vec4i>hierarchy1, hierarchy2;
       // Template
       findContours(imageresult2,contours2,hierarchy2,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE,cvPoint(0,0));
       // Szene
       findContours(imageresult1,contours1,hierarchy1,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE,cvPoint(0,0));


           double ans1=0;
               double ans2=0;
                   double ans3=0;
       // Match all contours with eachother
       for(unsigned int i = 0; i < contours1.size(); i++)
       {
           for(unsigned int j = 0; j < contours2.size(); j++)
           {
               ans1 = matchShapes(contours2[j],contours1[i],CV_CONTOURS_MATCH_I1 ,0);
               //ans2 = matchShapes(contours2[j],contours1[i],CV_CONTOURS_MATCH_I2 ,0);
               //ans3 = matchShapes(contours2[j],contours1[i],CV_CONTOURS_MATCH_I3 ,0);

               if(ans1 < 0.05)
               {
                   //printf("%.3f %.3f %.3f\n",ans1,ans2,ans3);
                   Scalar color(rand()%255,rand()%255,rand()%255);
                   drawContours(scene2,contours1,i,color,2,8,hierarchy1,0,Point());
                   drawContours(Template,contours2,j,color,2,8,hierarchy2,0,Point());
               }
           }
       }
       imshow("scene",scene2);
       imshow("template", Template);
}
