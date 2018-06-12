#ifndef MATCHIMAGE_H
#define MATCHIMAGE_H


#include <QDialog>
//--------------------------------------------- QT
#include <QMainWindow>
#include <QDir>
#include <QString>
#include <math.h>
#include <Qtcore>
#include <QtGui>
#include <QTreeWidgetItem>
#include <algorithm>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
//--------------------------------------------- OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgcodecs.hpp"


namespace Ui {
class matchImage;
}

class matchImage : public QDialog
{
    Q_OBJECT

public:
    explicit matchImage(QWidget *parent = 0);
    ~matchImage();

private slots:
    void on_btn_load_clicked();

    void on_btn_load2_clicked();

    void on_btn_match1_clicked();

    void on_btn_openResult_clicked();

    void on_btn_match2_clicked();

    void on_btn_viewImg1_clicked();

    void updateImage(cv::Mat img,int viewImage);

    void AddRoot(QString name,QString description,int index);

    void AddChild(QTreeWidgetItem *parent,QString name,QString description);

    void doUnion(int a, int b);

    void unionCoords(int x, int y, int x2, int y2);

    int isHasLabel(int *label, int count,int dt);

    void findInterestedLocation();

    void resizeImageFn(int i,float reFac);

    void labelingLocation();

    void matchingImage(int medthode);

    void shapeBaseMatch(cv::Mat image1,cv::Mat image2,cv::Mat drawImg);

private:
    Ui::matchImage *ui;

    cv::Mat imageSrc,imageTemp,imageOri,imageDisplay,imageResult,cropChkImg;
    int chkImage1=0,chkImage2=0,chkImage3=0,countMatch=0;

    int x[50],y[50],w[50],h[50],a[50],row,col;
    float inputCorrel=0.5,correlD[50];

    int newWSrc,newHSrc,newWTemp,newHTemp,hResult,wResult;
    int hTemp,wTemp,hSrc,wSrc;
    int isReSize=0;

    int *component;
    int *input;
    int  checkFind = 0;
    cv::Mat resizeSrcImage,resizeTempImage;

    int incX=0,incY=0;
    int firstShapeBase=0;
    int testM=0;
    int xx=0;

    std::vector<cv::Point>cnt2 ;
};

#endif // MATCHIMAGE_H
