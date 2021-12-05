#include "knn_NEAT.h"

static void freeIndividual(void *ind)
{
  knn_NEAT_free((knn_NEAT *)&((knn_NEAT_Individual *)ind)->net);
}
void knn_NEAT_freePopulation(knn_NEAT_Population *population)
{
  population->population.free(freeIndividual);
  population->linksHist.free();
  population->speciesReps.free();
}

static void checkInnov(const Array<knn_NEAT_Link> linksHistory)
{
  for (uint i = 0; i < linksHistory.length; i++)
  {
    assert(linksHistory.arr[i].innov == i);
  }
}
void knn_NEAT_updateHistory(knn_NEAT_Population *population)
{
  if (population->linksHist.length == 0)
  {
    size_t linksL = population->population[0].net.links.length;
    population->linksHist = Array<knn_NEAT_Link>(linksL, linksL * linksL);
    for (uint i = 0; i < linksL; i++)
    {
      population->linksHist[i] = population->population[0].net.links[i];
    }
  }
  for (uint net = 0; net < population->population.length; net++)
  {
    for (uint i = 0; i < population->population[net].net.links.length; i++)
    {
      knn_NEAT_Link link = population->population[net].net.links[i];
      knn_NEAT_Link h = population->linksHist[link.innov];
      if (h.input == link.input && h.output == link.output)
        continue;
      bool included = false;
      for (uint j = 0; j < population->linksHist.length; j++)
      {
        if (population->linksHist[j].input == population->population[net].net.links[i].input && population->linksHist[j].output == population->population[net].net.links[i].output)
        {
          assert(population->linksHist[j].innov == population->population[net].net.links[i].innov);
          included = true;
        }
      }
      if (!included)
        population->linksHist.addElement(link);
    }
  }
  checkInnov(population->linksHist);
}
void knn_NEAT_setSpecies(knn_NEAT_Population *population)
{
  if (population->speciesReps.length == 0)
  {
    if (population->speciesReps.MAX_LENGTH == 0)
    {
      population->speciesReps=Array<knn_NEAT_Individual>(0,population->population.length);
    }
    population->population[0].species = 0;
    population->speciesLength++;
    population->speciesReps.addElement(population->population[0]);
    for (uint i = 1; i < population->population.length; i++)
    {
      bool newSpecies = true;
      for (uint j = 0; j < population->speciesReps.length; j++)
      {
        if (knn_NEAT_calculateNetDelta(&population->population[i].net, &population->speciesReps[j].net, NEAT_maxSpeciesDelta) <= NEAT_maxSpeciesDelta)
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
        population->speciesReps.addElement(population->population[i]);
      }
    }
  }
  else
  {
    for (uint i = 0; i < population->population.length; i++)
    {
      float distance = NEAT_maxSpeciesDelta;
      bool newSpecies = true;
      for (uint j = 0; j < population->speciesReps.length; j++)
      {
        float ndist = knn_NEAT_calculateNetDelta(&population->population[i].net, &population->speciesReps[j].net, distance);
        if (ndist < distance)
        {
          distance = ndist;
          population->population[i].species = population->speciesReps[j].species;
          newSpecies = false;
        }
      }
      if (newSpecies)
      {
        population->population[i].species = population->speciesLength;
        population->speciesLength++;
        population->speciesReps.addElement(population->population[i]);
      }
    }
  }
}

