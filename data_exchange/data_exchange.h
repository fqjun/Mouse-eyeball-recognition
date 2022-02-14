#pragma once

#include <opencv4/opencv2/opencv.hpp>
#include <fstream>
#include <string.h>
#include <fcntl.h>   //文件控制定义
#include <termios.h> //POSIX终端控制定义
#include <unistd.h>  //UNIX标准定义
#include <errno.h>   //ERROR数字定义
#include <sys/select.h>
#include <vector>

using namespace std;

class Data_exchange
{
private:
    /* data */
    vector<float>yvalue;
    vector<float>xvalue;

    cv::Mat data_csv;

    float y_value_temp;
    bool record_flag = false;

public:
    Data_exchange(){};
    ~Data_exchange(){};

    void getvalueAutomatically(float x_value,float y_value);
    void getvalueManually(float &x_value);
    void writeinCSV();
};






