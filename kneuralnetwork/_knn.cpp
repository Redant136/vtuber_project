#include "knn.h"

static bool knn_NEAT_isRecuriveLink(knn_NEAT *net, uint in, uint out)
{
  bool res = false;
  if (in == out)
  {
    return true;
  }
  for (uint i = 0; i < net->links.length; i++)
  {
    if (!res && (!net->links[i].recursive) && net->links[i].output == in)
    {
      if (net->links[i].input == out)
      {
        return true;
      }
      else
      {
        res = knn_NEAT_isRecuriveLink(net, in, net->links[i].input);
      }
    }
    if (res)
    {
      return true;
    }
  }
  return res;
}
static uint knn_NEAT_getLinkInnov(uint input, uint output, Array<knn_NEAT_Link> *linksHistory)
{
  for (uint i = 0; i < linksHistory->length; i++)
  {
    if (linksHistory->get(i).input == input && linksHistory->get(i).output == output)
    {
      return linksHistory->get(i).innov;
    }
  }
  knn_NEAT_Link l = {input, output, 0, linksHistory->length, false, false};
  linksHistory->addElement(l);
  return linksHistory->length - 1;
}

static void knn_NEAT_calculateNetNode(knn_NEAT *net, uint node)
{
  if (net->nodes[node].calculated)
    return;
  for (uint i = 0; i < net->links.length; i++)
  {
    if (net->links[i].enabled && !net->links[i].recursive && net->links[i].output == node)
    {
      if (!net->nodes[net->links[i].input].calculated)
      {
        knn_NEAT_calculateNetNode(net, net->links[i].input);
      }
      net->nodes[net->links[i].output].value += net->nodes[net->links[i].input].value;
    }
  }
  net->nodes[node].value = NEAT_Function(net->nodes[node].value);
  net->nodes[node].calculated = true;
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
    if (n1->links[i].innov > mostRecentInnovN1)
    {
      mostRecentInnovN1 = n1->links[i].innov;
    }
  }
  for (uint i = 0; i < n2->links.length; i++)
  {
    if (n2->links[i].innov > mostRecentInnovN2)
    {
      mostRecentInnovN2 = n2->links[i].innov;
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
    if (n1->links[i].innov > mostRecentInnovN2)
    {
      E++;
    }
    else
    {
      D++;
      for (int j = 0; j < n2->links.length; j++)
      {
        if (n1->links[i].innov == n2->links[j].innov)
        {
          D--;
          W += abs(n1->links[i].weight - n2->links[j].weight);
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
    if (n2->links[i].innov > mostRecentInnovN1)
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
  float *newValues = new float[net->nodes.length];
  for (uint i = 0; i < net->nodes.length; i++)
  {
    if (knn_NEAT_getNodeType(net, i) == 1 || knn_NEAT_getNodeType(net, i) == 3)
    {
      newValues[i] = net->nodes[i].value;
      net->nodes[i].calculated = true;
    }
    else
    {
      net->nodes[i].calculated = false;
    }
  }
  for (uint i = 0; i < net->links.length; i++)
  {
    if (net->links[i].recursive)
    {
      newValues[net->links[i].output] += net->nodes[net->links[i].input].value;
    }
  }
  for (uint i = 0; i < net->nodes.length; i++)
  {
    net->nodes[i].value = newValues[i];
  }
  free(newValues);
  for (uint i = net->input; i < net->input + net->output; i++)
  {
    knn_NEAT_calculateNetNode(net, i);
  }
}
knn_NEAT knn_NEAT_breedNets(knn_NEAT *parent1, knn_NEAT *parent2, float speciesRatio, Array<knn_NEAT_Link> *linksHistory)
{
  knn_NEAT offspring = knn_NEAT_clone(parent1);
  // structure crossover
  if (randf() < NEAT_oddMutationWithCrossover)
  {
    for (uint i = 0; i < offspring.links.length; i++)
    {
      for (uint j = 0; j < parent2->links.length; j++)
      {
        if (offspring.links[i].innov == parent2->links[j].innov)
        {
          if (randf() < 0.5)
          {
            offspring.links[i].weight = parent2->links[j].weight;
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
          offspring.nodes.addElement(node);
        }
      }
      for (int i = 0; i < parent2->links.length; i++)
      {
        bool included = false;
        for (int j = 0; j < offspring.links.length; j++)
        {
          if (parent2->links[i].innov == offspring.links[j].innov)
          {
            included = true;
            break;
          }
        }
        if (!included)
        {
          if (parent2->links[i].recursive ||
              knn_NEAT_isRecuriveLink(&offspring, parent2->links[i].input, parent2->links[i].output) || knn_NEAT_getNodeType(&offspring, parent2->links[i].input) == 2)
          {
            knn_NEAT_Link link = {parent2->links[i].input, parent2->links[i].output, parent2->links[i].weight, parent2->links[i].innov, true, true};
            offspring.links.addElement(link);
          }
          else
          {
            knn_NEAT_Link link = {parent2->links[i].input, parent2->links[i].output, parent2->links[i].weight, parent2->links[i].innov, true, parent2->links[i].recursive};
            offspring.links.addElement(link);
          }
        }
      }
    }
    // disabled gene transmited
    for (int i = 0; i < offspring.links.length; i++)
    {
      if (!offspring.links[i].enabled)
      {
        if (!(randf() < NEAT_oddDisabledGeneTransmited))
        {
          offspring.links[i].enabled = true;
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
    if (randf() < NEAT_oddAddNode && !offspring.links[i].recursive && offspring.links[i].enabled)
    {
      offspring.links[i].enabled = false;
      knn_Node node = {0.f, false};
      knn_NEAT_Link l1 = {offspring.links[i].input, offspring.nodes.length, knn_NEAT_getLinkInnov(offspring.links[i].input, offspring.nodes.length, linksHistory), 1.f, true, false};
      knn_NEAT_Link l2 = {offspring.nodes.length, offspring.links[i].output, knn_NEAT_getLinkInnov(offspring.nodes.length, offspring.links[i].output, linksHistory), offspring.links[i].weight, true, false};
      offspring.nodes.addElement(node);
      offspring.links.addElement(l1);
      offspring.links.addElement(l2);
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
        if (offspring.links[l].input == i && offspring.links[l].output == j)
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
            offspring.links.addElement(link);
          }
        }
        else
        {
          if (knn_NEAT_getNodeType(&offspring, i) != 3 || randf() < NEAT_oddBias)
          {
            knn_NEAT_Link link = {i, j, NEAT_mutationRange * (randf() * 2 - 1), knn_NEAT_getLinkInnov(i, j, linksHistory), true, false};
            offspring.links.addElement(link);
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
        offspring.links[i].weight = NEAT_mutationRange * (randf() * 2 - 1);
      }
      else
      {
        offspring.links[i].weight += NEAT_mutationRange * (randf());
        if (abs(offspring.links[i].weight) > NEAT_mutationRange)
        {
          offspring.links[i].weight = abs(offspring.links[i].weight) / offspring.links[i].weight * NEAT_mutationRange;
        }
      }
    }
  }
  return offspring;
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
  if (strcmp(name, "linear") == 0 || strcmp(name, "lin") == 0)
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

knn_DeepLearning_Layer *knn_DeepLearning_inputLayer(size_t size)
{
  knn_DeepLearning_Layer *layer = (knn_DeepLearning_Layer *)malloc(sizeof(knn_DeepLearning_Layer));
  layer->type = knn_DeepLearning_Layer::Input;
  layer->nodes = Array<knn_Node>(size);
  layer->lineLength = Array<uint>(1,0);
  return layer;
}
knn_DeepLearning_Layer *knn_DeepLearning_addLinearLayer(knn_DeepLearning_Layer *in, const char *function)
{
  knn_DeepLearning_Layer *layer = (knn_DeepLearning_Layer *)malloc(sizeof(knn_DeepLearning_Layer));
  layer->type = knn_DeepLearning_Layer::Linear;
  layer->nodes = Array<knn_Node>(in->nodes.length,in->nodes.MAX_LENGTH);
  layer->fullyConnected.links = Array<knn_DeepLearning_Link>(layer->nodes.MAX_LENGTH);
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    knn_DeepLearning_Link *l = &layer->fullyConnected.links[i];
    l->input = i;
    l->output = i;
    l->weight = randf();
  }
  layer->bias = Array<float>(in->nodes.MAX_LENGTH);
  for (uint i = 0; i < in->nodes.MAX_LENGTH; i++)
  {
    layer->bias[i] = randf();
  }
  layer->fullyConnected.function = parseFunction(function);
  layer->lineLength = {in->lineLength.arr, in->lineLength.length, in->lineLength.MAX_LENGTH};
}
knn_DeepLearning_Layer *knn_DeepLearning_addConnectedLayer(knn_DeepLearning_Layer *in, size_t size, const char *function)
{
  knn_DeepLearning_Layer *layer = (knn_DeepLearning_Layer *)malloc(sizeof(knn_DeepLearning_Layer));
  layer->type = knn_DeepLearning_Layer::Linear;
  layer->nodes = Array<knn_Node>(size);
  layer->fullyConnected.links = Array<knn_DeepLearning_Link>(layer->nodes.MAX_LENGTH);
  for (uint i = 0; i < layer->nodes.length; i++)
  {
    knn_DeepLearning_Link *l = &layer->fullyConnected.links[i];
    l->input = i;
    l->output = i;
    l->weight = randf();
  }
  layer->bias = Array<float>(size);
  for (uint i = 0; i < size; i++)
  {
    layer->bias[i] = randf();
  }
  layer->fullyConnected.function = parseFunction(function);
  layer->lineLength = {in->lineLength.arr, in->lineLength.length, in->lineLength.MAX_LENGTH};
}
knn_DeepLearning_Layer *knn_DeepLearning_addConvolutionLayer(knn_DeepLearning_Layer *in); // not implemented
knn_DeepLearning_Layer *knn_DeepLearning_addMaxPoolingLayer(knn_DeepLearning_Layer *in);  // not implemented
knn_DeepLearning_Layer *knn_DeepLearning_addDropoutLayer(knn_DeepLearning_Layer *in, float dropout);
knn_DeepLearning_Layer *knn_DeepLearning_addSoftmaxLayer(knn_DeepLearning_Layer *in);

static void calculateLayer(knn_DeepLearning_Layer *layer)
{
  // TODO(ANT)
}

void knn_DeepLearning_calculate(knn_DeepLearning *net)
{
  calculateLayer(&net->output);
}

knn_DeepLearning knn_DeepLearning_createNet(uint number_layers, ...);
