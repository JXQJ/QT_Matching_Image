#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

#include <QDialog>
//--------------------------------------------- QT
#include <QMainWindow>
#include <QDir>
#include <QString>
#include <math.h>

//--------------------------------------------- OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_btn_load_clicked();

    void on_btn_load2_clicked();

    void on_btn_match_clicked();

private:
    Ui::Dialog *ui;

    void updateImage1();
    void updateImage2();
    cv::Mat image,image2;
    int chkImage1=0,chkImage2=0;
};

#endif // DIALOG_H
