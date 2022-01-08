#include "knn.h"
#define print ch_println

static float randf_knn()
{
  return randf() * 2 - 1;
}

knn_NEAT knn_NEAT_createNet(uint inputSize, uint outputSize)
{
  knn_NEAT net;
  net.input = inputSize;
  net.output = outputSize;
  net.nodes = (struct Array){mallocArr(knn_Node, net.input + net.output + 1), net.input + net.output + 1, net.input + net.output + 1};
  for (uint i = 0; i < net.nodes.length; i++)
  {
    knn_Node a = {0, false};
    array_get(knn_Node, net.nodes, i) = a;
  }
  net.links = (struct Array){mallocArr(knn_NEAT_Link, net.input * net.output), net.input * net.output, net.input * net.output};
  for (uint i = 0; i < outputSize; i++)
  {
    for (uint j = 0; j < inputSize; j++)
    {
      float w = randf();
      knn_NEAT_Link l = {j, i, w, i * inputSize + j, true, false};
      if (randf() < 0.5)
      {
        l.enabled = false;
      }
      net.linksArr[i * inputSize + j] = l;
    }
  }
  return net;
}
knn_NEAT knn_NEAT_clone(knn_NEAT *net)
{
  knn_NEAT clone = {.input = net->input, .output = net->output};
  clone.nodes = array_clone(knn_Node, net->nodes);
  clone.links = array_clone(knn_NEAT_Link, net->links);
  return clone;
}
void knn_NEAT_free(knn_NEAT *net)
{
  array_ffree(&net->nodes);
  array_ffree(&net->links);
}

knn_Node *knn_NEAT_getNetInput(knn_NEAT *net)
{
  return net->nodes.arr;
}
knn_Node *knn_NEAT_getNetOutput(knn_NEAT *net)
{
  return array_arr(knn_Node, net->nodes) + net->input;
}
float **knn_NEAT_getInput(knn_NEAT *net)
{
  float **r = malloc(sizeof(float *) * net->input);
  for (uint i = 0; i < net->input; i++)
  {
    r[i] = &array_get(knn_Node, net->nodes, i).value;
  }
  return r;
}
float **knn_NEAT_getOutput(knn_NEAT *net)
{
  float **r = malloc(sizeof(float *) * net->output);
  for (uint i = 0; i < net->output; i++)
  {
    r[i] = &array_get(knn_Node, net->nodes, i + net->input).value;
  }
  return r;
}
void knn_NEAT_setInput(knn_NEAT *net, float *val)
{
  for (uint i = 0; i < net->input; i++)
  {
    array_get(knn_Node, net->nodes, i).value = val[i];
  }
}
// HID=0,IN=1,OUT=2,BIAS=3,ACT=4
uint knn_NEAT_getNodeType(knn_NEAT *net, uint node)
{
  if (node < net->input)
    return 1;
  else if (node < net->output)
    return 2;
  else if (node == net->input + net->output)
    return 3;
  else
    return 0;
}

bool knn_NEAT_isRecuriveLink(knn_NEAT *net, uint in, uint out)
{
  bool res = false;
  if (in == out)
  {
    return true;
  }
  for (uint i = 0; i < net->links.length; i++)
  {
    if (!res && (!net->linksArr[i].recursive) && net->linksArr[i].output == in)
    {
      if (net->linksArr[i].input == out)
      {
        return true;
      }
      else
      {
        res = knn_NEAT_isRecuriveLink(net, in, net->linksArr[i].input);
      }
    }
    if (res)
    {
      return true;
    }
  }
  return res;
}
uint knn_NEAT_getLinkInnov(uint input, uint output, struct Array *linksHistoryArr)
{
  knn_NEAT_Link *linksHistory = linksHistoryArr->arr;
  for (uint i = 0; i < linksHistoryArr->length; i++)
  {
    if (linksHistory[i].input == input && linksHistory[i].output == output)
    {
      return linksHistory[i].innov;
    }
  }
  knn_NEAT_Link l = {input, output, 0, linksHistoryArr->length, false, false};
  array_addElement(linksHistoryArr, l);
  return linksHistoryArr->length - 1;
}

