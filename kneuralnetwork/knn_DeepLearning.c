#define CHEVAN_UTILS_MACRO_MAGIC
#include "knn_DeepLearning.h"
#include "../chevan_thread.h"

#define derivate(f, x) ((f(x + 0.0001) - f(x)) / 0.0001)

#define BACKPROP_RANGE 1

struct knn_DeepLearning_Backpropagator_Internal
{
  uint nDeriv;
  double *weightsChange;
  double derivSquareSum;
};
void knn_DeepLearning_initBackpropagateInternal(knn_DeepLearning *net, knn_DeepLearning_Backpropagator *backpropagator)
{
  backpropagator->internal = calloc(1, sizeof(knn_DeepLearning_Backpropagator_Internal));
}

static void storeWeightsChange_FullyConnected(knn_DeepLearning_Layer *layer, double **weightsChange, double *layerCost)
{
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    layerCost[i] *= derivate(layer->fullyConnected.function, layer->fullyConnected.preFunctionNodeValues[i]);
  }

  // previous layer cost
  double *pLC = calloc(layer->prev->nodes.length, sizeof(double));
  for (uint i = 0; i < layer->fullyConnected.links.length; i++)
  {
    knn_DeepLearning_Link *link = &array_get(knn_DeepLearning_Link, layer->fullyConnected.links, i);
    pLC[link->input] += layerCost[link->output] * link->weight;
    (*weightsChange)[i] += array_get(knn_Node, layer->prev->nodes, link->input).value * layerCost[link->output];
  }
  *weightsChange += layer->fullyConnected.links.length;
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    (*weightsChange)[i] = layerCost[i];
  }
  *weightsChange += layer->nodes.length;
  memcpy(layerCost, pLC, layer->prev->nodes.length * sizeof(double));
  free(pLC);
}
static void storeWeightsChange_Softmax(knn_DeepLearning_Layer *layer, double **weightsChange, double *layerCost)
{
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    double tmp = 0;
    for (uint j = 0; j < layer->nodes.length; j++)
    {
      double sj = array_get(knn_Node, layer->nodes, j).value;
      double si = (i == j ? 1 : 0) - array_get(knn_Node, layer->nodes, i).value;
      tmp += sj * si;
    }
    layerCost[i] *= tmp;
  }

  // previous layer cost
  double *pLC = calloc(layer->prev->nodes.length, sizeof(double));
  for (uint i = 0; i < layer->fullyConnected.links.length; i++)
  {
    knn_DeepLearning_Link *link = &array_get(knn_DeepLearning_Link, layer->fullyConnected.links, i);
    pLC[link->input] += layerCost[link->output] * link->weight;
    (*weightsChange)[i] += array_get(knn_Node, layer->prev->nodes, link->input).value * layerCost[link->output];
  }
  *weightsChange += layer->fullyConnected.links.length;
  memcpy(layerCost, pLC, layer->prev->nodes.length * sizeof(double));
  free(pLC);
}
static void storeWeightsChange(knn_DeepLearning_Layer *layer, double *weightsChange, double *layerCost)
{
  switch (layer->type)
  {
  case Input:
    break;
  case FullyConnected:
  case Linear:
    storeWeightsChange_FullyConnected(layer, &weightsChange, layerCost);
    break;
  case Convolutionnal:
    nImpl_err;
    break;
  case Softmax:
    storeWeightsChange_Softmax(layer, &weightsChange, layerCost);
    break;
  default:
    nImpl_err;
    break;
  }
  if (layer->prev)
    storeWeightsChange(layer->prev, weightsChange, layerCost);
}

