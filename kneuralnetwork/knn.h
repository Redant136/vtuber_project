#pragma once

#ifndef knn_h
#define knn_h
#include "../chevan_utils.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define nImpl_err AC_NOT_IMPLEMENTED_ERROR

#ifdef __cplusplus
  struct knnArray
  {
    void *arr;
    size_t length;
    size_t MAX_LENGTH;
  };
#define array_get(type, a, i) (*(type *)((unsigned char *)a.arr + (i) * sizeof(type)))
  typedef chevan_utils::chevanut_vec::ivec2 knnVec2;
  typedef chevan_utils::chevanut_vec::ivec3 knnVec3;
  typedef chevan_utils::chevanut_vec::ivec4 knnVec4;

#else
typedef struct Array knnArray;
typedef struct ivec2 knnVec2;
typedef struct ivec3 knnVec3;
typedef struct ivec4 knnVec4;
#endif

#define _NEAT
#ifdef _NEAT // NEAT defines
#ifndef NEAT_Function
#define NEAT_Function(x) (1 / (1 + exp(-4.9 * x)))
#endif
#ifndef NEAT_maxSpeciesDelta
#define NEAT_maxSpeciesDelta 3.0
#endif
#ifndef NEAT_oddInterspeciesMutation
#define NEAT_oddInterspeciesMutation 0.01
#endif

#ifndef NEAT_c1
#define NEAT_c1 1
#endif
#ifndef NEAT_c2
#define NEAT_c2 1
#endif
#ifndef NEAT_c3
#define NEAT_c3 0.4
#endif

#ifndef NEAT_mutationRange
#define NEAT_mutationRange 10
#endif
#ifndef NEAT_oddMutationWithCrossover
#define NEAT_oddMutationWithCrossover 0.75
#endif
#ifndef NEAT_oddDisabledGeneTransmited
#define NEAT_oddDisabledGeneTransmited 0.75
#endif
#ifndef NEAT_oddAddDisjointExcessGene
#define NEAT_oddAddDisjointExcessGene 0.75
#endif
#ifndef NEAT_oddAddNode
#define NEAT_oddAddNode 0.03
#endif
#ifndef NEAT_oddNewLinkSmallSpecies
#define NEAT_oddNewLinkSmallSpecies 0.05
#endif
#ifndef NEAT_oddNewLinkBigSpecies
#define NEAT_oddNewLinkBigSpecies 0.3
#endif
#ifndef NEAT_oddBias
#define NEAT_oddBias 0.75
#endif
#ifndef NEAT_oddRecursiveLink
#define NEAT_oddRecursiveLink 0.001
#endif
#ifndef NEAT_rationBigSpecies
#define NEAT_rationBigSpecies 0.1
#endif
#ifndef NEAT_oddWeightMutate
#define NEAT_oddWeightMutate 0.8
#endif
#ifndef NEAT_oddWeightRandomMutate
#define NEAT_oddWeightRandomMutate 0.1
#endif
#endif

  typedef struct knn_Node
  {
    float value;
    bool calculated;
  } knn_Node;

  typedef struct knn_NEAT_Link
  {
    uint input, output;
    float weight;
    uint innov;
    bool enabled, recursive;
  } knn_NEAT_Link;

  typedef float (*knn_ActivationFunc)(float);

  typedef struct knn_NEAT
  {
    // nodes are arranged input,output,1(bias),hidden
    knnArray nodes;
    size_t input, output;
    union
    {
      knn_NEAT_Link *linksArr;
      knnArray links;
    };

  } knn_NEAT;

  knn_NEAT knn_NEAT_createNet(uint inputSize, uint outputSize);
  knn_NEAT knn_NEAT_clone(knn_NEAT *net);
  void knn_NEAT_free(knn_NEAT *net);

  knn_Node *knn_NEAT_getNetInput(knn_NEAT *net);
  knn_Node *knn_NEAT_getNetOutput(knn_NEAT *net);
  float **knn_NEAT_getInput(knn_NEAT *net);
  float **knn_NEAT_getOutput(knn_NEAT *net);
  void knn_NEAT_setInput(knn_NEAT *net, float *val);
  // HID=0,IN=1,OUT=2,BIAS=3,ACT=4
  uint knn_NEAT_getNodeType(knn_NEAT *net, uint node);

  float knn_NEAT_calculateNetDelta(knn_NEAT *n1, knn_NEAT *n2, float maxDelta);
  void knn_NEAT_calculateNet(knn_NEAT *net);
  knn_NEAT knn_NEAT_breedNets(knn_NEAT *parent1, knn_NEAT *parent2, float speciesRatio, knnArray *linksHistory);

  typedef struct knn_DeepLearning_Link
  {
    uint input, output;
    float weight;
  } knn_DeepLearning_Link;
  typedef struct knn_DeepLearning_Layer
  {
    struct knn_DeepLearning_Layer *prev, *next;
    enum
    {
      FullyConnected, // 1->1 2->1 3->1
      Linear,         // 1->1 2->2
      Convolutionnal, // 1,2,3,4->1, 2,3,4,5->2
      MaxPooling,     // 1,2,3,4->1 5,6,7,8->2
      Dropout,        // dropout
      Softmax,        // gives percentage
      Input           // in layer
    } type;
    knnArray nodes;
    knnArray bias;
    union
    {
      struct
      {
        knnArray links;
        knn_ActivationFunc function;
      } fullyConnected;
      struct
      {
        float dropoutValue;
      } dropout;
      struct
      {
        knnArray links,convFormat;
        knn_ActivationFunc function;
      } convolutionnal;
      struct
      {
        knnArray in;
        bool paddingType; // 0-discard, 1-add extra
      } pooling;
    };
    knnArray layout; // if length is 1 or 2, ignore. allows to map n dimension to 1D
  } knn_DeepLearning_Layer;
  typedef struct knn_DeepLearning
  {
    knn_DeepLearning_Layer input;
    knn_DeepLearning_Layer output;
  } knn_DeepLearning;

  knn_DeepLearning_Layer *knn_DeepLearning_inputLayer(knnArray layout);
  knn_DeepLearning_Layer *knn_DeepLearning_inputLayer_size(size_t size);
  knn_DeepLearning_Layer *knn_DeepLearning_addLinearLayer(knn_DeepLearning_Layer *in, const char *function);
  knn_DeepLearning_Layer *knn_DeepLearning_addConnectedLayer(knn_DeepLearning_Layer *in, size_t size, const char *function);
  knn_DeepLearning_Layer *knn_DeepLearning_addConvolutionLayer(knn_DeepLearning_Layer *in, knnArray convFormat); // not implemented
  knn_DeepLearning_Layer *knn_DeepLearning_addMaxPoolingLayer(knn_DeepLearning_Layer *in);                       // not implemented
  knn_DeepLearning_Layer *knn_DeepLearning_addDropoutLayer(knn_DeepLearning_Layer *in, float dropout);
  knn_DeepLearning_Layer *knn_DeepLearning_addSoftmaxLayer(knn_DeepLearning_Layer *in);

  void knn_DeepLearning_calculate(knn_DeepLearning *net);

  static uint knn_DeepLearning_getCorrespondingIndexV2(knnVec2 pos, knnArray layout)
  {
    assert(layout.length == 2);
    return pos.x + pos.y * array_get(uint, layout, 0);
  }
  static uint knn_DeepLearning_getCorrespondingIndexV3(knnVec3 pos, knnArray layout)
  {
    assert(layout.length == 3);
    return pos.x + pos.y * array_get(uint, layout, 0) + pos.z * array_get(uint, layout, 1);
  }
  static uint knn_DeepLearning_getCorrespondingIndexV4(knnVec4 pos, knnArray layout)
  {
    assert(layout.length == 4);
    return pos.x + pos.y * array_get(uint, layout, 0) + pos.z * array_get(uint, layout, 1) + pos.w * array_get(uint, layout, 2);
  }
  static uint knn_DeepLearning_getCorrespondingIndexArr(knnArray pos, knnArray layout)
  {
    if (layout.length == 0 || layout.length == 1)
    {
      return array_get(uint, pos, 0);
    }
    assert(pos.length == layout.length);
    uint index = 0;
    ulong skip = 1;
    for (uint i = 0; i < layout.length; i++)
    {
      index += array_get(uint, pos, i) * skip;
      skip *= array_get(uint, layout, i);
    }
    return index;
  }
  static uint knn_DeepLearning_getCorrespondingIndex(uint l, knnArray layout, ...)
  {
    assert(l == layout.length);
    uint index = 0;
    ulong skip = 1;
    va_list args;
    va_start(args, layout);
    for (uint i = 0; i < l; i++)
    {
      int line = va_arg(args, int);
      index += line * skip;
      skip *= line;
    }

    return 0;
  }

