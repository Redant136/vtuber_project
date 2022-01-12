#define CHEVAN_UTILS_MACRO_MAGIC
#include "knn_DeepLearning.h"
#include "../chevan_thread.h"

static float randf_knn()
{
  return randf() * 2 - 1;
}

static float linearFunction(float x)
{
  return x;
}
static float sigmoidFunction(float x)
{
  return 1 / (1 + exp(-x));
}
static float softsignFunction(float x)
{
  return x / (1 + abs(x));
}
static float softplusFunction(float x)
{
  return log(exp(x) + 1);
}
static float swishFunction(float x)
{
  return x / (1 + exp(x));
}
static float reluFunction(float x)
{
  if (x > 0)
  {
    return x;
  }
  else
  {
    return 0.0;
  }
}

static knn_ActivationFunc parseFunction(const char *name)
{
  if (strcmp(name, "linear") == 0 || strcmp(name, "lin") == 0 || strcmp(name, "line") == 0)
  {
    return &linearFunction;
  }
  else if (strcmp(name, "sigmoid") == 0 || strcmp(name, "sig") == 0)
  {
    return &sigmoidFunction;
  }
  else if (strcmp(name, "tanh") == 0)
  {
    return &tanhf;
  }
  else if (strcmp(name, "softsign") == 0)
  {
    return &softsignFunction;
  }
  else if (strcmp(name, "softplus") == 0)
  {
    return &softplusFunction;
  }
  else if (strcmp(name, "swish") == 0)
  {
    return &swishFunction;
  }
  else if (strcmp(name, "relu") == 0)
  {
    return &reluFunction;
  }
  else
  {
    return &linearFunction;
  }
}

