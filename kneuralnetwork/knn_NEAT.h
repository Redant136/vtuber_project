#pragma once
#ifndef knn_NEAT_Population_h
#define knn_NEAT_Population_h
#ifdef __cplusplus
extern "C"
{
#endif

#include "knn.h"

  typedef struct knn_NEAT_Individual
  {
    knn_NEAT net;
    float fitness;
    uint species, age;
  } knn_NEAT_Individual;
  typedef struct knn_NEAT_Population
  {
    knn_NEAT_Individual *population;
    size_t populationLength;
    union
    {
      struct
      {
        knn_NEAT_Link *linksHist;
        size_t linksHistLength;
      };
      knnArray linksHistory;
    };
    knnArray speciesReps; // array of knn_NEAT_Individual
    uint speciesLength;
    ulong epoch;
  } knn_NEAT_Population;

  static knn_NEAT_Population knn_NEAT_createPopulation(size_t populationSize, uint inputSize, uint outputSize)
  {
    knn_NEAT_Population population;
    population.populationLength = populationSize;
    population.population = malloc(sizeof(knn_NEAT_Individual) * populationSize);
    for (uint i = 0; i < populationSize; i++)
    {
      knn_NEAT_Individual ind = {knn_NEAT_createNet(inputSize, outputSize), 0, 0, 0};
      population.population[i] = ind;
    }
    population.speciesReps = (knnArray){malloc(sizeof(knn_NEAT_Individual) * populationSize), 0, populationSize};
    population.speciesLength = 0;
    size_t linksL = population.population[0].net.links.length;
    population.linksHistory = (knnArray){malloc(sizeof(knn_NEAT_Link) * linksL * linksL), linksL, linksL * linksL};
    for (uint i = 0; i < linksL; i++)
    {
      population.linksHist[i] = population.population[0].net.linksArr[i];
    }
    return population;
  }
  void knn_NEAT_freePopulation(knn_NEAT_Population *population);

  void knn_NEAT_updateHistory(knn_NEAT_Population *population);
  void knn_NEAT_setSpecies(knn_NEAT_Population *population);
  void knn_NEAT_epoch(knn_NEAT_Population *population);
  void knn_NEAT_selectNewSpeciesReps(knn_NEAT_Population *population);

#ifdef __cplusplus
}
#endif
#endif
