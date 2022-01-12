#include "knn_NEAT.h"

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

static void freeIndividual(void *ind)
{
  knn_NEAT_free((knn_NEAT *)&((knn_NEAT_Individual *)ind)->net);
}
void knn_NEAT_freePopulation(knn_NEAT_Population *population)
{
  struct Array pop = {population->population, population->populationLength, population->populationLength};
  array_free(knn_NEAT_Individual, &pop, freeIndividual);
  array_ffree(&population->linksHistory);
  array_ffree(&population->speciesReps);
}

static void checkInnov(const struct Array linksHistory)
{
  knn_NEAT_Link *linksHist = linksHistory.arr;
  for (uint i = 0; i < linksHistory.length; i++)
  {
    assert(linksHist[i].innov == i);
  }
}
void knn_NEAT_updateHistory(knn_NEAT_Population *population)
{
  if (population->linksHistory.length == 0)
  {
    size_t linksL = population->population[0].net.links.length;
    population->linksHistory = (struct Array){malloc(sizeof(knn_NEAT_Link) * linksL * linksL), linksL, linksL * linksL};
    for (uint i = 0; i < linksL; i++)
    {
      population->linksHist[i] = population->population[0].net.linksArr[i];
    }
  }
  for (uint net = 0; net < population->populationLength; net++)
  {
    for (uint i = 0; i < population->population[net].net.links.length; i++)
    {
      knn_NEAT_Link link = population->population[net].net.linksArr[i];
      knn_NEAT_Link h = population->linksHist[link.innov];
      if (h.input == link.input && h.output == link.output)
        continue;
      bool included = false;
      for (uint j = 0; j < population->linksHistLength; j++)
      {
        if (population->linksHist[j].input == population->population[net].net.linksArr[i].input && population->linksHist[j].output == population->population[net].net.linksArr[i].output)
        {
          assert(population->linksHist[j].innov == population->population[net].net.linksArr[i].innov);
          included = true;
        }
      }
      if (!included)
        array_addElement(&population->linksHistory, link);
    }
  }
  checkInnov(population->linksHistory);
}
void knn_NEAT_setSpecies(knn_NEAT_Population *population)
{
  if (population->speciesReps.length == 0)
  {
    if (population->speciesReps.MAX_LENGTH == 0)
    {
      population->speciesReps = (struct Array){malloc(sizeof(knn_NEAT_Individual) * population->populationLength), 0, population->populationLength};
    }
    population->population[0].species = 0;
    population->speciesLength++;
    array_addElement(&population->speciesReps, population->population[0]);
    for (uint i = 1; i < population->populationLength; i++)
    {
      bool newSpecies = true;
      for (uint j = 0; j < population->speciesReps.length; j++)
      {
        if (knn_NEAT_calculateNetDelta(&population->population[i].net, population->speciesReps.arr + j * sizeof(knn_NEAT_Individual), NEAT_maxSpeciesDelta) <= NEAT_maxSpeciesDelta)
        {
          population->population[i].species = ((knn_NEAT_Individual *)population->speciesReps.arr)[j].species;
          newSpecies = false;
          break;
        }
      }
      if (newSpecies)
      {
        population->population[i].species = population->speciesLength;
        population->speciesLength++;
        array_addElement(&population->speciesReps, population[i]);
      }
    }
  }
  else
  {
    for (uint i = 0; i < population->populationLength; i++)
    {
      float distance = NEAT_maxSpeciesDelta;
      bool newSpecies = true;
      for (uint j = 0; j < population->speciesReps.length; j++)
      {
        float ndist = knn_NEAT_calculateNetDelta(&population->population[i].net, &array_get(knn_NEAT_Individual, population->speciesReps, j).net, distance);
        if (ndist < distance)
        {
          distance = ndist;
          population->population[i].species = array_get(knn_NEAT_Individual, population->speciesReps, j).species;
          newSpecies = false;
        }
      }
      if (newSpecies)
      {
        population->population[i].species = population->speciesLength;
        population->speciesLength++;
        array_addElement(&population->speciesReps, population->population[i]);
      }
    }
  }
}

