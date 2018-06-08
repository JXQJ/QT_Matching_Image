#include <QCoreApplication>

//--------------------------------------------- OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgcodecs.hpp"

using namespace cv;
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Mat image = imread("D://Internship/MatchingImage/Picture/99.jpg", 0);     //shiba   s1   99
    Mat imageDis = imread("D://Internship/MatchingImage/Picture/99.jpg", 1);
    Mat tempImg = imread("D://Internship/MatchingImage/Picture/i.jpg", 0);  //minishiBa  s2  i

    int hTemp = tempImg.rows;
    int wTemp = tempImg.cols;

    int hSrc = image.rows;
    int wSrc = image.cols;

    int newWSrc,newHSrc,newWTemp,newHTemp;

    float reFac = 1;
    if(wSrc>500)reFac = 0.5;
    Mat resizeImage,resizeTempImg;
    int IsResize=1;
    if(IsResize==1)
    {
        newWSrc = wSrc * reFac;
        newHSrc = hSrc * reFac;

        newWTemp =  wTemp * reFac;
        newHTemp =  hTemp * reFac;

        Size sizeImg(newWSrc,newHSrc);
        resize(image,resizeImage,sizeImg);
        Size sizeTemp(newWTemp,newHTemp);
        resize(tempImg,resizeTempImg,sizeTemp);
    }
    /*else
    {
        resizeImage = image.clone();
        resizeTempImg = tempImg.clone();
        newWSrc = wSrc;
        newHSrc = hSrc;

        newWTemp =  wTemp;
        newHTemp =  hTemp;
    }*/

    imshow("Input",image );
    imshow("Resize Input",resizeImage );
    printf("H TEMP %d\n",newHTemp);
    printf("H  %d\n",newHSrc);
    Mat result;
    //int result_cols =  image.cols - tempImg.cols + 1,result_rows = image.rows - tempImg.rows + 1;
    int result_cols =  newWSrc - newWTemp + 1,result_rows = newHSrc - newHTemp + 1;
    result.create( result_rows, result_cols, CV_8UC1 );//CV_32FC1  //CV_8UC1
    matchTemplate( resizeImage, resizeTempImg, result, TM_CCORR_NORMED);
    normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );

    imshow("Result",result);

    //imwrite("ResultImg.png",result);
    //result = imread("D://Internship/build-Test-Desktop_Qt_5_11_0_MinGW_32bit-Debug/ResultImg.png", 0);  // dog
    //result = imread("D://Internship/build-Test-Desktop_Qt_5_11_0_MinGW_32bit-Debug/ResultImg2.png", 0);
    result = imread("D://Internship/build-Test-Desktop_Qt_5_11_0_MinGW_32bit-Debug/ResultImg3.png", 0);

    int row = result.rows;
    int col = result.cols;

    int incX=0,incY=0;

    printf("Cols: %d\n",col);
    printf("Rows: %d\n",row);

    int sum=0;
    int count=0;

    int wSlide= min(tempImg.cols,tempImg.rows);

    Mat cropChkImg;
    Rect roi;
    roi.width = wSlide;
    roi.height = wSlide;

    Rect roi2;
    roi2.x = 0;
    roi2.y = 0;
    roi2.width = wSlide;
    roi2.height = wSlide;
    tempImg =  tempImg (roi2);

    int angle=0;

    Mat corr,r,preImgRotate;
    float correl;

    int WH = max(tempImg.cols,tempImg.rows);
    Point2f pc(WH/2., WH/2.);

    Mat imgSrcRotate=image.clone();
    int output[row][col];
        for (int y = 0;y < row; y++)
        {
            for (int x = 0; x < col; x++)
            {
                output[y][x] = result.at<uchar>(y,x);

                if(output[y][x] > 155){
                    //printf("-");
                    //printf("%d %d~",x,y);

                    if(reFac==1)
                    {
                        incX=x;incY=y;
                    }
                    else
                    {
                        incX=2*x,incY=2*y;
                    }
                    roi.x = incX;
                    roi.y = incY;

                    imgSrcRotate = image.clone();
                    cropChkImg = imgSrcRotate(roi);

                    matchTemplate(cropChkImg, tempImg , corr, TM_CCORR_NORMED);       // check correlation btw 2 img
                    correl = corr.at<float>(0,0);
                    preImgRotate = cropChkImg.clone();
                    for(angle=0;angle<=360;angle+=45)
                    {

                        r = getRotationMatrix2D(pc, angle, 1.0);
                        warpAffine(cropChkImg, cropChkImg ,r, cropChkImg.size());

                        if(incX==310&&incY==65&&angle==315)imshow("ccc",cropChkImg);

                        matchTemplate(cropChkImg, tempImg , corr, TM_CCORR_NORMED);       // check correlation btw 2 img
                        correl = corr.at<float>(0,0);
                        if(correl>0.9)
                        {
                            printf("Match) X:%d Y:%d angle:%d  Correl:%.2f\n",incX,incY,angle,correl);
                            rectangle( imageDis,Point( incX , incY ),Point( incX+wTemp-1 , incY+hTemp-1), Scalar(0, 255 ,0));
                            rectangle( image,Point( incX , incY ),Point( incX+wTemp-1, incY+hTemp-1 ), Scalar(255, 0 ,0),CV_FILLED);
                            imshow("Clear ",image);
                            cropChkImg = preImgRotate.clone();
                            break;
                        }
                    }
                    cropChkImg = preImgRotate.clone();
                    //rectangle( imageDis,Point( incX , incY ),Point( incX+wTemp-1 , incY+hTemp-1), Scalar(0, 255 ,0));
                    //rectangle( resizeImage ,Point( x , y ),Point( x+newWTemp-1 , y+newHTemp-1), Scalar(0, 255 ,0));
                    count++;
                }
                else
                {

                     //printf("0");
                }
                //printf("%4d",output[y][x]);
                //sum += output[y][x];

            }
           // printf("\n");
        }

    imshow("Display",imageDis);
    //imshow("Re Display",resizeImage);
    //int average = sum/(row*col);
    printf("\nPixel %d\n ",image.rows*image.cols);
    //printf("AVG %d\n ",average);
    printf("Count %d\n ",count);




    return a.exec();
}