knn_DeepLearning_Layer *knn_DeepLearning_inputLayer(knnArray layout)
{
  knn_DeepLearning_Layer *layer = malloc(sizeof(knn_DeepLearning_Layer));
  layer->type = knn_DeepLearning_Input;
  uint tS = 1;
  for (uint i = 0; i < layout.length; i++)
  {
    tS *= array_get(uint, layout, i);
  }
  layer->nodes = (struct Array)array_init(knn_Node, tS);
  layer->layout = array_clone(uint, layout);
  layer->prev = NULL;
  layer->next = NULL;
  return layer;
}
knn_DeepLearning_Layer *knn_DeepLearning_inputLayer_size(size_t size)
{
  knn_DeepLearning_Layer *layer = malloc(sizeof(knn_DeepLearning_Layer));
  layer->type = knn_DeepLearning_Input;
  layer->nodes = (struct Array)array_init(knn_Node, size);
  layer->layout = (struct Array)array_init(uint, 1);
  array_get(uint, layer->layout, 0) = size;
  layer->prev = NULL;
  layer->next = NULL;
  return layer;
}
knn_DeepLearning_Layer *knn_DeepLearning_addLinearLayer(knn_DeepLearning_Layer *in, const char *function)
{
  knn_DeepLearning_Layer *layer = malloc(sizeof(knn_DeepLearning_Layer));
  in->next = layer;
  layer->prev = in;
  layer->type = knn_DeepLearning_Linear;
  layer->nodes = (struct Array){mallocArr(knn_Node, in->nodes.MAX_LENGTH), in->nodes.length, in->nodes.MAX_LENGTH};
  layer->fullyConnected.links = (struct Array){mallocArr(knn_DeepLearning_Layer, layer->nodes.MAX_LENGTH), layer->nodes.MAX_LENGTH, layer->nodes.MAX_LENGTH};
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    knn_DeepLearning_Link *l = &array_get(knn_DeepLearning_Link, layer->fullyConnected.links, i);
    l->input = i;
    l->output = i;
    l->weight = randf_knn();
  }
  layer->bias = calloc(in->nodes.MAX_LENGTH, sizeof(float));
  for (uint i = 0; i < in->nodes.MAX_LENGTH; i++)
  {
    layer->bias[i] = randf_knn();
  }
  layer->fullyConnected.function = parseFunction(function);
  layer->layout = (struct Array){in->layout.arr, in->layout.length, in->layout.MAX_LENGTH};
  layer->next = 0;
  return layer;
}
knn_DeepLearning_Layer *knn_DeepLearning_addConnectedLayer(knn_DeepLearning_Layer *in, size_t size, const char *function)
{
  knn_DeepLearning_Layer *layer = malloc(sizeof(knn_DeepLearning_Layer));
  in->next = layer;
  layer->prev = in;
  layer->type = knn_DeepLearning_Linear;
  layer->nodes = (struct Array){mallocArr(knn_Node, size), size, size};
  layer->fullyConnected.preFunctionNodeValues = calloc(layer->nodes.length, sizeof(float));
  layer->fullyConnected.links = (struct Array){calloc(layer->nodes.MAX_LENGTH * in->nodes.length, sizeof(knn_DeepLearning_Link)),
                                               layer->nodes.MAX_LENGTH * in->nodes.length, layer->nodes.MAX_LENGTH * in->nodes.length};
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    for (uint j = 0; j < layer->prev->nodes.length; j++)
    {
      knn_DeepLearning_Link *l = &array_get(knn_DeepLearning_Link, layer->fullyConnected.links, i * layer->prev->nodes.length + j);
      l->input = j;
      l->output = i;
      l->weight = randf_knn();
    }
  }
  layer->bias = calloc(size, sizeof(float));
  for (uint i = 0; i < size; i++)
  {
    layer->bias[i] = randf_knn();
  }
  layer->fullyConnected.function = parseFunction(function);
  layer->layout = (struct Array)array_init(uint, 1);
  array_get(uint, layer->layout, 0) = size;
  layer->next = 0;
  return layer;
}
knn_DeepLearning_Layer *knn_DeepLearning_addConnectedLayer_layout(knn_DeepLearning_Layer *in, knnArray layout, const char *function)
{
  knn_DeepLearning_Layer *layer = malloc(sizeof(knn_DeepLearning_Layer));
  in->next = layer;
  layer->prev = in;
  layer->type = knn_DeepLearning_FullyConnected;
  layer->nodes.MAX_LENGTH = 1;
  for (uint i = 0; i < layout.length; i++)
  {
    layer->nodes.MAX_LENGTH *= array_get(uint, layout, i);
  }
  layer->nodes = (struct Array){calloc(layer->nodes.MAX_LENGTH, sizeof(knn_Node)), layer->nodes.MAX_LENGTH, layer->nodes.MAX_LENGTH};
  layer->fullyConnected.preFunctionNodeValues = calloc(layer->nodes.length, sizeof(float));
  layer->fullyConnected.links = array_init(knn_DeepLearning_Link, layer->nodes.MAX_LENGTH * in->nodes.length);
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    for (uint j = 0; j < layer->prev->nodes.length; j++)
    {
      knn_DeepLearning_Link *l = &array_get(knn_DeepLearning_Link, layer->fullyConnected.links, i * layer->prev->nodes.length + j);
      l->input = j;
      l->output = i;
      l->weight = randf_knn();
    }
  }
  layer->bias = calloc(layer->nodes.length, sizeof(float));
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    layer->bias[i] = randf_knn();
  }
  layer->fullyConnected.function = parseFunction(function);
  layer->layout = array_clone(uint, layout);
  layer->next = 0;
  return layer;
}
static void convLayer_setupLinks(uint index, knnArray pos, knnArray convFormat, knnArray links, knnArray layout)
{
  if (index == pos.length)
  {
    knn_DeepLearning_Link *l = &array_get(knn_DeepLearning_Link, links, knn_DeepLearning_getCorrespondingIndexArr(pos, convFormat));
    l->input = knn_DeepLearning_getCorrespondingIndexArr(pos, layout);
    l->output = 0;
    return;
  }
  for (uint i = 0; i < array_get(uint, convFormat, index); i++)
  {
    array_get(uint, pos, index) = i;
    convLayer_setupLinks(index + 1, pos, convFormat, links, layout);
  }
}
knn_DeepLearning_Layer *knn_DeepLearning_addConvolutionLayer(knn_DeepLearning_Layer *in, knnArray convFormat, const char *function)
{
  knn_DeepLearning_Layer *layer = malloc(sizeof(knn_DeepLearning_Layer));
  in->next = layer;
  layer->prev = in;
  layer->type = knn_DeepLearning_Convolutionnal;
  layer->convolutionnal.function = parseFunction(function);

  assert(in->layout.length == convFormat.length);
  layer->layout = array_clone(uint, in->layout);

  for (uint i = 0; i < in->layout.length; i++)
  {
    array_get(uint, layer->layout, i) = array_get(uint, in->layout, i) - (i < convFormat.length ? array_get(uint, convFormat, i) : 0) + 1;
  }
  layer->nodes.MAX_LENGTH = 1;
  for (uint i = 0; i < layer->layout.length; i++)
  {
    layer->nodes.MAX_LENGTH *= array_get(uint, layer->layout, i);
  }
  layer->nodes.length = layer->nodes.MAX_LENGTH;
  layer->nodes.arr = calloc(sizeof(knn_Node), layer->nodes.length);
  layer->fullyConnected.preFunctionNodeValues = calloc(layer->nodes.length, sizeof(float));

  layer->convolutionnal.convFormat = array_clone(uint, convFormat);
  layer->convolutionnal.links.MAX_LENGTH = 1;
  for (uint i = 0; i < convFormat.length; i++)
  {
    layer->convolutionnal.links.MAX_LENGTH *= array_get(uint, convFormat, i);
  }
  layer->convolutionnal.links.length = layer->convolutionnal.links.MAX_LENGTH;
  layer->convolutionnal.links.arr = calloc(sizeof(knn_DeepLearning_Link), layer->convolutionnal.links.length);

  knnArray links = layer->convolutionnal.links;
  for (uint i = 0; i < layer->convolutionnal.links.length; i++)
  {
    array_get(knn_DeepLearning_Link, links, i).weight = randf_knn();
  }

  knnArray currentPosition = array_init(uint, convFormat.length);
  convLayer_setupLinks(0, currentPosition, convFormat, links, layer->prev->layout);
  array_ffree(&currentPosition);
  layer->next = 0;
  return layer;
}
knn_DeepLearning_Layer *knn_DeepLearning_addMaxPoolingLayer(knn_DeepLearning_Layer *in)
{
  nImpl_err;
  return NULL;
}
knn_DeepLearning_Layer *knn_DeepLearning_addDropoutLayer(knn_DeepLearning_Layer *in, float dropout)
{
  knn_DeepLearning_Layer *layer = malloc(sizeof(knn_DeepLearning_Layer));
  in->next = layer;
  layer->prev = in;
  layer->type = knn_DeepLearning_Dropout;
  layer->dropout.dropoutRate = dropout;
  layer->nodes = (struct Array){mallocArr(knn_Node, in->nodes.MAX_LENGTH), in->nodes.length, in->nodes.MAX_LENGTH};
  layer->layout = (struct Array){in->layout.arr, in->layout.length, in->layout.MAX_LENGTH};
  layer->next = 0;
  return layer;
}
knn_DeepLearning_Layer *knn_DeepLearning_addSoftmaxLayer(knn_DeepLearning_Layer *in, size_t size)
{
  knn_DeepLearning_Layer *layer = knn_DeepLearning_addConnectedLayer(in, size, "");
  layer->type = knn_DeepLearning_Softmax;
  memset(layer->bias, 0, layer->nodes.length * sizeof(float));
  return layer;
}
knn_DeepLearning_Layer *knn_DeepLearning_addSoftmaxLayer_layout(knn_DeepLearning_Layer *in, knnArray layout)
{
  knn_DeepLearning_Layer *layer = knn_DeepLearning_addConnectedLayer_layout(in, layout, "");
  layer->type = knn_DeepLearning_Softmax;
  memset(layer->bias, 0, layer->nodes.length * sizeof(float));
  return layer;
}

void knn_DeepLearning_clear(knn_DeepLearning *net)
{
  for (knn_DeepLearning_Layer *current = net->input; current != NULL; current = current->next)
  {
    for (uint i = 0; i < current->nodes.length; i++)
    {
      array_get(knn_Node, current->nodes, i).value = 0;
      array_get(knn_Node, current->nodes, i).calculated = false;
    }
  }
}

knn_DeepLearning knn_DeepLearning_createNet(uint number_layers, ...) { nImpl_err; }
float **knn_DeepLearning_getLayerValuesPointers(knn_DeepLearning_Layer *layer)
{
  float **arr = malloc(sizeof(float *) * layer->nodes.length);
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    arr[i] = &array_get(knn_Node, layer->nodes, i).value;
  }
  return arr;
}

