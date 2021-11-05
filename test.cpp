#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>

#include "webcam.h"
int main(int argc, char **argv)
{

  webcamInfo*webcam=webcamInfoAlloc();
  initWebcam(webcam);

  webcam_data data=initWebcamData(webcam);

  cv::namedWindow("Webcam", cv::WINDOW_AUTOSIZE);

  // this will contain the image from the webcam
  cv::Mat frame;
  // cv::VideoCapture(0) >> frame;

  frame.cols = data.width;
  frame.rows = data.height;

  while (1)
  {
    readWebcamFrame(&data, webcam);


    frame.data = data.data;
    cv::imshow("Webcam", frame);
    if (cv::waitKey(10) >= 0)
      break;
  }
}