void knn_NEAT_calculateNetNode(knn_NEAT *net, uint node)
{
  if (array_get(knn_Node, net->nodes, node).calculated)
    return;
  for (uint i = 0; i < net->links.length; i++)
  {
    if (net->linksArr[i].enabled && !net->linksArr[i].recursive && net->linksArr[i].output == node)
    {
      if (!array_get(knn_Node, net->nodes, net->linksArr[i].input).calculated)
      {
        knn_NEAT_calculateNetNode(net, net->linksArr[i].input);
      }
      array_get(knn_Node, net->nodes, net->linksArr[i].output).value += array_get(knn_Node, net->nodes, net->linksArr[i].input).value * net->linksArr[i].weight;
    }
  }
  array_get(knn_Node, net->nodes, node).value = NEAT_Function(array_get(knn_Node, net->nodes, node).value);
  array_get(knn_Node, net->nodes, node).calculated = true;
}

float knn_NEAT_calculateNetDelta(knn_NEAT *n1, knn_NEAT *n2, float maxDelta)
{
  uint E = 0;
  uint D = 0;
  uint W = 0;
  uint N = n1->links.length;
  if (n2->links.length > n1->links.length)
  {
    N = n2->links.length;
  }
  uint similarGenes = 0;
  uint mostRecentInnovN1 = 0;
  uint mostRecentInnovN2 = 0;
  // maxInnov
  for (uint i = 0; i < n1->links.length; i++)
  {
    if (n1->linksArr[i].innov > mostRecentInnovN1)
    {
      mostRecentInnovN1 = n1->linksArr[i].innov;
    }
  }
  for (uint i = 0; i < n2->links.length; i++)
  {
    if (n2->linksArr[i].innov > mostRecentInnovN2)
    {
      mostRecentInnovN2 = n2->linksArr[i].innov;
    }
  }
  // E, D and W
  bool net2LinkD[n2->links.length];
  for (uint i = 0; i < n2->links.length; i++)
  {
    net2LinkD[i] = true;
  }
  for (uint i = 0; i < n1->links.length; i++)
  {
    if (((NEAT_c1 * E) / N + (NEAT_c2 * D) / N) > maxDelta)
    {
      return ((NEAT_c1 * E) / N + (NEAT_c2 * D) / N);
    }
    if (n1->linksArr[i].innov > mostRecentInnovN2)
    {
      E++;
    }
    else
    {
      D++;
      for (int j = 0; j < n2->links.length; j++)
      {
        if (n1->linksArr[i].innov == n2->linksArr[j].innov)
        {
          D--;
          W += abs(n1->linksArr[i].weight - n2->linksArr[j].weight);
          net2LinkD[j] = false;
          similarGenes++;
          break;
        }
      }
    }
  }
  W /= similarGenes;
  for (int i = 0; i < n2->links.length; i++)
  {
    if (((NEAT_c1 * E) / N + (NEAT_c2 * D) / N + NEAT_c3 * W) >= maxDelta)
    {
      return ((NEAT_c1 * E) / N + (NEAT_c2 * D) / N + NEAT_c3 * W);
    }
    if (n2->linksArr[i].innov > mostRecentInnovN1)
    {
      E++;
    }
    else
    {
      if (net2LinkD[i])
      {
        D++;
      }
    }
  }
  return ((NEAT_c1 * E) + (NEAT_c2 * D) + NEAT_c3 * W);
}

