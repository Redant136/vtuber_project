#pragma once

#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct webcam_data
  {
    // formated in rgb(a)
    uint8_t *data;
    uint32_t width, height, channels;
  } webcam_data;
  struct webcamInfo;
  typedef struct webcamInfo webcamInfo;

  webcamInfo *webcamInfoAlloc();
  int initWebcam(webcamInfo *webcam, unsigned int n);
  webcam_data initWebcamData(webcamInfo *webcam);
  int readWebcamFrame(webcam_data *data, webcamInfo *webcam);
  int webcamUninit(webcamInfo *webcam, webcam_data *data);

#ifdef __cplusplus
}
#endif
