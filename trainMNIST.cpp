#define CHEVAN_DEBUG
#include "kneuralnetwork/knn.h"
#include "kneuralnetwork/knn_DeepLearning.h"
#include "chevan_utils.h"
using namespace chevan_utils;
using namespace chevan_utils::chevanut_print;
using namespace chevanut_byte_typedef;

#include "chevan_chrono.h"

#define mnistPath "/home/chevan/mnistDataset"
struct DataPoint
{
  uint label;
  uchar *pixels;
};

static struct
{
  uint width, height;
  Array<DataPoint> training;
  Array<DataPoint> test;
} dataset;

int reverseInt(int i)
{
  unsigned char c1, c2, c3, c4;
  c1 = i & 255;
  c2 = (i >> 8) & 255;
  c3 = (i >> 16) & 255;
  c4 = (i >> 24) & 255;
  return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
}
static void parseMnist()
{
  std::string trainingImagesPath = mnistPath "/train-images-idx3-ubyte/train-images.idx3-ubyte";
  std::string trainingLabelPath = mnistPath "/train-labels-idx1-ubyte/train-labels.idx1-ubyte";
  std::string testImagesPath = mnistPath "/t10k-images-idx3-ubyte/t10k-images.idx3-ubyte";
  std::string testLabelPath = mnistPath "/t10k-labels-idx1-ubyte/t10k-labels.idx1-ubyte";

  size_t imgDataL;
  uchar *imgData;
  imgData = (uchar *)ch_bufferFile(testImagesPath.c_str(), (void **)&imgData, &imgDataL);
  size_t labelDataL;
  uchar *labelData;
  labelData = (uchar *)ch_bufferFile(testLabelPath.c_str(), (void **)&labelData, &labelDataL);

  struct
  {
    struct
    {
      u32 magicNum;
      u32 imagesLength; // only 1 pixel per image
      u32 rows;
      u32 columns;
      uchar *pixels;
    } images;
    struct
    {
      u32 magicNum;
      u32 labelsLength;
      uchar *labels;
    } labels;
  } mnistData;
  memcpy(&mnistData.images, imgData, sizeof(mnistData.images));
  mnistData.images.magicNum = reverseInt(mnistData.images.magicNum);
  mnistData.images.imagesLength = reverseInt(mnistData.images.imagesLength);
  mnistData.images.rows = reverseInt(mnistData.images.rows);
  mnistData.images.columns = reverseInt(mnistData.images.columns);
  assert(mnistData.images.magicNum == 2051);
  mnistData.images.pixels = new uchar[mnistData.images.imagesLength * mnistData.images.rows * mnistData.images.columns];
  memcpy(mnistData.images.pixels, imgData + sizeof(u32) * 4, mnistData.images.imagesLength * mnistData.images.rows * mnistData.images.columns);

  memcpy(&mnistData.labels, labelData, sizeof(mnistData.labels));
  mnistData.labels.magicNum = reverseInt(mnistData.labels.magicNum);
  mnistData.labels.labelsLength = reverseInt(mnistData.labels.labelsLength);
  assert(mnistData.labels.magicNum == 2049);
  mnistData.labels.labels = new uchar[mnistData.labels.labelsLength];
  memcpy(mnistData.labels.labels, labelData + sizeof(u32) * 2, mnistData.labels.labelsLength);

  // ----------------------------------

  dataset.width = mnistData.images.rows;
  dataset.height = mnistData.images.columns;
  dataset.test = Array<DataPoint>(std::min(mnistData.labels.labelsLength, mnistData.images.imagesLength));
  for (uint i = 0; i < dataset.training.length; i++)
  {
    dataset.test[i].label = mnistData.labels.labels[i];
    dataset.test[i].pixels = mnistData.images.pixels + i * (dataset.width * dataset.height);
  }
  free(imgData);
  free(labelData);

  // --------------------------------

  imgData = (uchar *)ch_bufferFile(trainingImagesPath.c_str(), (void **)&imgData, &imgDataL);
  labelData = (uchar *)ch_bufferFile(trainingLabelPath.c_str(), (void **)&labelData, &labelDataL);

  memcpy(&mnistData.images, imgData, sizeof(mnistData.images));
  mnistData.images.magicNum = reverseInt(mnistData.images.magicNum);
  mnistData.images.imagesLength = reverseInt(mnistData.images.imagesLength);
  mnistData.images.rows = reverseInt(mnistData.images.rows);
  mnistData.images.columns = reverseInt(mnistData.images.columns);
  assert(mnistData.images.magicNum == 2051);
  mnistData.images.pixels = new uchar[mnistData.images.imagesLength * mnistData.images.rows * mnistData.images.columns];
  memcpy(mnistData.images.pixels, imgData + sizeof(u32) * 4, mnistData.images.imagesLength * mnistData.images.rows * mnistData.images.columns);

  memcpy(&mnistData.labels, labelData, sizeof(mnistData.labels));
  mnistData.labels.magicNum = reverseInt(mnistData.labels.magicNum);
  mnistData.labels.labelsLength = reverseInt(mnistData.labels.labelsLength);
  assert(mnistData.labels.magicNum == 2049);
  mnistData.labels.labels = new uchar[mnistData.labels.labelsLength];
  memcpy(mnistData.labels.labels, labelData + sizeof(u32) * 2, mnistData.labels.labelsLength);

  // ---------------------------------

  assert(dataset.width == mnistData.images.rows);
  assert(dataset.height == mnistData.images.columns);
  dataset.training = Array<DataPoint>(std::min(mnistData.labels.labelsLength, mnistData.images.imagesLength));
  for (uint i = 0; i < dataset.training.length; i++)
  {
    dataset.training[i].label = mnistData.labels.labels[i];
    dataset.training[i].pixels = mnistData.images.pixels + i * (dataset.width * dataset.height);
  }
  free(imgData);
  free(labelData);
}
int main()
{
  seed_rand();
  if (0)
  {
    static const int numTrainingSets = 4;
    double training_inputs[numTrainingSets][2] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}};
    float training_outputs[numTrainingSets][1] = {{0.0f}, {1.0f}, {1.0f}, {0.0f}};
    knn_DeepLearning_Layer *in = knn_DeepLearning_inputLayer_size(2);
    knn_DeepLearning_Layer *current = in;
    current = knn_DeepLearning_addConnectedLayer(current, 2, "sig");
    current = knn_DeepLearning_addConnectedLayer(current, 1, "sig");
    knn_DeepLearning net = knn_DeepLearning_createNet_InOut(in, current);
    for (uint _t = 0; _t < 1000000; _t++)
    {
      uint t = randf() * numTrainingSets;
      knn_DeepLearning_Backpropagator back = {training_outputs[t], knn_DeepLearning_meanSquaredError};
      knn_DeepLearning_initBackpropagateInternal(&net, &back);
      for (uint i = 0; i < net.input->nodes.length; i++)
      {
        array_get(knn_Node, net.input->nodes, i).value = training_inputs[t][i];
      }
      knn_DeepLearning_calculate(&net);
      println(knn_DeepLearning_getCost(&net, back));
      knn_DeepLearning_backpropagate(&net, back);
    }
    return 0;
  }

  cchrono chrono;
  parseMnist();

  knnArray format = {new uint[2], 2, 2};
  array_get(uint, format, 0) = dataset.width;
  array_get(uint, format, 1) = dataset.height;
  knn_DeepLearning_Layer *in = knn_DeepLearning_inputLayer(format);
  array_get(uint, format, 0) = 5;
  array_get(uint, format, 1) = 5;
  knn_DeepLearning_Layer *current = in;
  // current = knn_DeepLearning_addConvolutionLayer(in, format, "relu");
  array_get(uint, format, 0) = dataset.width;
  array_get(uint, format, 1) = dataset.height;
  // current = knn_DeepLearning_addConnectedLayer_layout(current, format, "tanh");
  // current = knn_DeepLearning_addConnectedLayer_layout(current, format, "sig");

  current = knn_DeepLearning_addDropoutLayer(current, 0.1);
  current = knn_DeepLearning_addConnectedLayer(current, 49, "sig");

  knn_DeepLearning_Layer *out = knn_DeepLearning_addConnectedLayer(current, 10, "sig");
  // knn_DeepLearning_Layer *out = knn_DeepLearning_addSoftmaxLayer(current, 10);
  knn_DeepLearning net = knn_DeepLearning_createNet_InOut(in, out);

  float **inV = knn_DeepLearning_getLayerValuesPointers(net.input);
  float **outV = knn_DeepLearning_getLayerValuesPointers(net.output);
  float result[10];
  knn_DeepLearning_Backpropagator backprop = {result, knn_DeepLearning_meanSquaredError,1};
  knn_DeepLearning_initBackpropagateInternal(&net, &backprop);

  double lastRes[100] = {1};
  // cchrono_start(chrono);
  for (uint d = 0; d < dataset.training.length; d++)
  {
    uint i = d % dataset.training.length;
    for (uint j = 0; j < net.input->nodes.length; j++)
    {
      *inV[j] = dataset.training[i].pixels[j] / 255.0;
    }

    knn_DeepLearning_calculate(&net);

    for (uint j = 0; j < 10; j++)
    {
      result[j] = 0;
    }
    result[dataset.training[i].label] = 1;

    double cost = knn_DeepLearning_getCost(&net, backprop);
    lastRes[d % sizeofArr(lastRes)] = cost;

    // println(dataset.training[i].label);
    // print("{");
    // for(uint j=0;j<9;j++){
    //   print(*outV[j], ",");
    // }
    // println(*outV[9], "}");
    // print("{");
    // for (uint j = 0; j < 9; j++)
    // {
    //   print(result[j], ",");
    // }
    // println(result[9], "}");

    double sum = 0;
    for (uint j = 0; j < sizeofArr(lastRes); j++)
    {
      sum += lastRes[j];
    }
    if (d % 100 == 0)
    {
      println(d, ": ", cost, "\t\t\t", sum / sizeofArr(lastRes));
    }
    // println(sum / sizeofArr(lastRes));
    // println(cost);

    // knn_DeepLearning_backpropagate(&net, backprop);
    knn_DeepLearning_backpropagateStore(&net, backprop, 10);

    knn_DeepLearning_clear(&net);
  }
  // ch_println(cchrono_elapsed(chrono));

  if (1)
  {
    uint index = randf() * dataset.training.length;
    // index %= 2;
    printSep();
    for (uint j = 0; j < net.input->nodes.length; j++)
    {
      *inV[j] = dataset.training[index].pixels[j] / 255.0;
    }
    knn_DeepLearning_calculate(&net);
    int netOut = -1;
    float max = -1;
    float sum = 0;
    for (uint i = 0; i < net.output->nodes.length; i++)
    {
      sum += *outV[i];
      if (max < *outV[i])
      {
        max = *outV[i];
        netOut = i;
      }
    }
    for (uint i = 0; i < net.output->nodes.length; i++)
    {
      // println(i, ": ", *outV[i] / sum * 100, "%");
      println(i, ": ", *outV[i]);
    }

    println(netOut);
    println(dataset.training[index].label);

    for (int i = 0; i < 28; i++)
    {
      for (int j = 0; j < 28; j++)
      {
        print(dataset.training[index].pixels[j + i * 28] > 127 ? "1" : " ", " ");
      }
      println();
    }
  }
  while (0)
  {
    uint index = 0;
    std::cin >> index;
    // index %= 2;
    printSep();
    for (uint j = 0; j < net.input->nodes.length; j++)
    {
      *inV[j] = dataset.training[index].pixels[j] / 255.0;
    }
    knn_DeepLearning_calculate(&net);
    int netOut = -1;
    float max = -1;
    float sum = 0;
    for (uint i = 0; i < net.output->nodes.length; i++)
    {
      sum += *outV[i];
      if (max < *outV[i])
      {
        max = *outV[i];
        netOut = i;
      }
    }
    for (uint i = 0; i < net.output->nodes.length; i++)
    {
      // println(i, ": ", *outV[i] / sum * 100, "%");
      println(i, ": ", *outV[i]);
    }

    println(netOut);
    println(dataset.training[index].label);

    for (int i = 0; i < 28; i++)
    {
      for (int j = 0; j < 28; j++)
      {
        print(dataset.training[index].pixels[j + i * 28] > 70 ? "1" : " ", " ");
      }
      println();
    }
  }
}
