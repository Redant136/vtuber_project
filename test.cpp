// #include <stdio.h>
// #include <stdlib.h>
// #include <iostream>
// #include <fstream>
// #include <sstream>
// #include <opencv2/opencv.hpp>

// #include "webcam.h"
// #include <tensorflow/c/c_api.h>
#include "kneuralnetwork/knn.h"
#include "kneuralnetwork/knn_DeepLearning.h"
using namespace chevan_utils;
using namespace chevan_utils::chevanut_print;
using namespace chevanut_byte_typedef;
// using namespace chevanut_byte_typedef;


int main(int argc, char **argv)
{


  // webcamInfo *webcam = webcamInfoAlloc();
  // initWebcam(webcam);

  // webcam_data data = initWebcamData(webcam);

  // cv::namedWindow("Webcam", cv::WINDOW_AUTOSIZE);

  // // this will contain the image from the webcam
  // cv::Mat frame = cv::Mat(0, 0, CV_8UC3);
  // // cv::VideoCapture(0) >> frame;

  // frame.cols = data.width;
  // frame.rows = data.height;

  // readWebcamFrame(&data, webcam);

  // frame.data = data.data;

  // while (1)
  // {
  //   readWebcamFrame(&data, webcam);

  //   frame.data = data.data;
  //   cv::imshow("Webcam", frame);
  //   if (cv::waitKey(10) >= 0)
  //     break;
  // }
}