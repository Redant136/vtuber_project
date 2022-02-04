#define CHEVAN_DEBUG
#include "chevan_utils.h"
// #include <tensorflow/c/c_api.h>
using namespace ch;
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

  cchrono chrono;
  parseMnist();






}
