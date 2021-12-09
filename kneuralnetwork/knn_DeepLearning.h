#pragma once

#ifndef knn_DeepLearning_h
#define knn_DeepLearning_h
#ifdef __cplusplus
extern "C"
{
#endif

#include "knn.h"

  typedef struct knn_DeepLearning_Backpropagator
  {
    float *expextedOutput;
    float(*costFunction)(float,float);
  } knn_DeepLearning_Backpropagator;

  void knn_DeepLearning_backpropagate(knn_DeepLearning *net, knn_DeepLearning_Backpropagator info);

  static float knn_DeepLearning_basicCostFunction(float y, float yHat)
  {
    return (y - yHat) * (y - yHat);
  }

#ifdef __cplusplus
}
#endif

#endif
