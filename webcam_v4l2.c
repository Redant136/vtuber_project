
#include "chevan_utils.h"
#include "webcam.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <linux/videodev2.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

enum io_method
{
  IO_METHOD_READ,
  IO_METHOD_MMAP,
  IO_METHOD_USERPTR,
};

struct buffer
{
  void *start;
  size_t length;
};

struct buffer *buffers;
static unsigned int n_buffers;

struct webcamInfo
{
  enum io_method io;
  int fd;
  int force_format;
  struct v4l2_format fmt;
};

static void errno_exit(const char *s)
{
  fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
  exit(EXIT_FAILURE);
}

static int xioctl(int fh, int request, void *arg)
{
  int r;

  do
  {
    r = ioctl(fh, request, arg);
  } while (-1 == r && EINTR == errno);

  return r;
}

static void init_read(unsigned int buffer_size, const char *dev_name, const int fd)
{
  buffers = calloc(1, sizeof(*buffers));

  if (!buffers)
  {
    fprintf(stderr, "Out of memory\\n");
    exit(EXIT_FAILURE);
  }

  buffers[0].length = buffer_size;
  buffers[0].start = malloc(buffer_size);

  if (!buffers[0].start)
  {
    fprintf(stderr, "Out of memory\\n");
    exit(EXIT_FAILURE);
  }
}

static void init_mmap(const char *dev_name, const int fd)
{
  struct v4l2_requestbuffers req;

  CLEAR(req);

  req.count = 4;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;

  if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req))
  {
    if (EINVAL == errno)
    {
      fprintf(stderr, "%s does not support "
                      "memory mappingn",
              dev_name);
      exit(EXIT_FAILURE);
    }
    else
    {
      errno_exit("VIDIOC_REQBUFS");
    }
  }

  if (req.count < 2)
  {
    fprintf(stderr, "Insufficient buffer memory on %s\\n",
            dev_name);
    exit(EXIT_FAILURE);
  }

  buffers = calloc(req.count, sizeof(*buffers));

  if (!buffers)
  {
    fprintf(stderr, "Out of memory\\n");
    exit(EXIT_FAILURE);
  }

  for (n_buffers = 0; n_buffers < req.count; ++n_buffers)
  {
    struct v4l2_buffer buf;

    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = n_buffers;

    if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
      errno_exit("VIDIOC_QUERYBUF");

    buffers[n_buffers].length = buf.length;
    buffers[n_buffers].start =
        mmap(NULL /* start anywhere */,
             buf.length,
             PROT_READ | PROT_WRITE /* required */,
             MAP_SHARED /* recommended */,
             fd, buf.m.offset);

    if (MAP_FAILED == buffers[n_buffers].start)
      errno_exit("mmap");
  }
}

static void init_userp(unsigned int buffer_size, const char *dev_name, const int fd)
{
  struct v4l2_requestbuffers req;

  CLEAR(req);

  req.count = 4;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_USERPTR;

  if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req))
  {
    if (EINVAL == errno)
    {
      fprintf(stderr, "%s does not support "
                      "user pointer i/on",
              dev_name);
      exit(EXIT_FAILURE);
    }
    else
    {
      errno_exit("VIDIOC_REQBUFS");
    }
  }

  buffers = calloc(4, sizeof(*buffers));

  if (!buffers)
  {
    fprintf(stderr, "Out of memory\\n");
    exit(EXIT_FAILURE);
  }

  for (n_buffers = 0; n_buffers < 4; ++n_buffers)
  {
    buffers[n_buffers].length = buffer_size;
    buffers[n_buffers].start = malloc(buffer_size);

    if (!buffers[n_buffers].start)
    {
      fprintf(stderr, "Out of memory\\n");
      exit(EXIT_FAILURE);
    }
  }
}

static void convertFromYUYV422(void *dst, uint8_t *data, size_t size)
{
  // 2 bytes per pixel, take 2 pixels and use YUV conversion to change it
  // parse every 4 bytes
  struct YUV422
  {
    uint8_t y1, u, y2, v;
  };

  for (uint i = 0; i < size / sizeof(struct YUV422); i++)
  {
    membuild(struct YUV422, yuv, data + i * sizeof(struct YUV422));

    color3_t byte1 = {yuv.y1, yuv.u, yuv.v};
    byte1 = YCbCrToRGB(byte1);
    color3_t byte2 = {yuv.y2, yuv.u, yuv.v};
    byte2 = YCbCrToRGB(byte2);

    memcpy(dst + 2 * i * sizeof(color3_t), &byte1, sizeof(color3_t));
    memcpy(dst + (2 * i + 1) * sizeof(color3_t), &byte2, sizeof(color3_t));
  }
}