void knn_NEAT_calculateNet(knn_NEAT *net)
{
  float *newValues = mallocArr(float, net->nodes.length);
  for (uint i = 0; i < net->nodes.length; i++)
  {
    if (knn_NEAT_getNodeType(net, i) == 1 || knn_NEAT_getNodeType(net, i) == 3)
    {
      newValues[i] = array_get(knn_Node, net->nodes, i).value;
      array_get(knn_Node, net->nodes, i).calculated = true;
    }
    else
    {
      array_get(knn_Node, net->nodes, i).calculated = false;
    }
  }
  for (uint i = 0; i < net->links.length; i++)
  {
    if (net->linksArr[i].recursive)
    {
      newValues[net->linksArr[i].output] += array_get(knn_Node, net->nodes, net->linksArr[i].input).value;
    }
  }
  for (uint i = 0; i < net->nodes.length; i++)
  {
    array_get(knn_Node, net->nodes, i).value = newValues[i];
  }
  free(newValues);
  for (uint i = net->input; i < net->input + net->output; i++)
  {
    knn_NEAT_calculateNetNode(net, i);
  }
}
knn_NEAT knn_NEAT_breedNets(knn_NEAT *parent1, knn_NEAT *parent2, float speciesRatio, struct Array *linksHistory)
{
  knn_NEAT offspring = knn_NEAT_clone(parent1);
  // structure crossover
  if (randf() < NEAT_oddMutationWithCrossover)
  {
    for (uint i = 0; i < offspring.links.length; i++)
    {
      for (uint j = 0; j < parent2->links.length; j++)
      {
        if (offspring.linksArr[i].innov == parent2->linksArr[j].innov)
        {
          if (randf() < 0.5)
          {
            offspring.linksArr[i].weight = parent2->linksArr[j].weight;
          }
        }
      }
    }
    // add disjoint/excess link
    if (randf() < NEAT_oddAddDisjointExcessGene)
    {
      if (parent2->nodes.length > parent1->nodes.length)
      {
        for (int i = parent1->nodes.length; i < parent2->nodes.length; i++)
        {
          knn_Node node = {0.f, false};
          array_addElement(&offspring.nodes, node);
        }
      }
      for (int i = 0; i < parent2->links.length; i++)
      {
        bool included = false;
        for (int j = 0; j < offspring.links.length; j++)
        {
          if (parent2->linksArr[i].innov == offspring.linksArr[j].innov)
          {
            included = true;
            break;
          }
        }
        if (!included)
        {
          if (parent2->linksArr[i].recursive ||
              knn_NEAT_isRecuriveLink(&offspring, parent2->linksArr[i].input, parent2->linksArr[i].output) || knn_NEAT_getNodeType(&offspring, parent2->linksArr[i].input) == 2)
          {
            knn_NEAT_Link link = {parent2->linksArr[i].input, parent2->linksArr[i].output, parent2->linksArr[i].weight, parent2->linksArr[i].innov, true, true};
            array_addElement(&offspring.links, link);
          }
          else
          {
            knn_NEAT_Link link = {parent2->linksArr[i].input, parent2->linksArr[i].output, parent2->linksArr[i].weight, parent2->linksArr[i].innov, true, parent2->linksArr[i].recursive};
            array_addElement(&offspring.links, link);
          }
        }
      }
    }
    // disabled gene transmited
    for (int i = 0; i < offspring.links.length; i++)
    {
      if (!offspring.linksArr[i].enabled)
      {
        if (!(randf() < NEAT_oddDisabledGeneTransmited))
        {
          offspring.linksArr[i].enabled = true;
        }
      }
    }
  }

  // mutate structure
  // add node
  // condition is only here to remove from program simply
  if (1)
  {
    uint i = randf() * offspring.links.length;
    if (randf() < NEAT_oddAddNode && !offspring.linksArr[i].recursive && offspring.linksArr[i].enabled)
    {
      offspring.linksArr[i].enabled = false;
      knn_Node node = {0.f, false};
      knn_NEAT_Link l1 = {offspring.linksArr[i].input, offspring.nodes.length, knn_NEAT_getLinkInnov(offspring.linksArr[i].input, offspring.nodes.length, linksHistory), 1.f, true, false};
      knn_NEAT_Link l2 = {offspring.nodes.length, offspring.linksArr[i].output, knn_NEAT_getLinkInnov(offspring.nodes.length, offspring.linksArr[i].output, linksHistory), offspring.linksArr[i].weight, true, false};
      array_addElement(&offspring.nodes, node);
      array_addElement(&offspring.links, l1);
      array_addElement(&offspring.links, l2);

      // offspring.nodes.push_back({0.0, 0.0, NeuralNet<F>::Node::HID, false});
      // offspring.links.push_back({offspring.linksArr[i].input, (u32)(offspring.nodes.length - 1), getInnovFromHistory<F>(offspring.linksArr[i].input, offspring.nodes.length - 1, linksHistory), 1.0, true, false, false});
      // offspring.links.push_back({(u32)(offspring.nodes.length - 1), offspring.linksArr[i].output, getInnovFromHistory<F>(offspring.nodes.length - 1, offspring.linksArr[i].output, linksHistory), offspring.linksArr[i].weight, true, false, false});
    }
  }

  // add link
  if (1)
  {
    bool mutate = false;
    if (speciesRatio < NEAT_rationBigSpecies)
    {
      mutate = randf() < NEAT_oddNewLinkSmallSpecies;
    }
    else
    {
      mutate = randf() < NEAT_oddNewLinkBigSpecies;
    }
    if (mutate)
    {
      uint j = randf() * offspring.nodes.length;
      while (knn_NEAT_getNodeType(&offspring, j) == 1 || knn_NEAT_getNodeType(&offspring, j) == 3)
      {
        j = randf() * offspring.nodes.length;
      }
      uint i = randf() * offspring.nodes.length;
      bool exists = false;
      for (uint l = 0; l < offspring.links.length; l++)
      {
        if (offspring.linksArr[l].input == i && offspring.linksArr[l].output == j)
        {
          exists = true;
        }
      }
      if (!exists)
      {
        if (knn_NEAT_isRecuriveLink(&offspring, i, j) || knn_NEAT_getNodeType(&offspring, i) == 2)
        {
          if (randf() < NEAT_oddRecursiveLink)
          {
            knn_NEAT_Link link = {i, j, NEAT_mutationRange * (randf() * 2 - 1), knn_NEAT_getLinkInnov(i, j, linksHistory), true, true};
            array_addElement(&offspring.links, link);
            // offspring.links.push_back({i, j, getInnovFromHistory<F>(i, j, linksHistory), NEAT_mutationRange * (randf() * 2 - 1), true, true, false});
          }
        }
        else
        {
          if (knn_NEAT_getNodeType(&offspring, i) != 3 || randf() < NEAT_oddBias)
          {
            knn_NEAT_Link link = {i, j, NEAT_mutationRange * (randf() * 2 - 1), knn_NEAT_getLinkInnov(i, j, linksHistory), true, false};
            array_addElement(&offspring.links, link);
            // offspring.links.push_back({i, j, getInnovFromHistory<F>(i, j, linksHistory), NEAT_mutationRange * (randf() * 2 - 1), true, false, false});
          }
        }
      }
    }
  }

  // change weights
  for (uint i = 0; i < offspring.links.length; i++)
  {
    if (randf() < NEAT_oddWeightMutate)
    {
      if (randf() <= NEAT_oddWeightRandomMutate)
      {
        offspring.linksArr[i].weight = NEAT_mutationRange * (randf() * 2 - 1);
      }
      else
      {
        offspring.linksArr[i].weight += NEAT_mutationRange * (randf());
        if (abs(offspring.linksArr[i].weight) > NEAT_mutationRange)
        {
          offspring.linksArr[i].weight = abs(offspring.linksArr[i].weight) / offspring.linksArr[i].weight * NEAT_mutationRange;
        }
      }
    }
  }
  return offspring;
}

