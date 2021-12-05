#include "knn_DeepLearning.h"

#define derivate(f, x) ((f(x + 0.0001) - f(x)) / 0.0001)

static void backFullyConnected(knn_DeepLearning_Layer *layer, float *layerCost, float range)
{
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

static void backPropLayer(knn_DeepLearning_Layer *layer, float *layerCost, float range)
{
  switch (layer->type)
  {
  case FullyConnected:
  case Linear:
    backFullyConnected(layer, layerCost, range);
    break;

  default:
    break;
  }
  if (layer->prev)
    backPropLayer(layer->prev, layerCost, range);
}

void knn_backpropagate(knn_DeepLearning *net, knn_DeepLearning_Backpropagator info)
{
  size_t maxNodeLength = 0;
  for (knn_DeepLearning_Layer *layer = &net->input; layer != &net->output; layer = layer->next)
  {
    maxNodeLength = max(maxNodeLength, layer->nodes.length);
  }

  float *cost = calloc(maxNodeLength, sizeof(float));
  for (uint i = 0; i < net->output.nodes.length; i++)
  {
    cost[i] = info.costFunction(array_get(knn_Node, net->output.nodes, i).value, info.expextedOutput[i]);
  }
  backPropLayer(&net->output, cost, 0.1);

  free(cost);
}
