#include "knn_DeepLearning.h"

#define derivate(f, x) ((f(x + 0.0001) - f(x)) / 0.0001)

static void backFullyConnected(knn_DeepLearning_Layer *layer, float *layerCost, float range)
{
  // previous layer cost
  float *pLC = calloc(layer->prev->nodes.MAX_LENGTH, sizeof(float));
  for (uint i = 0; i < layer->fullyConnected.links.length; i++)
  {
    knn_DeepLearning_Link *link = &array_get(knn_DeepLearning_Link, layer->fullyConnected.links, i);
    layerCost[link->output] = derivate(layer->fullyConnected.function, layerCost[link->output]);
    pLC[link->input] += layerCost[link->output] * link->weight;
    link->weight -= array_get(knn_Node, layer->prev->nodes, link->input).value * layerCost[link->output] * range;
  }
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    array_get(float, layer->bias, i) -= layerCost[i] * range;
  }
  memcpy(pLC, layerCost, layer->prev->nodes.MAX_LENGTH * sizeof(float));
  free(pLC);
}

static void backConvLayer_recurse(uint index,knnArray pos,knn_DeepLearning_Layer*layer,float *linksDeriv,float*prevNodesDeriv,float *layerCost)
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
static void backConvLayer(knn_DeepLearning_Layer *layer, float *layerCost, float range)
{
  float *linksDeriv = calloc(layer->convolutionnal.links.length, sizeof(float));
  float *prevNodesDeriv = calloc(layer->prev->nodes.length, sizeof(float));
  knnArray pos = array_init(uint, layer->layout.length);
  backConvLayer_recurse(0, pos, layer, linksDeriv, prevNodesDeriv, layerCost);

  for(uint i=0;i<layer->convolutionnal.links.length;i++){
    array_get(knn_DeepLearning_Link, layer->convolutionnal.links, i).weight -= linksDeriv[i] * range;
  }
  memcpy(layerCost, prevNodesDeriv, sizeof(float) * layer->prev->nodes.length);
  array_ffree(&pos);
  free(prevNodesDeriv);
  free(linksDeriv);
}

static void backPropLayer(knn_DeepLearning_Layer *layer, float *layerCost, float range)
{
  switch (layer->type)
  {
  case FullyConnected:
  case Linear:
    backFullyConnected(layer, layerCost, range);
    break;
  case Convolutionnal:
    backConvLayer(layer, layerCost, range);
    break;
  case Input:
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
  for (knn_DeepLearning_Layer *layer = net->input; layer != net->output; layer = layer->next)
  {
    maxNodeLength = max(maxNodeLength, layer->nodes.length);
  }

  float *cost = calloc(maxNodeLength, sizeof(float));
  for (uint i = 0; i < net->output->nodes.length; i++)
  {
    cost[i] = info.costFunction(array_get(knn_Node, net->output->nodes, i).value, info.expextedOutput[i]);
  }
  backPropLayer(net->output, cost, 0.1);

  free(cost);
}