webcamInfo *webcamInfoAlloc()
{
  webcamInfo *webcam = malloc(sizeof(webcamInfo));
  webcam->io = IO_METHOD_USERPTR;
  webcam->fd = -1;
  webcam->force_format = true;
  return webcam;
}
int initWebcam(webcamInfo *webcam, unsigned int n)
{
  char dev_name[16] = {0};
  sprintf(dev_name, "/dev/video%d", n);
  struct stat st;

  if (-1 == stat(dev_name, &st))
  {
    fprintf(stderr, "Cannot identify '%s': %d, %s\\n",
            dev_name, errno, strerror(errno));
    exit(EXIT_FAILURE);
  }

  if (!S_ISCHR(st.st_mode))
  {
    fprintf(stderr, "%s is no devicen", dev_name);
    exit(EXIT_FAILURE);
  }

  webcam->fd = open(dev_name, O_RDWR | O_NONBLOCK, 0);

  if (-1 == webcam->fd)
  {
    fprintf(stderr, "Cannot open '%s': %d, %s\\n",
            dev_name, errno, strerror(errno));
    exit(EXIT_FAILURE);
  }

  // -------------------------

  struct v4l2_capability cap;
  struct v4l2_cropcap cropcap;
  struct v4l2_crop crop;
  unsigned int min;

  if (-1 == xioctl(webcam->fd, VIDIOC_QUERYCAP, &cap))
  {
    if (EINVAL == errno)
    {
      fprintf(stderr, "%s is no V4L2 device\\n",
              dev_name);
      exit(EXIT_FAILURE);
    }
    else
    {
      errno_exit("VIDIOC_QUERYCAP");
    }
  }

  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
  {
    fprintf(stderr, "%s is no video capture device\\n",
            dev_name);
    exit(EXIT_FAILURE);
  }

  switch (webcam->io)
  {
  case IO_METHOD_READ:
    if (!(cap.capabilities & V4L2_CAP_READWRITE))
    {
      fprintf(stderr, "%s does not support read i/o\\n",
              dev_name);
      exit(EXIT_FAILURE);
    }
    break;

  case IO_METHOD_MMAP:
  case IO_METHOD_USERPTR:
    if (!(cap.capabilities & V4L2_CAP_STREAMING))
    {
      fprintf(stderr, "%s does not support streaming i/o\\n",
              dev_name);
      exit(EXIT_FAILURE);
    }
    break;
  }

  /* Select video input, video standard and tune here. */

  CLEAR(cropcap);

  cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  if (0 == xioctl(webcam->fd, VIDIOC_CROPCAP, &cropcap))
  {
    crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    crop.c = cropcap.defrect; /* reset to default */

    if (-1 == xioctl(webcam->fd, VIDIOC_S_CROP, &crop))
    {
      switch (errno)
      {
      case EINVAL:
        /* Cropping not supported. */
        break;
      default:
        /* Errors ignored. */
        break;
      }
    }
  }
  else
  {
    /* Errors ignored. */
  }

  CLEAR(webcam->fmt);

  webcam->fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (webcam->force_format)
  {
    webcam->fmt.fmt.pix.width = 640;
    webcam->fmt.fmt.pix.height = 480;
    webcam->fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    webcam->fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    if (-1 == xioctl(webcam->fd, VIDIOC_S_FMT, &webcam->fmt))
      errno_exit("VIDIOC_S_FMT");

    /* Note VIDIOC_S_FMT may change width and height. */
  }
  else
  {
    /* Preserve original settings as set by v4l2-ctl for example */
    if (-1 == xioctl(webcam->fd, VIDIOC_G_FMT, &webcam->fmt))
      errno_exit("VIDIOC_G_FMT");
  }

  /* Buggy driver paranoia. */
  min = webcam->fmt.fmt.pix.width * 2;
  if (webcam->fmt.fmt.pix.bytesperline < min)
    webcam->fmt.fmt.pix.bytesperline = min;
  min = webcam->fmt.fmt.pix.bytesperline * webcam->fmt.fmt.pix.height;
  if (webcam->fmt.fmt.pix.sizeimage < min)
    webcam->fmt.fmt.pix.sizeimage = min;

  switch (webcam->io)
  {
  case IO_METHOD_READ:
    init_read(webcam->fmt.fmt.pix.sizeimage, dev_name, webcam->fd);
    break;

  case IO_METHOD_MMAP:
    init_mmap(dev_name, webcam->fd);
    break;

  case IO_METHOD_USERPTR:
    init_userp(webcam->fmt.fmt.pix.sizeimage, dev_name, webcam->fd);
    break;
  }

  // ----------------------------------------
  enum v4l2_buf_type type;

  switch (webcam->io)
  {
  case IO_METHOD_READ:
    /* Nothing to do. */
    break;

  case IO_METHOD_MMAP:
    for (uint i = 0; i < n_buffers; ++i)
    {
      struct v4l2_buffer buf;

      CLEAR(buf);
      buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory = V4L2_MEMORY_MMAP;
      buf.index = i;

      if (-1 == xioctl(webcam->fd, VIDIOC_QBUF, &buf))
        errno_exit("VIDIOC_QBUF");
    }
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(webcam->fd, VIDIOC_STREAMON, &type))
      errno_exit("VIDIOC_STREAMON");
    break;

  case IO_METHOD_USERPTR:
    for (uint i = 0; i < n_buffers; ++i)
    {
      struct v4l2_buffer buf;

      CLEAR(buf);
      buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory = V4L2_MEMORY_USERPTR;
      buf.index = i;
      buf.m.userptr = (unsigned long)buffers[i].start;
      buf.length = buffers[i].length;

      if (-1 == xioctl(webcam->fd, VIDIOC_QBUF, &buf))
        errno_exit("VIDIOC_QBUF");
    }
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(webcam->fd, VIDIOC_STREAMON, &type))
      errno_exit("VIDIOC_STREAMON");
    break;
  }
  return 0;
}
webcam_data initWebcamData(webcamInfo *webcam)
{
  // printf("%c,%c,%c,%c\n",
  //        webcam->fmt.fmt.pix.pixelformat >> 0 & (255),
  //        webcam->fmt.fmt.pix.pixelformat >> 8 & (255),
  //        webcam->fmt.fmt.pix.pixelformat >> 16 & (255),
  //        webcam->fmt.fmt.pix.pixelformat >> 24 & (255));
  return (webcam_data){malloc(webcam->fmt.fmt.pix.width * webcam->fmt.fmt.pix.height * 3), webcam->fmt.fmt.pix.width, webcam->fmt.fmt.pix.height, 3};
}
int readWebcamFrame(webcam_data *data, webcamInfo *webcam)
{
  struct v4l2_buffer buf;
  switch (webcam->io)
  {
  case IO_METHOD_READ:
  {
    if (-1 == read(webcam->fd, buffers[0].start, buffers[0].length))
    {
      switch (errno)
      {
      case EAGAIN:
        return 0;

      case EIO:
        /* Could ignore EIO, see spec. */

        /* fall through */

      default:
        errno_exit("read");
      }
    }

    break;
  }
  case IO_METHOD_MMAP:
  {
    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(webcam->fd, VIDIOC_DQBUF, &buf))
    {
      switch (errno)
      {
      case EAGAIN:
        return 1;

      case EIO:
      default:
        errno_exit("VIDIOC_DQBUF");
      }
    }

    assert(buf.index < n_buffers);

    convertFromYUYV422(data->data, buffers[buf.index].start, buf.bytesused);

    if (-1 == xioctl(webcam->fd, VIDIOC_QBUF, &buf))
      errno_exit("VIDIOC_QBUF");

    break;
  }
  case IO_METHOD_USERPTR:
  {
    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_USERPTR;

    if (-1 == xioctl(webcam->fd, VIDIOC_DQBUF, &buf))
    {
      switch (errno)
      {
      case EAGAIN:
        return 1;
      case EIO:
      default:
        errno_exit("VIDIOC_DQBUF");
      }
    }

    convertFromYUYV422(data->data,(void*)buf.m.userptr,buf.bytesused);
    // memcpy(data->data, (void *)buf.m.userptr, buf.bytesused);

    if (-1 == xioctl(webcam->fd, VIDIOC_QBUF, &buf))
      errno_exit("VIDIOC_QBUF");
    break;
  }
  }
  return 0;
}
int webcamUninit(webcamInfo *webcam, webcam_data *data)
{
  if (webcam)
  {
    switch (webcam->io)
    {
    case IO_METHOD_READ:
      break;
    case IO_METHOD_MMAP:
    case IO_METHOD_USERPTR:
      enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      if (-1 == xioctl(webcam->fd, VIDIOC_STREAMOFF, &type))
        errno_exit("VIDIOC_STREAMOFF");
      break;
    }

    // ---------------
    switch (webcam->io)
    {
    case IO_METHOD_READ:
    case IO_METHOD_USERPTR:
      free(data->data);
      break;

    case IO_METHOD_MMAP:
      if (-1 == munmap(data->data, webcam->fmt.fmt.pix.sizeimage))
        errno_exit("munmap");
      break;
    }

    // -----------------

    if (-1 == close(webcam->fd))
      errno_exit("close");

    webcam->fd = -1;
  }
  return 0;
}
