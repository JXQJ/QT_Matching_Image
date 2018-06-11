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

void resizeImageFn(float reFac);
void findInterestedLocation();
void shapeBaseMatch(Mat image1,Mat image2,Mat drawImg);
void doUnion(int a, int b);
void unionCoords(int x, int y, int x2, int y2);
int isHasLabel(int *label, int count,int dt);

int newWSrc,newHSrc,newWTemp,newHTemp;
int hTemp,wTemp,hSrc,wSrc;
Mat image,tempImg;
Mat disPlayImg;
Mat resizeImage,resizeTempImg;
int reSize=0;
Mat result;
Mat cropChkImg;
int incX=0,incY=0;

int xx=0;
int testM=0;

int *component;
int *input;
void doUnion(int a, int b)
{
    // get the root component of a and b, and set the one's parent to the other
    while (component[a] != a)
        a = component[a];
    while (component[b] != b)
        b = component[b];
    component[b] = a;
}

void unionCoords(int x, int y, int x2, int y2)
{
    if (x2 >= 0 && y2 >= 0 && y2 < newHSrc && x2 < newWSrc && input[x*newHSrc+y] && input[x2*newHSrc+y2])
        doUnion(x*200 + y, x2*newHSrc + y2);
}

int isHasLabel(int *label, int count,int dt)
{
    for(int i = 0 ; i < count ; i++)
    {
        if(label[i] == dt)
            return i;

    }
    return -1;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    image = imread("D://Internship/MatchingImage/Picture/99.jpg", 0);     //shiba   s1   9
    disPlayImg = imread("D://Internship/MatchingImage/Picture/99.jpg", 1);     //shiba   s1   9
    tempImg = imread("D://Internship/MatchingImage/Picture/i.jpg", 0);  //minishiBa  s2  i
    Mat img3 = imread("D://Internship/MatchingImage/Picture/i2.jpg", 0);  //minishiBa  s2  i


    // Pre-Match 2 Image:  find interest location.
    hTemp = tempImg.rows;
    wTemp = tempImg.cols;

    hSrc = image.rows;
    wSrc = image.cols;

    if(wSrc>=500)
    {
         resizeImageFn(0.5);
         reSize=1;
         printf("..Resize Image..\n");
    }
    else
    {
        resizeImage = image.clone();
        resizeTempImg = tempImg.clone();
        newWSrc = wSrc;
        newHSrc = hSrc;

        newWTemp =  wTemp;
        newHTemp =  hTemp;
        reSize=0;
    }

    int wSlide= min(wTemp,hTemp);

    //Rect roi2;
    //roi2.x = 0;
    //roi2.y = 0;
    //roi2.width = wSlide;
    //roi2.height = wSlide;
    //tempImg =  tempImg (roi2);

    Rect roi;
    roi.width = wSlide;
    roi.height = wSlide;

    // ---------------------------------Find Interest Location-----------------------------------------
    findInterestedLocation();
    //imshow("Result",result);
    //result = imread("D://Internship/build-Test-Desktop_Qt_5_11_0_MinGW_32bit-Debug/ResultImg3.png", 0); // open result
    result = imread("D://Internship/build-Contour-Desktop_Qt_5_11_0_MinGW_32bit-Debug/ResultImg3re.png", 0);

    int row = result.rows;
    int col = result.cols;  
    int output[row][col];
    int maskImage[row][col];

    component = new int[row*col];
    input = new int[row*col];

    int count=0,countInput=0;
    for (int y = 0;y < row; y++)
    {
        for (int x = 0; x < col; x++)
        {
            output[y][x] = result.at<uchar>(y,x);

            if(output[y][x] > 160)
            {
                input[countInput++]=output[y][x]=1;
            }
            else
            {
                input[countInput++]=output[y][x]==0;
            }
        }
    }

    //---------------------------------------Labeling-----------------------------------------
    int label[10];
    int label_count = 0;
    for (int i = 0; i < row*col; i++)
         component[i] = i;
    /*for (int x = 0; x < col; x++)
    {
        for (int y = 0; y < row; y++)
        {
            unionCoords(x, y, x-1, y+1);
            unionCoords(x, y, x+1, y);
            unionCoords(x, y, x, y+1);
            unionCoords(x, y, x+1 , y+1);
        }
    }

    // print the array
        for (int x = 0; x < col; x++)
        {
            for (int y = 0; y < row; y++)
            {
                if (input[x*row+y] == 0)
                {
                    component[x*row+y] = 0;
                    continue;
                }
                int c = x*row + y;
                while (component[c] != c) c = component[c];
                component[x*row+y] = c;
            }
        }


       for (int x = 0; x < col; x++)
       {
            for (int y = 0; y < row; y++)
            {
                int dt = component[x*col+y];
                if (dt == 0) continue;
                if (isHasLabel(label,label_count,dt)!=-1) continue;
                else label[label_count++] = dt;
            }
       }
       for(int i = 0 ; i < label_count ; i++)
               printf("%d ",label[i]);

           for (int x = 0; x < col; x++)
           {
               for (int y = 0; y < row; y++)
               {
                   int dt = component[x*row+y];
                   if (dt == 0) continue;
                   component[x*row+y] = isHasLabel(label,label_count,dt)+1;

               }
           }
*/
    //----------------------------------------------------------------------------------------
    printf("Cols: %d\n",col);
    printf("Rows: %d\n",row);
    printf("TCols: %d\n",tempImg.cols);
    printf("TRows: %d\n",tempImg.rows);
    printf("New TW: %d\n", newWTemp);
    printf("WSlide: %d\n", wSlide);

    // ----------------------------------------Match------------------------------------------
     for (int y = 0;y < row; y++)
    {
         for (int x = 0; x < col; x++)
         {

                if(output[y][x] == 1 && maskImage[y][x]!=-1)
                {
                    //if(y*2==144)printf("////// Y:%d  X: %d   %d\n",y*2,x*2,maskImage[y][x]);
                    if(reSize==0)
                    {
                        incX=x;incY=y;
                    }
                    else
                    {
                        incX=2*x,incY=2*y;
                    }
                    roi.x = incX;
                    roi.y = incY;

                    cropChkImg = image(roi); 
                   // if(xx==0)
                    if(testM<3)
                    {
                        shapeBaseMatch(cropChkImg,tempImg,disPlayImg);
                    }
                    if(xx==1)  // Match
                    {
                         printf("Match) X:%d  Y:%d\n",incX,incY);
                         for(int i=y;(i-y)<newWTemp;i++)
                         {
                             for(int j=x;(j-x)<newWTemp;j++)
                             {
                                 maskImage[i][j] = -1;
                             }
                         }

                         x+=newWTemp/2;
                         xx=0;
                    }
                    if(xx==-1)  // Not Match
                    {
                        for(int i=y;(i-y)<newWTemp;i++)
                        {
                            for(int j=x;(j-x)<newWTemp;j++)
                            {
                                maskImage[i][j] = -1;

                                if(i==144/2) printf("xxxxY %d X%d|  %d\n",i*2,j*2,maskImage[i][j]);
                            }
                        }
                        xx=-1;
                        x+=newWTemp/2;
                    }

                    count++;
                }

        }
     }

        /*for(int i=0;i<row;i++)
        {
            for(int j=0;j<col;j++)
            {
               // printf("%2d", maskImage[i][j] );
            }
            //printf("|\n");
        }*/
    printf("Count %d\n ",count);
    imshow("Display Img",disPlayImg);
    return a.exec();
}