static void updatePop(knn_NEAT_Population *population)
{
  struct Array offspringsPerSpecies = {calloc(population->speciesReps.length, sizeof(float)), population->speciesReps.length};

  if (1)
  {
    for (uint i = 0; i < offspringsPerSpecies.length; i++)
    {
      for (uint j = 0; j < population->populationLength; j++)
      {
        if (population->population[j].species == array_get(knn_NEAT_Individual, population->speciesReps, i).species)
        {
          if (isnormal(population->population[j].fitness))
            array_get(float, offspringsPerSpecies, i) += population->population[j].fitness;
        }
      }
    }
    for (uint i = 0; i < offspringsPerSpecies.length; i++)
    {
      if (array_get(knn_NEAT_Individual, population->speciesReps, i).age >= 15)
        array_get(float, offspringsPerSpecies, i) = 0;
    }
    float offspringSum = 0;
    for (uint i = 0; i < offspringsPerSpecies.length; i++)
    {
      offspringSum += array_get(float, offspringsPerSpecies, i);
    }
    for (uint i = 0; i < offspringsPerSpecies.length; i++)
    {
      if (offspringSum == 0)
      {
        array_get(float, offspringsPerSpecies, i) = (float)population->populationLength / offspringsPerSpecies.length;
      }
      else
      {
        array_get(float, offspringsPerSpecies, i) *= population->populationLength / offspringSum;
      }
    }
    uint floorSum = 0;
    for (uint i = 0; i < offspringsPerSpecies.length; i++)
    {
      floorSum += (uint)array_get(float, offspringsPerSpecies, i);
    }

    if (floorSum < population->populationLength)
    {
      // default 0 value, add 1 to everything
      struct Array max = {calloc(population->populationLength - floorSum, sizeof(int)), population->populationLength - floorSum};
      for (uint i = 0; i < max.length; i++)
      {
        for (uint j = 0; j < offspringsPerSpecies.length; j++)
        {
          bool included = false;
          for (uint l = 0; l < max.length; l++)
          {
            included |= array_get(int, max, l) == j;
          }
          if (array_get(int, max, i) == 0 || (!included && (uint)array_get(float, offspringsPerSpecies, array_get(int, max, i) - 1) >= (uint)array_get(float, offspringsPerSpecies, j)))
          {
            if (array_get(int, max, i) == 0)
              array_get(int, max, i) = j + 1;
            else if (array_get(float, offspringsPerSpecies, array_get(int, max, i) - 1) < array_get(float, offspringsPerSpecies, j))
              array_get(int, max, i) = j + 1;
          }
        }
      }
      for (uint i = 0; i < max.length; i++)
      {
        array_get(float, offspringsPerSpecies, array_get(int, max, i) - 1)++;
      }
      array_ffree(&max);
    }
    for (uint i = 0; i < offspringsPerSpecies.length; i++)
    {
      array_get(uint, offspringsPerSpecies, i) = (uint)array_get(float, offspringsPerSpecies, i);
    }
  }

  struct Array nextGen = {malloc(population->populationLength * sizeof(knn_NEAT_Individual)), 0, population->populationLength};
  // champion
  for (uint i = 0; i < offspringsPerSpecies.length; i++)
  {
    if (array_get(uint, offspringsPerSpecies, i) > 5)
    {
      int maxFitIndex = -1;
      float speciesMaxFit = 0;
      for (uint j = 0; j < population->populationLength; j++)
      {
        if (population->population[j].species == array_get(knn_NEAT_Individual, population->speciesReps, i).species)
        {
          if (population->population[j].fitness > speciesMaxFit)
          {
            speciesMaxFit = population->population[j].fitness;
            maxFitIndex = j;
          }
        }
      }
      if (maxFitIndex != -1)
      {
        array_get(uint, offspringsPerSpecies, i)--;
        array_get(knn_NEAT_Individual, nextGen, nextGen.length) =
            (knn_NEAT_Individual){.net = population->population[maxFitIndex].net,
                                  .fitness = 0,
                                  .species = population->population[maxFitIndex].species,
                                  .age = population->population[maxFitIndex].age + 1};
        nextGen.length++;
      }
    }
  }

  for (uint i = 0; i < offspringsPerSpecies.length; i++)
  {
    float totalFit = 0;
    for (uint j = 0; i < population->populationLength; j++)
    {
      if (population->population[j].species == array_get(knn_NEAT_Individual, population->speciesReps, i).species)
      {
        totalFit += population->population[j].fitness;
      }
    }
    for (uint l = 0; l < array_get(uint, offspringsPerSpecies, i); l++)
    {
      uint parent1Index = 0;
      uint parent2Index = 0;
      if (1)
      {
        float randInd = randf();
        float currentFitInd = 0;
        for (uint i = 0; i < population->populationLength; i++)
        {
          if (population->population[i].species == array_get(knn_NEAT_Individual, population->speciesReps, i).species)
          {
            if (randInd < population->population[i].fitness / totalFit + currentFitInd)
            {
              parent1Index = i;
              break;
            }
            else
            {
              currentFitInd += population->population[i].fitness / totalFit;
            }
          }
        }
        bool interSpecies = randf() < NEAT_oddInterspeciesMutation;
        float totalFit = 0;
        for (uint i = 0; i < population->populationLength; i++)
        {
          if (interSpecies || population->population[i].species == population->population[parent1Index].species)
          {
            totalFit += population->population[i].fitness;
          }
        }
        randInd = randf();
        currentFitInd = 0;
        for (uint i = 0; i < population->populationLength; i++)
        {
          if (interSpecies || population->population[i].species == population->population[parent1Index].species)
          {
            if (randInd < population->population[i].fitness / totalFit + currentFitInd)
            {
              parent2Index = i;
            }
            else
            {
              currentFitInd += population->population[i].fitness / totalFit;
            }
          }
        }
      }
      knn_NEAT offspring = knn_NEAT_breedNets(&population->population[parent1Index].net,
                                              &population->population[parent2Index].net,
                                              array_get(uint, offspringsPerSpecies, i) / population->populationLength,
                                              &population->linksHistory);
      array_addElement(&nextGen, offspring);
    }
  }
  array_ffree(&offspringsPerSpecies);
  free(population->population);
  population->population = nextGen.arr;
  population->populationLength = nextGen.length;
}
void knn_NEAT_epoch(knn_NEAT_Population *population)
{
  population->epoch++;

  // lowerFitnessBySpecies
  for (uint i = 0; i < population->populationLength; i++)
  {
    uint speciesSize = 0;
    for (uint j = 0; j < population->populationLength; j++)
    {
      assert(population->population[j].fitness >= 0);
      if (knn_NEAT_calculateNetDelta(&population->population[i].net, &population->population[j].net, NEAT_maxSpeciesDelta) <= NEAT_maxSpeciesDelta)
        speciesSize++;
    }
    population->population[i].fitness /= speciesSize;
  }

  updatePop(population);
  knn_NEAT_setSpecies(population);
  knn_NEAT_selectNewSpeciesReps(population);
}
void knn_NEAT_selectNewSpeciesReps(knn_NEAT_Population *population)
{
  for (uint i = 0; i < population->speciesReps.length; i++)
  {
    float randInd = randf();
    uint speciesSize = 0;
    for (uint j = 0; j < population->populationLength; j++)
    {
      if (population->population[j].species == array_get(knn_NEAT_Individual, population->speciesReps, i).species)
      {
        speciesSize++;
      }
    }
    uint speciesIndex = 0;
    for (uint j = 0; j < population->populationLength; j++)
    {
      if (population->population[j].species == array_get(knn_NEAT_Individual, population->speciesReps, i).species)
      {
        speciesIndex++;
        if (randInd < speciesIndex / speciesSize)
        {
          array_get(knn_NEAT_Individual, population->speciesReps, i) = population->population[j];
        }
      }
    }
  }
}
