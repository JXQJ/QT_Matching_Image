#ifndef DIALOG_H
#define DIALOG_H



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

//--------------------------------------------- OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgcodecs.hpp"

#include <iostream>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

    void AddRoot(QString name,QString description,int index);
    void AddChild(QTreeWidgetItem *parent,QString name,QString description);

public:
    explicit Dialog(QWidget *parent = 0);

    ~Dialog();

private slots:
    void on_btn_load_clicked();

    void on_btn_load2_clicked();

    void on_btn_match_clicked();

private:
    Ui::Dialog *ui;

    void MatchingMethod( int, void* );
    void updateImage1();
    void updateImage2();
    void correlation();


    cv::Mat image,image2;
    int chkImage1=0,chkImage2=0;
    double correl = -1;

    bool use_mask;
    cv::Mat img,templ,mask,result,crop;
    const char* image_window = "Source Image";
    const char* result_window = "Result window";
    const std::string file1,file2;

    int countMatch = 0;
    int x[50],y[50],w[50],h[50];
    double correlD[50],inputCorrel=0.5;


};

#endif // DIALOG_H
