#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <sstream>
#include "chevan_utils.h"
using namespace chevan_utils;
using namespace chevanut_print;

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

  // this will contain the image from the webcam
  cv::Mat frame;

  // capture the next frame from the webcam
  camera >> frame;

  // display the frame until you press a key
  while (1)
  {
    // show the image on the window
    camera >> frame;
    cv::imshow("Webcam", frame);
    // wait (10ms) for a key to be pressed
    if (cv::waitKey(10) >= 0)
      break;
  }
  return 0;
}