#include "knn_DeepLearning.h"
#include "../chevan_thread.h"

static void calculateLayer(knn_DeepLearning_Layer *layer);
static void calcFullyConnected(knn_DeepLearning_Layer *layer)
{
  for (uint i = 0; i < layer->fullyConnected.links.length; i++)
  {
    knn_DeepLearning_Link l = array_get(knn_DeepLearning_Link, layer->fullyConnected.links, i);
    array_get(knn_Node, layer->nodes, l.output).value += array_get(knn_Node, layer->prev->nodes, l.input).value * l.weight;
    assert(isfinite(array_get(knn_Node, layer->nodes, l.output).value));
  }
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    array_get(knn_Node, layer->nodes, i).value += layer->bias[i];
    layer->fullyConnected.preFunctionNodeValues[i] = array_get(knn_Node, layer->nodes, i).value;
    array_get(knn_Node, layer->nodes, i).value = layer->fullyConnected.function(array_get(knn_Node, layer->nodes, i).value);
    assert(isfinite(array_get(knn_Node, layer->nodes, i).value));
    array_get(knn_Node, layer->nodes, i).calculated = true;
  }
}
static void calcConv_recurse(uint index, knnArray pos, knn_DeepLearning_Layer *layer)
{
  if (index == pos.length)
  {
    uint inOffset = knn_DeepLearning_getCorrespondingIndexArr(pos, layer->prev->layout);
    uint outOffset = knn_DeepLearning_getCorrespondingIndexArr(pos, layer->layout);
    for (uint i = 0; i < layer->convolutionnal.links.length; i++)
    {
      array_get(knn_Node, layer->nodes, outOffset).value +=
          array_get(knn_Node, layer->prev->nodes, array_get(knn_DeepLearning_Link, layer->convolutionnal.links, i).input + inOffset).value *
          array_get(knn_DeepLearning_Link, layer->convolutionnal.links, i).weight;
    }
    return;
  }
  for (uint i = 0; i < array_get(uint, layer->layout, index); i++)
  {
    array_get(uint, pos, index) = i;
    calcConv_recurse(index + 1, pos, layer);
  }
}
static void calcConv(knn_DeepLearning_Layer *layer)
{
  knnArray pos = array_init(uint, layer->layout.length);
  calcConv_recurse(0, pos, layer);
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    layer->convolutionnal.preFunctionNodeValues[i] = array_get(knn_Node, layer->nodes, i).value;
    array_get(knn_Node, layer->nodes, i).value = layer->convolutionnal.function(array_get(knn_Node, layer->nodes, i).value);
    array_get(knn_Node, layer->nodes, i).calculated = true;
  }
  array_ffree(&pos);
}
static void calcDropout(knn_DeepLearning_Layer *layer)
{
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    if (randf() > layer->dropout.dropoutRate)
    {
      array_get(knn_Node, layer->nodes, i).value = array_get(knn_Node, layer->prev->nodes, i).value;
    }
  }
}
static void calcSoftmax(knn_DeepLearning_Layer *layer)
{
  calcFullyConnected(layer);
  float sum = 0;
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    float d = array_get(knn_Node, layer->nodes, i).value;
    array_get(knn_Node, layer->nodes, i).value = exp(array_get(knn_Node, layer->nodes, i).value);
    if (isinf(array_get(knn_Node, layer->nodes, i).value))
    {
      array_get(knn_Node, layer->nodes, i).value = 10000;
    }

    nassert_value(d, isfinite(array_get(knn_Node, layer->nodes, i).value));
    layer->fullyConnected.preFunctionNodeValues[i] = array_get(knn_Node, layer->nodes, i).value;
    sum += array_get(knn_Node, layer->nodes, i).value;
  }
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    array_get(knn_Node, layer->nodes, i).value /= sum;
  }
}

static void calculateLayer(knn_DeepLearning_Layer *layer)
{
  if (!layer->type != knn_DeepLearning_Input)
  {
    for (uint i = 0; i < layer->nodes.length; i++)
    {
      array_get(knn_Node, layer->nodes, i).value = 0;
      array_get(knn_Node, layer->nodes, i).calculated = false;
    }
  }
  switch (layer->type)
  {
  case knn_DeepLearning_FullyConnected:
  case knn_DeepLearning_Linear:
    calcFullyConnected(layer);
    break;
  case knn_DeepLearning_Convolutionnal:
    calcConv(layer);
    break;
  case knn_DeepLearning_Softmax:
    calcSoftmax(layer);
    break;
  case knn_DeepLearning_Dropout:
    calcDropout(layer);
    break;
  default:
    nImpl_err;
    break;
  }

  if (layer->next)
    calculateLayer(layer->next);
}

void knn_DeepLearning_calculate(knn_DeepLearning *net)
{
  calculateLayer(net->input->next);
}

#if 0
static const uint THREAD_COUNT = knn_ThreadCount;
static void *_calcFullyConnected_weight_thread(void *_args)
{
  struct
  {
    uint tID;
    knn_DeepLearning_Layer *layer;
  } arg;
  memcpy(&arg, _args, sizeof(arg));

  // doing this saves the trouble of setting up a mutex as you are only writting to one output node
  for (uint n = arg.tID * arg.layer->prev->nodes.length; n < arg.layer->fullyConnected.links.length; n += THREAD_COUNT * arg.layer->prev->nodes.length)
  {
    for (uint i = n; i < n + arg.layer->prev->nodes.length; i++)
    {
      knn_DeepLearning_Link l = array_get(knn_DeepLearning_Link, arg.layer->fullyConnected.links, i);
      array_get(knn_Node, arg.layer->nodes, l.output).value += array_get(knn_Node, arg.layer->prev->nodes, l.input).value * l.weight;
      assert(isfinite(array_get(knn_Node, arg.layer->nodes, l.output).value));
    }
  }
  return NULL;
}
static void calcFullyConnected_threaded(knn_DeepLearning_Layer *layer)
{
  cthread_t calcThreads[THREAD_COUNT];
  struct
  {
    uint tID;
    knn_DeepLearning_Layer *layer;
  } threadData[THREAD_COUNT];
  for (uint t = 0; t < THREAD_COUNT; t++)
  {
    threadData[t].tID = t;
    threadData[t].layer = layer;
    cthread_create(calcThreads[t], _calcFullyConnected_weight_thread, threadData + t);
  }
  for (uint t = 0; t < THREAD_COUNT; t++)
  {
    cthread_join(calcThreads[t], NULL);
  }
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    array_get(knn_Node, layer->nodes, i).value += layer->bias[i];
    layer->fullyConnected.preFunctionNodeValues[i] = array_get(knn_Node, layer->nodes, i).value;
    array_get(knn_Node, layer->nodes, i).value = layer->fullyConnected.function(array_get(knn_Node, layer->nodes, i).value);
    assert(isfinite(array_get(knn_Node, layer->nodes, i).value));
    array_get(knn_Node, layer->nodes, i).calculated = true;
  }
}
#define calcFullyConnected calcFullyConnected_threaded

#endif