static void backFullyConnected(knn_DeepLearning_Layer *layer, double *layerCost, double range)
{
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    layerCost[i] *= derivate(layer->fullyConnected.function, layer->fullyConnected.preFunctionNodeValues[i]);
  }

  // previous layer cost
  double *pLC = calloc(layer->prev->nodes.length, sizeof(double));
  for (uint i = 0; i < layer->fullyConnected.links.length; i++)
  {
    knn_DeepLearning_Link *link = &array_get(knn_DeepLearning_Link, layer->fullyConnected.links, i);
    pLC[link->input] += layerCost[link->output] * link->weight;
    link->weight -= range * array_get(knn_Node, layer->prev->nodes, link->input).value * layerCost[link->output];
  }
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    layer->bias[i] -= range * layerCost[i];
  }
  memcpy(layerCost, pLC, layer->prev->nodes.length * sizeof(double));
  free(pLC);
}
static void backConvLayer_recurse(uint index, knnArray pos, knn_DeepLearning_Layer *layer, double *linksDeriv, double *prevNodesDeriv, double *layerCost)
{
  if (index == pos.length)
  {
    uint inOffset = knn_DeepLearning_getCorrespondingIndexArr(pos, layer->prev->layout);
    uint outOffset = knn_DeepLearning_getCorrespondingIndexArr(pos, layer->layout);
    for (uint i = 0; i < layer->convolutionnal.links.length; i++)
    {
      uint linkIn = array_get(knn_DeepLearning_Link, layer->convolutionnal.links, i).input + inOffset;
      linksDeriv[i] += array_get(knn_Node, layer->prev->nodes, linkIn).value * layerCost[outOffset];
      prevNodesDeriv[linkIn] += array_get(knn_DeepLearning_Link, layer->convolutionnal.links, i).weight * layerCost[outOffset];
    }
    return;
  }
  for (uint i = 0; i < array_get(uint, layer->layout, index); i++)
  {
    array_get(uint, pos, index) = i;
    backConvLayer_recurse(index + 1, pos, layer, linksDeriv, prevNodesDeriv, layerCost);
  }
}
static void backConvLayer(knn_DeepLearning_Layer *layer, double *layerCost, double range)
{
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    layerCost[i] = derivate(layer->convolutionnal.function, layerCost[i]);
  }
  double *linksDeriv = calloc(layer->convolutionnal.links.length, sizeof(double));
  double *prevNodesDeriv = calloc(layer->prev->nodes.length, sizeof(double));
  knnArray pos = array_init(uint, layer->layout.length);
  backConvLayer_recurse(0, pos, layer, linksDeriv, prevNodesDeriv, layerCost);

  for (uint i = 0; i < layer->convolutionnal.links.length; i++)
  {
    array_get(knn_DeepLearning_Link, layer->convolutionnal.links, i).weight -= linksDeriv[i] * range;
  }
  memcpy(layerCost, prevNodesDeriv, sizeof(double) * layer->prev->nodes.length);
  array_ffree(&pos);
  free(prevNodesDeriv);
  free(linksDeriv);
}
static void backSoftmax(knn_DeepLearning_Layer *layer, double *layerCost, double range)
{
  double *tmpCost = calloc(layer->nodes.length, sizeof(double));
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    for (uint j = 0; j < layer->nodes.length; j++)
    {
      tmpCost[i] += array_get(knn_Node, layer->nodes, i).value * ((i == j ? 1 : 0) - array_get(knn_Node, layer->nodes, j).value);
    }
    tmpCost[i] *= layerCost[i];
  }

  double *pLC = calloc(layer->prev->nodes.length, sizeof(double));
  for (uint i = 0; i < layer->fullyConnected.links.length; i++)
  {
    knn_DeepLearning_Link *link = &array_get(knn_DeepLearning_Link, layer->fullyConnected.links, i);
    pLC[link->input] += tmpCost[link->output] * link->weight;
    link->weight -= range * array_get(knn_Node, layer->prev->nodes, link->input).value * tmpCost[link->output];
  }
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    layer->bias[i] -= range * tmpCost[i];
  }
  memcpy(layerCost, pLC, layer->prev->nodes.length * sizeof(double));
  free(pLC);
  free(tmpCost);
}
static void backPropLayer(knn_DeepLearning_Layer *layer, double *layerCost, double range)
{
  switch (layer->type)
  {
  case FullyConnected:
  case Linear:
    backFullyConnected(layer, layerCost, range);
    break;
  case Convolutionnal:
    nImpl_err;
    backConvLayer(layer, layerCost, range);
    break;
  case Input:
    break;
  case Softmax:
    backSoftmax(layer, layerCost, range);
    break;
  default:
    nImpl_err;
    break;
  }
  if (layer->prev)
    backPropLayer(layer->prev, layerCost, range);
}

void knn_DeepLearning_backpropagate(knn_DeepLearning *net, knn_DeepLearning_Backpropagator info)
{
  size_t maxNodeLength = 0;
  for (knn_DeepLearning_Layer *layer = net->input; layer != NULL; layer = layer->next)
  {
    maxNodeLength = max(maxNodeLength, layer->nodes.length);
  }

  double *deriv = calloc(maxNodeLength, sizeof(double));
  double derivSquareSum = 0;
  for (uint i = 0; i < net->output->nodes.length; i++)
  {
    if (info.costFunction == knn_DeepLearning_meanSquaredError)
    {
      deriv[i] = array_get(knn_Node, net->output->nodes, i).value - info.expextedOutput[i];
    }
    else
    {
      deriv[i] = (info.costFunction(array_get(knn_Node, net->output->nodes, i).value + 0.0001, info.expextedOutput[i]) -
                  info.costFunction(array_get(knn_Node, net->output->nodes, i).value, info.expextedOutput[i])) /
                 0.0001;
    }
    derivSquareSum += deriv[i] * deriv[i];
  }
  // double cost = knn_DeepLearning_getCost(net, info);
  backPropLayer(net->output, deriv, BACKPROP_RANGE / sqrt(derivSquareSum));
  free(deriv);
}

