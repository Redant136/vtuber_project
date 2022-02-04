#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>
#include <sstream>
#include "chevan_utils.h"
using namespace ch;

#define caffePath "/home/chevan/vtuber_project/dnn"



int main()
{

  cv::VideoCapture camera(0);
  if (!camera.isOpened())
  {
    std::cerr << "ERROR: Could not open camera" << std::endl;
    return 1;
  }

  // create a window to display the images from the webcam
  cv::namedWindow("Webcam", cv::WINDOW_AUTOSIZE);

  cv::dnn::Net net = cv::dnn::readNet(caffePath "/res10_300x300_ssd_iter_140000.caffemodel", caffePath "/deploy.prototxt");
  double scale=1;

  cv::Mat frame,blob;
  camera >> frame;

  while (1)
  {
    // show the image on the window
    camera >> frame;
    if(frame.empty()){
      break;
    }

    // cv::resize(frame, frame, cv::Size(300, 300));
    cv::dnn::blobFromImage(frame, blob, scale, cv::Size(300, 300), cv::Scalar(104.0, 177.0, 123.0), false, false, 5);

    net.setInput(blob);
    cv::Mat prob = net.forward();

    
    for (uint i = 0; i < prob.size[2]; i++)
    {
      float confidence = prob.at<float>(i, 2);
      if (confidence > 0.9)
      {
        println(prob.at<float>(i, 0));
        println(prob.at<float>(i, 1));

        int x_left_bottom = prob.at<float>(i,3) * frame.cols;
        int y_left_bottom = prob.at<float>(i,4) * frame.rows;
        int x_right_top = prob.at<float>(i,5) * frame.cols;
        int y_right_top = prob.at<float>(i,6) * frame.rows;

        cv::Rect rect(x_left_bottom, y_left_bottom, x_right_top - x_left_bottom, y_right_top - y_left_bottom);
        cv::rectangle(frame, rect, cv::Scalar(0, 0, 255));
      }
    }


    cv::imshow("Webcam", frame);
    // wait (10ms) for a key to be pressed
    if (cv::waitKey(10) >= 0)
      break;
  }
}
