#pragma once

#ifndef knn_DeepLearning_h
#define knn_DeepLearning_h
#include "knn.h"
#ifdef __cplusplus
#define isnan(x) std::isnan(x)
extern "C"
{
#endif

  struct knn_DeepLearning_Backpropagator_Internal;
  typedef struct knn_DeepLearning_Backpropagator_Internal knn_DeepLearning_Backpropagator_Internal;
  typedef struct knn_DeepLearning_Backpropagator
  {
    double *expextedOutput;
    double (*costFunction)(double, double);
    knn_DeepLearning_Backpropagator_Internal *internal;
  } knn_DeepLearning_Backpropagator;

  void knn_DeepLearning_initBackpropagateInternal(knn_DeepLearning *net, knn_DeepLearning_Backpropagator *backpropagator);

  void knn_DeepLearning_backpropagate(knn_DeepLearning *net, knn_DeepLearning_Backpropagator info);
  void knn_DeepLearning_backpropagateStore(knn_DeepLearning *net, knn_DeepLearning_Backpropagator info, uint max);

  static double knn_DeepLearning_getCost(knn_DeepLearning *net, knn_DeepLearning_Backpropagator info)
  {
    double sum = 0;
    for (uint i = 0; i < net->output->nodes.length; i++)
    {
      sum += info.costFunction(array_get(knn_Node, net->output->nodes, i).value, info.expextedOutput[i]);
    }
    return sum / net->output->nodes.length;
  }

  static double knn_DeepLearning_meanSquaredError(double yHat, double y)
  {
    return (yHat - y) * (yHat - y) / 2;
  }

  static double knn_DeepLearning_crossEntropyLoss(double yHat, double y)
  {
    if (y == 1)
    {
      if (fcompare(yHat, 0))
        return 1000;
      nassert(isnan(-log(yHat)));
      return -log(yHat);
    }
    else
    {
      if (fcompare(1-yHat, 0))
        return 1000;
      nassert(isnan(-log(1 - yHat)));
      return -log(1 - yHat);
    }
  }
#ifdef __cplusplus
}
#endif

#endif
