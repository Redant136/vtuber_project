#include "chevan_utils.h"
#include "webcam.h"

#include <opencv2/highgui.hpp>



int main()
{
  webcamInfo *cam = webcamInfoAlloc();
  initWebcam(cam, 0);
  webcam_data data = initWebcamData(cam);

  cv::namedWindow("Webcam", cv::WINDOW_AUTOSIZE);
  cv::Mat frame = cv::Mat(0, 0, CV_8UC3);
  frame.cols = data.width;
  frame.rows = data.height;

  while (readWebcamFrame(&data, cam));
    // readWebcamFrame(&data, cam);

  frame.data = data.data;

  while (1)
  {
    if (cv::waitKey(10) >= 0)
      break;
    readWebcamFrame(&data, cam);

    frame.data = data.data;
    cv::imshow("Webcam", frame);
  }

  return 0;
}
