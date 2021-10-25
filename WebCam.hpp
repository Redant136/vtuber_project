#pragma once

#include "chevan_utils.h"
using namespace chevan_utils;

#ifdef __linux__

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
}



#elif __APPLE__

#elif _WIN32
#else

#endif
