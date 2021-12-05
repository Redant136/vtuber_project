#include "knn_NEAT.h"

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