static void backPropagateFullyConnectedOnStored(knn_DeepLearning_Layer *layer, double **weightsChange, double range)
{
  for (uint i = 0; i < layer->fullyConnected.links.length; i++)
  {
    knn_DeepLearning_Link *link = &array_get(knn_DeepLearning_Link, layer->fullyConnected.links, i);
    link->weight -= range * (*weightsChange)[i];
    assert(isfinite(link->weight));
  }
  *weightsChange += layer->fullyConnected.links.length;
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    layer->bias[i] -= range * (*weightsChange)[i];
    assert(isfinite(layer->bias[i]));
  }
  *weightsChange += layer->nodes.length;
}
static void backPropagateSoftmaxOnStored(knn_DeepLearning_Layer *layer, double **weightsChange, double range)
{
  for (uint i = 0; i < layer->fullyConnected.links.length; i++)
  {
    knn_DeepLearning_Link *link = &array_get(knn_DeepLearning_Link, layer->fullyConnected.links, i);
    link->weight -= range * (*weightsChange)[i];
    assert(isfinite(link->weight));
  }
  *weightsChange += layer->fullyConnected.links.length;
}
static void backPropagateOnStored(knn_DeepLearning_Layer *layer, double *weightsChange, double range)
{
  switch (layer->type)
  {
  case Input:
    break;
  case FullyConnected:
  case Linear:
    backPropagateFullyConnectedOnStored(layer, &weightsChange, range);
    break;
  case Convolutionnal:
    nImpl_err;
    break;
  case Softmax:
    backPropagateSoftmaxOnStored(layer, &weightsChange, range);
    break;
  default:
    nImpl_err;
    break;
  }
  if (layer->prev)
    backPropagateOnStored(layer->prev, weightsChange, range);
}

void knn_DeepLearning_backpropagateStore(knn_DeepLearning *net, knn_DeepLearning_Backpropagator info, uint max)
{
  info.internal->nDeriv++;
  size_t sum = 0;
  size_t maxNodeLength = 0;
  for (knn_DeepLearning_Layer *l = net->input; l != NULL; l = l->next)
  {
    maxNodeLength = max(maxNodeLength, l->nodes.length);
    switch (l->type)
    {
    case Input:
      break;
    case FullyConnected:
    case Linear:
      sum += l->fullyConnected.links.length + l->nodes.length;
      break;
    case Softmax:
      sum += l->fullyConnected.links.length;
      break;
    case Convolutionnal:
      nImpl_err;
      break;
    default:
      nImpl_err;
      break;
    }
  }
  double *wChange = calloc(sum, sizeof(double));
  double *deriv = calloc(maxNodeLength, sizeof(double));
  double derivSquareSum = 0;

  for (uint i = 0; i < net->output->nodes.length; i++)
  {
    if (info.costFunction == knn_DeepLearning_meanSquaredError)
    {
      deriv[i] = array_get(knn_Node, net->output->nodes, i).value - info.expextedOutput[i];
    }
    else if (info.costFunction == knn_DeepLearning_crossEntropyLoss)
    {
      ch_println("cross");
      assert(0);
    }
    else
    {
      deriv[i] = (info.costFunction(array_get(knn_Node, net->output->nodes, i).value + 0.0001, info.expextedOutput[i]) -
                  info.costFunction(array_get(knn_Node, net->output->nodes, i).value, info.expextedOutput[i])) /
                 0.0001;
    }
    derivSquareSum += deriv[i] * deriv[i];
  }
  info.internal->derivSquareSum += derivSquareSum;

  storeWeightsChange(net->output, wChange, deriv);
  if (!info.internal->weightsChange)
  {
    info.internal->weightsChange = wChange;
  }
  else
  {
    for (uint i = 0; i < sum; i++)
    {
      info.internal->weightsChange[i] += wChange[i];
    }
    free(wChange);
  }
  if (info.internal->nDeriv >= max)
  {
    for (uint i = 0; i < sum; i++)
    {
      info.internal->weightsChange[i] /= max;
    }
    info.internal->derivSquareSum /= max;
    backPropagateOnStored(net->output, info.internal->weightsChange, BACKPROP_RANGE / sqrt(info.internal->derivSquareSum));
    info.internal->nDeriv = 0;
    memset(info.internal->weightsChange, 0, sizeof(double) * sum);
  }
}
