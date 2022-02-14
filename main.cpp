#include <iostream>
#include <opencv2/opencv.hpp>

#include "abstract_object/abstract_object.hpp"
#include "data_exchange/data_exchange.h"

// #define TEST
// #define TEST1
#define DEBUG

// 轮廓点集 轮廓关系
std::vector<std::vector<cv::Point>> contours_;
std::vector<cv::Vec4i> hierarchy_;

abstract_blade::FanBlade target;
std::vector<abstract_blade::FanBlade> target_box;

// 筛选条件
float small_rect_area = 0.f;

void findEyeball(cv::Mat& _input_bin_img, cv::Mat& _input_drawing_board);

int main() {
  Data_exchange data_exchange;
  int index = 0;
  float area = 0.f;

  // 创建图像
  cv::Mat frame;
  cv::Mat src_img;
  cv::Mat gray_img;
  cv::Mat bin_copy;      // 首次二值化图的显示图
  cv::Mat bin_img_1;     // 首次二值化图
  cv::Mat bin_img_2;     // 掩码1号
  cv::Mat bin_img_3;     // 掩码2号
  cv::Mat bin_img_last;  // 最终二值化图

#ifdef DEBUG
  cv::Mat bin_img_1_roi;
  cv::Mat bin_img_2_roi;

  cv::Mat bin_img_3_roi;
  cv::Mat bin_img_4_roi;

#endif  // DEBUG

  cv::Mat src_roi_img;
  // cv::Rect roi_1 = cv::Rect(566, 520, 60, 60);  // 596,550
  // cv::Rect roi_2 = cv::Rect(650, 430, 80, 180);  // 596,550
  cv::Rect roi_1 = cv::Rect(566 - 470, 520 - 400, 60, 60);   // 596,550
  cv::Rect roi_2 = cv::Rect(650 - 470, 430 - 400, 80, 180);  // 596,550

#ifdef TEST
  cv::Mat img3;
  cv::Mat img4;
  cv::Mat img5;
#endif  // TEST

  // 创建滑动条窗口
  cv::Mat trackbar_img_ = cv::Mat::zeros(1, 300, CV_8UC1);

  // 创建滑动条变量
  int gray_value = 91;         // 83~91 77 85
  int gray_mask_value_1 = 33;  // 43
  int gray_mask_value_2 = 79;  //

  // 读取图像
  cv::VideoCapture cap(
      "/home/jun/workplace/Github/Mouse-eyeball-recognition/mouse.mp4");

  if (!cap.isOpened()) {
    std::cout << "camera can't open" << std::endl;
    return -1;
  }

  std::string window_name = {"threshold"};
  cv::namedWindow(window_name);
  cv::createTrackbar("GRAY_TH", window_name, &gray_value, 255, nullptr);
  cv::createTrackbar("GRAY_MASK_TH_1", window_name, &gray_mask_value_1, 255,
                     nullptr);
  cv::createTrackbar("GRAY_MASK_TH_2", window_name, &gray_mask_value_2, 255,
                     nullptr);

  while (1) {
    cap >> frame;
    cv::Rect roi_test = cv::Rect(470, 400, 260, 250);
    cv::Mat src_img = frame(roi_test);
    // cv::imshow("src_img", src_img);

#ifdef TEST1
    cv::Rect roi_test = cv::Rect(650, 430, 80, 180);
    cv::Mat long_roi_img = src_img(roi_test);
    cv::imshow("long_roi_img", long_roi_img);
#endif  // TEST1
#ifdef TEST
    img4 = src_img.clone();
    src_roi_img = src_img(roi);

    img5 = src_img(cv::Rect(566, 520, 60, 60));
    img3 = src_roi_img.clone();
    img3.copyTo(img5);
    cv::imshow("src", img4);
    cv::imshow("ROI", src_roi_img);
    cv::imshow("copy", src_img);
#endif  // TEST

    // 转换成灰度图
    // cv::GaussianBlur(src_img,src_img,cv::Size(5,5),1);

    // cv::erode(src_img,src_img,cv::getStructuringElement(cv::MORPH_ELLIPSE,cv::Size(21,21)));
    // cv::dilate(src_img,src_img,cv::getStructuringElement(cv::MORPH_ELLIPSE,cv::Size(21,21)));

    cv::cvtColor(src_img, gray_img, cv::COLOR_BGR2GRAY);

    // cv::medianBlur(gray_img,gray_img,5);
    // 阈值化
    cv::imshow(window_name, trackbar_img_);

    cv::threshold(gray_img, bin_img_1, gray_value, 255, cv::THRESH_BINARY);
    cv::threshold(gray_img, bin_img_2, gray_mask_value_1, 255,
                  cv::THRESH_BINARY);
    cv::threshold(gray_img, bin_img_3, gray_mask_value_2, 255,
                  cv::THRESH_BINARY);
    bin_img_1.copyTo(bin_copy);
    // cv::imshow("bin_copy",bin_copy);

#ifdef DEBUG
    bin_img_1_roi = bin_img_1(roi_1);
    bin_img_2_roi = bin_img_2(roi_1);
    bin_img_2_roi.copyTo(bin_img_1_roi);

    bin_img_3_roi = bin_img_1(roi_2);
    cv::imshow("bin_img_3_roi", bin_img_3_roi);
    bin_img_4_roi = bin_img_3(roi_2);
    bin_img_4_roi.copyTo(bin_img_3_roi);

    cv::imshow("bin_img_4_roi", bin_img_4_roi);
#endif  // DEBUG

    cv::imshow("bin_img_1", bin_img_1);
    // cv::imshow("bin_img_2",bin_img_2);

    // cv::blur(bin_img_1, bin_img_last, cv::Size(3, 3));

    // 预处理
    cv::erode(bin_img_1, bin_img_last,
              cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(21, 21)));
    cv::dilate(bin_img_last, bin_img_last,
               cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(21, 21)));

    // bin_img_1.copyTo(bin_img_last);

    // 利用轮廓提取查找轮廓
    findEyeball(bin_img_last, src_img);
    // 创建圆形轮廓

    // 采集圆形轮廓数据

    data_exchange.getvalueAutomatically(index, target_box[0].Area());
    ++index;

    cv::imshow("src", src_img);

    // cv::imshow("无预处理阈值化", bin_copy);
    cv::imshow("bin_img_last", bin_img_last);

    // if (cv::waitKey(0) == 32) {
    //   continue;
    // }
    target_box.clear();
    std::vector<abstract_blade::FanBlade>(target_box).swap(target_box);

    if (cv::waitKey(1) == 27) {
      break;
    }
  }
  data_exchange.writeinCSV();
  return 0;
}

