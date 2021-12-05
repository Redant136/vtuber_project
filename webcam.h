#pragma once

#include "chevan_utils.h"
using namespace chevan_utils;
using namespace chevanut_vec;

struct webcam_data
{
  // formated in rgb(a)
  uint8_t *data;
  uint32_t width, height, channels;
};
struct webcamInfo;
static int initWebcam(webcamInfo *webcam);
static webcam_data initWebcamData(webcamInfo *webcam);
static int readWebcamFrame(webcam_data *data, webcamInfo *webcam);

#ifdef WebCamOpenCV
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

struct webcamInfo
{
  cv::VideoCapture cameraCapture;
  cv::Mat cameraFrame;
};

static webcamInfo *webcamInfoAlloc()
{
  webcamInfo *webcam = new webcamInfo();
  return webcam;
}

static int initWebcam(webcamInfo *webcam)
{
  int deviceID = 0;
  webcam->cameraCapture.open(deviceID);
  if (!webcam->cameraCapture.isOpened())
  {
    std::cerr << "ERROR! Unable to open camera\n";
    return 1;
  }
  webcam->cameraCapture >> webcam->cameraFrame;
  if (webcam->cameraFrame.empty())
  {
    std::cerr << "ERROR! blank frame grabbed\n";
    return 1;
  }
  return 0;
}

static webcam_data initWebcamData(webcamInfo *webcam)
{
  return {(uint8_t *)malloc(webcam->cameraFrame.cols * webcam->cameraFrame.rows * 3), (uint32_t)webcam->cameraFrame.cols, (uint32_t)webcam->cameraFrame.rows, 3};
}

static int readWebcamFrame(webcam_data *data, webcamInfo *webcam)
{
  webcam->cameraCapture >> webcam->cameraFrame;
  if (webcam->cameraFrame.empty())
  {
    std::cerr << "ERROR! blank frame grabbed\n";
    return 1;
  }
  cv::cvtColor(webcam->cameraFrame, webcam->cameraFrame, cv::COLOR_BGR2RGB);
  data->width = webcam->cameraFrame.cols;
  data->height = webcam->cameraFrame.rows;
  memcpy(data->data, webcam->cameraFrame.data, data->width * data->height * 3);
  return 0;
}

#elif defined(__linux__)

extern "C"
{
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
}

struct webcamInfo
{
  uint video_stream_idx;
  uint width, height;
  enum AVPixelFormat pix_fmt;

  uint8_t *video_data[4];
  int video_linesize[4];
  int video_bufsize;

  AVFrame *frame;
  AVPacket *pkt;

  AVFormatContext *fmt_ctx;
  AVStream *video_stream;
  AVCodecContext *dec_ctx;
};

static webcamInfo *webcamInfoAlloc()
{
  webcamInfo *webcam = new webcamInfo();
  return webcam;
}