static void updatePop(knn_NEAT_Population *population)
{
  Array<float> offspringsPerSpecies = Array<float>(population->speciesReps.length);

  if (1)
  {
    for (uint i = 0; i < offspringsPerSpecies.length; i++)
    {
      for (uint j = 0; j < population->population.length; j++)
      {
        if (population->population[j].species == population->speciesReps[i].species)
        {
          if (std::isnormal(population->population[j].fitness))
            offspringsPerSpecies[i] += population->population[j].fitness;
        }
      }
    }
    for (uint i = 0; i < offspringsPerSpecies.length; i++)
    {
      if (population->speciesReps[i].age >= 15)
        offspringsPerSpecies[i] = 0;
    }
    float offspringSum = 0;
    for (uint i = 0; i < offspringsPerSpecies.length; i++)
    {
      offspringSum += offspringsPerSpecies[i];
    }
    for (uint i = 0; i < offspringsPerSpecies.length; i++)
    {
      if (offspringSum == 0)
      {
        offspringsPerSpecies[i] = (float)population->population.length / offspringsPerSpecies.length;
      }
      else
      {
        offspringsPerSpecies[i] *= population->population.length / offspringSum;
      }
    }
    uint floorSum = 0;
    for (uint i = 0; i < offspringsPerSpecies.length; i++)
    {
      floorSum += (uint) offspringsPerSpecies[i];
    }

    if (floorSum < population->population.length)
    {
      // default 0 value, add 1 to everything
      Array<int> max = Array<int>(population->population.length - floorSum);
      for (uint i = 0; i < max.length; i++)
      {
        for (uint j = 0; j < offspringsPerSpecies.length; j++)
        {
          bool included = false;
          for (uint l = 0; l < max.length; l++)
          {
            included |= max[l] == j;
          }
          if (max[i] == 0 || (!included && (uint)offspringsPerSpecies[max[i] - 1] >= (uint)offspringsPerSpecies[j]))
          {
            if (max[i] == 0)
              max[i] = j + 1;
            else if (array_get(float, offspringsPerSpecies, array_get(int, max, i) - 1) < array_get(float, offspringsPerSpecies, j))
              array_get(int, max, i) = j + 1;
          }
        }
      }
      for (uint i = 0; i < max.length; i++)
      {
        array_get(float, offspringsPerSpecies, array_get(int, max, i) - 1)++;
      }
      array_ffree(float, &max);
    }
    for (uint i = 0; i < offspringsPerSpecies.length; i++)
    {
      array_get(uint, offspringsPerSpecies, i) = (uint)array_get(float, offspringsPerSpecies, i);
    }
  }

  struct Array nextGen = {malloc(population->population.length * sizeof(knn_NEAT_Individual)), 0, population->population.length};
  // champion
  for (uint i = 0; i < offspringsPerSpecies.length; i++)
  {
    if (array_get(uint, offspringsPerSpecies, i) > 5)
    {
      int maxFitIndex = -1;
      float speciesMaxFit = 0;
      for (uint j = 0; j < population->population.length; j++)
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
    for (uint j = 0; i < population->population.length; j++)
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
        for (uint i = 0; i < population->population.length; i++)
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
        for (uint i = 0; i < population->population.length; i++)
        {
          if (interSpecies || population->population[i].species == population->population[parent1Index].species)
          {
            totalFit += population->population[i].fitness;
          }
        }
        randInd = randf();
        currentFitInd = 0;
        for (uint i = 0; i < population->population.length; i++)
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
                                              array_get(uint, offspringsPerSpecies, i) / population->population.length,
                                              &population->linksHist);
      array_addElement(&nextGen, offspring);
    }
  }
  array_ffree(uint, &offspringsPerSpecies);
  free(population->population);
  population->population = nextGen.arr;
  population->population.length = nextGen.length;
}
void knn_NEAT_epoch(knn_NEAT_Population *population)
{
  population->epoch++;

  // lowerFitnessBySpecies
  for (uint i = 0; i < population->population.length; i++)
  {
    uint speciesSize = 0;
    for (uint j = 0; j < population->population.length; j++)
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
    for (uint j = 0; j < population->population.length; j++)
    {
      if (population->population[j].species == array_get(knn_NEAT_Individual, population->speciesReps, i).species)
      {
        speciesSize++;
      }
    }
    uint speciesIndex = 0;
    for (uint j = 0; j < population->population.length; j++)
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