void findEyeball(cv::Mat& _input_bin_img, cv::Mat& _input_drawing_board) {
  cv::findContours(_input_bin_img, contours_, hierarchy_, 2,
                   cv::CHAIN_APPROX_NONE);

  for (size_t i = 0; i != contours_.size(); ++i) {
    if (contours_[i].size() < 6) {
      continue;
    }

    small_rect_area = cv::contourArea(contours_[i]);
    if (small_rect_area < 550 || small_rect_area > 30000) {
      continue;
    }

    target.inputParams(contours_[i]);
    // target.displayFanBlade(_input_drawing_board);
    target_box.push_back(target);  // target_box暂未用到
  }

  std::sort(target_box.begin(), target_box.end(),
            [](abstract_blade::FanBlade& c1, abstract_blade::FanBlade& c2) {
              return c1.Area() > c2.Area();
            });
  target_box[0].displayFanBlade(_input_drawing_board);

  std::cout << "box:" << target_box.size() << std::endl;
  for (size_t i = 0; i != target_box.size(); ++i) {
    std::cout << "box[" << target_box[i].Area()
              << "] size:" << target_box.size() << std::endl;
  }
  std::cout << "------------------" << std::endl;

  cv::imshow("drawing_board", _input_drawing_board);

  contours_.clear();
  hierarchy_.clear();

  std::vector<std::vector<cv::Point>>(contours_).swap(contours_);
  std::vector<cv::Vec4i>(hierarchy_).swap(hierarchy_);
}