#define _knn_DeepLearning_getCorrespondingIndex_1(layout, p) (p)
#define _knn_DeepLearning_getCorrespondingIndex_2(layout, x, y) knn_DeepLearning_getCorrespondingIndexV2((knnVec2){x, y}, layout)
#define _knn_DeepLearning_getCorrespondingIndex_3(layout, x, y, z) knn_DeepLearning_getCorrespondingIndexV3((knnVec3){x, y, z}, layout)
#define _knn_DeepLearning_getCorrespondingIndex_4(layout, x, y, z, w) knn_DeepLearning_getCorrespondingIndexV3((knnVec3){x, y, z, w}, layout)

#define knn_DeepLearning_getCorrespondingIndex(l, layout, ...) \
  ((l) < 1 ? 0 : (((l) <= 4 ? _knn_DeepLearning_getCorrespondingIndex_##l(layout, __VA_ARGS__) : (knn_DeepLearning_getCorrespondingIndex(l, layout, __VA_ARGS__)))))

  static knnArray knn_DeepLearning_createLayoutArr(uint l, ...)
  {
    knnArray arr = {mallocArr(uint, l), l, l};
    va_list args;
    va_start(args, l);
    for (uint i = 0; i < l; i++)
    {
      array_get(uint, arr, i) = va_arg(args, int);
    }
    return arr;
  }

  knn_DeepLearning knn_DeepLearning_createNet(uint number_layers, ...);
  static knn_DeepLearning knn_DeepLearning_createNet_InOut(knn_DeepLearning_Layer in, knn_DeepLearning_Layer out)
  {
    knn_DeepLearning net = {in, out};
    return net;
  }
  knn_DeepLearning knn_DeepLearning_clone(knn_DeepLearning *net);
  float **knn_DeepLearning_getLayerValuesPointers(knn_DeepLearning_Layer *layer);

#ifdef __cplusplus
}
#endif
#endif