static double linearFunction(double x)
{
  return x;
}
static double sigmoidFunction(double x)
{
  return 1 / (1 + exp(-x));
}
static double softsignFunction(double x)
{
  return x / (1 + abs(x));
}
static double softplusFunction(double x)
{
  return log(exp(x) + 1);
}
static double swishFunction(double x)
{
  return x / (1 + exp(x));
}
static double reluFunction(double x)
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
    return &tanh;
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
  layer->type = Input;
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
  layer->type = Input;
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
  layer->type = Linear;
  layer->nodes = (struct Array){mallocArr(knn_Node, in->nodes.MAX_LENGTH), in->nodes.length, in->nodes.MAX_LENGTH};
  layer->fullyConnected.links = (struct Array){mallocArr(knn_DeepLearning_Layer, layer->nodes.MAX_LENGTH), layer->nodes.MAX_LENGTH, layer->nodes.MAX_LENGTH};
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    knn_DeepLearning_Link *l = &array_get(knn_DeepLearning_Link, layer->fullyConnected.links, i);
    l->input = i;
    l->output = i;
    l->weight = randf_knn();
  }
  layer->bias = calloc(in->nodes.MAX_LENGTH, sizeof(double));
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
  layer->type = Linear;
  layer->nodes = (struct Array){mallocArr(knn_Node, size), size, size};
  layer->fullyConnected.preFunctionNodeValues = calloc(layer->nodes.length, sizeof(double));
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
  layer->bias = calloc(size, sizeof(double));
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
  layer->type = Linear;
  layer->nodes.MAX_LENGTH = 1;
  for (uint i = 0; i < layout.length; i++)
  {
    layer->nodes.MAX_LENGTH *= array_get(uint, layout, i);
  }
  layer->nodes = (struct Array){calloc(layer->nodes.MAX_LENGTH, sizeof(knn_Node)), layer->nodes.MAX_LENGTH, layer->nodes.MAX_LENGTH};
  layer->fullyConnected.preFunctionNodeValues = calloc(layer->nodes.length, sizeof(double));
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
  layer->bias = calloc(layer->nodes.length, sizeof(double));
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
  layer->type = Convolutionnal;
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
  layer->fullyConnected.preFunctionNodeValues = calloc(layer->nodes.length, sizeof(double));

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
  nImpl_err;
  return NULL;
}
knn_DeepLearning_Layer *knn_DeepLearning_addSoftmaxLayer(knn_DeepLearning_Layer *in, size_t size)
{
  knn_DeepLearning_Layer *layer = knn_DeepLearning_addConnectedLayer(in, size, "");
  layer->type = Softmax;
  memset(layer->bias, 0, layer->nodes.length * sizeof(double));
  return layer;
}
knn_DeepLearning_Layer *knn_DeepLearning_addSoftmaxLayer_layout(knn_DeepLearning_Layer *in, knnArray layout)
{
  knn_DeepLearning_Layer *layer = knn_DeepLearning_addConnectedLayer_layout(in, layout, "");
  layer->type = Softmax;
  memset(layer->bias, 0, layer->nodes.length * sizeof(double));
  return layer;
}

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
static void calcSoftmax(knn_DeepLearning_Layer *layer)
{
  calcFullyConnected(layer);
  double sum = 0;
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    double d = array_get(knn_Node, layer->nodes, i).value;
    array_get(knn_Node, layer->nodes, i).value = exp(array_get(knn_Node, layer->nodes, i).value);
    if (isinf(array_get(knn_Node, layer->nodes, i).value)){
      array_get(knn_Node, layer->nodes, i).value=10000;
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
  if (!layer->type != Input)
  {
    for (uint i = 0; i < layer->nodes.length; i++)
    {
      array_get(knn_Node, layer->nodes, i).value = 0;
      array_get(knn_Node, layer->nodes, i).calculated = false;
    }
  }
  switch (layer->type)
  {
  case FullyConnected:
  case Linear:
    calcFullyConnected(layer);
    break;
  case Convolutionnal:
    calcConv(layer);
    break;
  case Softmax:
    calcSoftmax(layer);
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