static int initWebcam(webcamInfo *webcam)
{
  avdevice_register_all();

  AVDictionary *options = NULL;
  av_dict_set(&options, "framerate", "20", 0);

  AVFormatContext *pAVFormatContext = NULL;

  assert(!avformat_open_input(&pAVFormatContext, "/dev/video0", av_find_input_format("v4l2"), NULL));
  webcam->fmt_ctx = pAVFormatContext;
  assert(!(avformat_find_stream_info(pAVFormatContext, NULL) < 0));

  AVStream *st;
  const AVCodec *dec = NULL;

  int ret = av_find_best_stream(webcam->fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
  assert(!(ret < 0));

  int stream_index = ret;
  st = webcam->fmt_ctx->streams[stream_index];

  dec = avcodec_find_decoder(st->codecpar->codec_id);
  if (!dec)
  {
    fprintf(stderr, "Failed to find %s codec\n",
            av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
    return AVERROR(EINVAL);
  }

  webcam->dec_ctx = avcodec_alloc_context3(dec);
  if (!webcam->dec_ctx)
  {
    fprintf(stderr, "Failed to allocate the %s codec context\n",
            av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
    return AVERROR(ENOMEM);
  }

  if ((ret = avcodec_parameters_to_context(webcam->dec_ctx, st->codecpar)) < 0)
  {
    fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
            av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
    return ret;
  }

  if ((ret = avcodec_open2(webcam->dec_ctx, dec, NULL)) < 0)
  {
    fprintf(stderr, "Failed to open %s codec\n",
            av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
    return ret;
  }
  webcam->video_stream_idx = stream_index;
  webcam->video_stream = webcam->fmt_ctx->streams[stream_index];

  webcam->width = webcam->dec_ctx->width;
  webcam->height = webcam->dec_ctx->height;
  webcam->pix_fmt = webcam->dec_ctx->pix_fmt;

  webcam->video_bufsize = av_image_alloc(webcam->video_data, webcam->video_linesize,
                                         webcam->width, webcam->height, webcam->pix_fmt, 1);
  assert(!(webcam->video_bufsize < 0));

  assert(webcam->video_stream);

  webcam->frame = av_frame_alloc();
  if (!webcam->frame)
  {
    fprintf(stderr, "Could not allocate frame\n");
    return AVERROR(ENOMEM);
  }

  webcam->pkt = av_packet_alloc();
  if (!webcam->pkt)
  {
    fprintf(stderr, "Could not allocate packet\n");
    return AVERROR(ENOMEM);
  }

  return 0;
}

static webcam_data initWebcamData(webcamInfo *webcam)
{
  return {(uint8_t *)malloc(webcam->width * webcam->height * 3), webcam->width, webcam->height, 4};
}

static int readWebcamFrame(webcam_data *data, webcamInfo *webcam)
{
  if (!webcam->video_stream)
  {
    fprintf(stderr, "Could not find video stream");
    return 1;
  }

  if (av_read_frame(webcam->fmt_ctx, webcam->pkt) < 0)
  {
    fprintf(stderr, "Could not read frame");
    return 1;
  }

  if (webcam->pkt->stream_index != webcam->video_stream_idx)
  {
    fprintf(stderr, "Incorrect frame type");
    return 1;
  }

  int ret = avcodec_send_packet(webcam->dec_ctx, webcam->pkt);
  if (ret < 0)
  {
    // fprintf(stderr, "Error submitting a packet for decoding (%s)\n", av_err2str(ret));
    return ret;
  }

  ret = avcodec_receive_frame(webcam->dec_ctx, webcam->frame);
  if (ret < 0)
  {
    // those two return values are special and mean there is no output
    // frame available, but there were no errors during decoding
    if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
      return 0;

    // fprintf(stderr, "Error during decoding (%s)\n", av_err2str(ret));
    return ret;
  }

  av_image_copy(webcam->video_data, webcam->video_linesize,
                (const uint8_t **)(webcam->frame->data), webcam->frame->linesize,
                webcam->pix_fmt, webcam->width, webcam->height);

  av_frame_unref(webcam->frame);

  if (webcam->pix_fmt == AV_PIX_FMT_YUYV422 && !webcam->video_data[1])
  {
    // 2 bytes per pixel, take 2 pixels and use YUV conversion to change it
    // parse every 4 bytes
    struct YUV422
    {
      uint8_t y1, u, y2, v;
    };

    for (uint i = 0; i < webcam->video_bufsize / sizeof(YUV422); i++)
    {
      membuild(YUV422, yuv, webcam->video_data[0] + i * sizeof(YUV422));

      Color3 byte1 = {yuv.y1, yuv.u, yuv.v};
      byte1 = YCbCrToRGB(byte1);
      Color3 byte2 = {yuv.y2, yuv.u, yuv.v};
      byte2 = YCbCrToRGB(byte2);

      memcpy(data->data + 2 * i * sizeof(Color3), &byte1, sizeof(Color3));
      memcpy(data->data + (2 * i + 1) * sizeof(Color3), &byte2, sizeof(Color3));
    }

    // for reder with opencv as it uses bgr
    for(uint i=0;i<data->width*data->height;i++){
      membuild(Color3,c,data->data+3*i);
      Color3 bgr={c.z,c.y,c.x};
      memcpy(data->data+3*i,&bgr,3);
    }
  }

  return 0;
}

#elif defined(__APPLE__)

#elif defined(_WIN32)
#else

#endif