void resizeImageFn(float reFac)
{
    newWSrc = wSrc * reFac;
    newHSrc = hSrc * reFac;

    newWTemp =  wTemp * reFac;
    newHTemp =  hTemp * reFac;

    Size sizeImg(newWSrc,newHSrc);
    resize(image,resizeImage,sizeImg);
    Size sizeTemp(newWTemp,newHTemp);
    resize(tempImg,resizeTempImg,sizeTemp);
    return;
}
void findInterestedLocation()
{
    int result_cols =  newWSrc - newWTemp + 1,result_rows = newHSrc - newHTemp + 1;
    result.create( result_rows, result_cols, CV_8UC1 );//CV_32FC1  //CV_8UC1
    matchTemplate( resizeImage, resizeTempImg, result, TM_CCORR_NORMED);
    normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );
    imshow("SAVE THIS RESULT.",result); // manual save .
}
int first=0;
vector<Point> cnt2 ;
void shapeBaseMatch(Mat image1,Mat image2,Mat drawImg)
{
    //printf(">>) X:%d  Y:%d",incX,incY);
    Mat canny_output,canny_output2;
    vector<vector<Point> > contours,contours2;
    vector<Vec4i> hierarchy;
    int thresValue = 127;

    Canny( image1, canny_output, thresValue, thresValue*2, 3 );
    findContours( canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
    if(first==0)
    {
        Canny( image2, canny_output2,thresValue, thresValue*2, 3 );
        findContours( canny_output2, contours2, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
        cnt2 = contours2[0];
        first=1;
    }
    imshow("canny Output",canny_output);

    //printf("**end**\n");
    vector<Point> cnt1 = contours[0];
    double ret = matchShapes(cnt1,cnt2,1,0.0);
    Scalar color = Scalar( 255, 0, 0);

    if(ret<0.05)
    {
        for( size_t i = 0; i< contours.size(); i++ )
        {
             //Scalar color = Scalar( rng.uniform(255, 255), rng.uniform(0,0), rng.uniform(0,0) );
             drawContours( drawImg, contours, (int)i, color, 2, 8, hierarchy, 0, Point(incX,incY) );  //430 190
        }
        rectangle( image,Point( incX , incY ),Point( incX+wTemp-1, incY+hTemp-1 ), Scalar(255, 0 ,0),CV_FILLED);
        xx=1;
        testM++;
       // printf("C//Match) X:%d  Y:%d\n",incX,incY);
        //imshow( "Contours", drawImg );
        //imshow( "Clear Img", image );
        disPlayImg = drawImg.clone();
    }
    else if(ret>5)
    {
        rectangle( image,Point( incX , incY ),Point( incX+wTemp-1, incY+hTemp-1 ), Scalar(255, 0 ,0),CV_FILLED);
        xx=-1;
    }


}
