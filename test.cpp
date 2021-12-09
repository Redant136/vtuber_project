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
// using namespace chevanut_byte_typedef;

int main(int argc, char **argv)
{
  knnArray format = {new uint[2], 2, 2};
  array_get(uint, format, 0) = 4;
  array_get(uint, format, 1) = 4;
  knn_DeepLearning_Layer *in = knn_DeepLearning_inputLayer(format);
  array_get(uint, format, 0) = 3;
  array_get(uint, format, 1) = 3;
  knn_DeepLearning_Layer *conv = knn_DeepLearning_addConvolutionLayer(in, format);

  // for (uint i = 0; i < 3; i++)
  // {
  //   for (uint j = 0; j < 3; j++)
  //   {
  //     print(array_get(knn_DeepLearning_Link, conv->convolutionnal.links, i * 3 + j).weight, " ");
  //   }
  //   println();
  // }

  knn_DeepLearning_Layer *out = knn_DeepLearning_addConnectedLayer(conv, 2, "lin");
  knn_DeepLearning net = knn_DeepLearning_createNet_InOut(in, out);

  float **outV = knn_DeepLearning_getLayerValuesPointers(net.output);
  for (uint i = 0; i < 2; i++)
  {
    // ch_println(*outV[i]);
  }
  knn_DeepLearning_calculate(&net);
  for (uint i = 0; i < 2; i++)
  {
    ch_println(*outV[i]);
  }
  float r[2]={0,0};
  knn_DeepLearning_Backpropagator backprop = {r, knn_DeepLearning_basicCostFunction};
  knn_DeepLearning_backpropagate(&net, backprop);

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