#include "webcam.h"
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

extern "C"
{

  struct webcamInfo
  {
    cv::VideoCapture cameraCapture;
    cv::Mat cameraFrame;
  };

  webcamInfo *webcamInfoAlloc()
  {
    webcamInfo *webcam = new webcamInfo();
    return webcam;
  }

  int initWebcam(webcamInfo *webcam,unsigned int n)
  {
    webcam->cameraCapture.open(n);
    if (!webcam->cameraCapture.isOpened())
    {
      std::cerr << "ERROR! Unable to open camera\n";
      return 1;
    }
    webcam->cameraCapture >> webcam->cameraFrame;
    if (webcam->cameraFrame.empty())
    {
      std::cerr << "ERROR! blank frame grabbed\n";
      return 1;
    }
    return 0;
  }

  webcam_data initWebcamData(webcamInfo *webcam)
  {
    return {(uint8_t *)malloc(webcam->cameraFrame.cols * webcam->cameraFrame.rows * 3), (uint32_t)webcam->cameraFrame.cols, (uint32_t)webcam->cameraFrame.rows, 3};
  }

  int readWebcamFrame(webcam_data *data, webcamInfo *webcam)
  {
    webcam->cameraCapture >> webcam->cameraFrame;
    if (webcam->cameraFrame.empty())
    {
      std::cerr << "ERROR! blank frame grabbed\n";
      return 1;
    }
    cv::cvtColor(webcam->cameraFrame, webcam->cameraFrame, cv::COLOR_BGR2RGB);
    data->width = webcam->cameraFrame.cols;
    data->height = webcam->cameraFrame.rows;
    memcpy(data->data, webcam->cameraFrame.data, data->width * data->height * 3);
    return 0;
  }

  int webcamUninit(webcamInfo *webcam, webcam_data *data)
  {
    if (data)
    {
      free(data->data);
    }
    delete webcam;
  }
}