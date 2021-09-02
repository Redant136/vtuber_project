#pragma once
#ifndef CHEVAN_UTILS_H
#define CHEVAN_UTILS_H
#define CHEVAN_UTILS_VERSION "1.3.2"

#ifndef CHEVAN_UTILS_NO_SHORTHANDS

#if defined(CHEVAN_UTILS_INCL) || defined(CHEVAN_UTILS_incl)
#define CHEVAN_UTILS_COMMON_INCLUDES
#endif
#if defined(CHEVAN_UTILS_BYTES) || defined(CHEVAN_UTILS_bytes)
#define CHEVAN_UTILS_BYTE_TYPEDEF
#endif
#if defined(CHEVAN_UTILS_print)
#define CHEVAN_UTILS_PRINT
#endif
#if defined(CHEVAN_UTILS_RAND) || defined(CHEVAN_UTILS_random) || defined(CHEVAN_UTILS_rand)
#define CHEVAN_UTILS_RANDOM
#endif
#if defined(CHEVAN_UTILS_VEC) || defined(CHEVAN_UTILS_vector) || defined(CHEVAN_UTILS_vec)
#define CHEVAN_UTILS_VECTOR
#endif
#if defined(CHEVAN_UTILS_math)
#define CHEVAN_UTILS_MATH
#endif
#if defined(CHEVAN_UTILS_B64) || defined(CHEVAN_UTILS_b64)
#define CHEVAN_UTILS_BASE64
#endif
#endif // !CHEVAN_UTILS_NO_SHORTHANDS

// #define CHEVAN_UTILS_ALL
#ifdef CHEVAN_UTILS_ALL
#define CHEVAN_UTILS_COMMON_INCLUDES
#define CHEVAN_UTILS_PRINT
#define CHEVAN_UTILS_RANDOM
#define CHEVAN_UTILS_BYTE_TYPEDEF
#define CHEVAN_UTILS_VECTOR
#define CHEVAN_UTILS_MATH
#define CHEVAN_UTILS_COLOR
#define CHEVAN_UTILS_PERLIN_NOISE
#define CHEVAN_UTILS_FASTCALC
#define CHEVAN_UTILS_BASE64
#define CHEVAN_UTILS_MACRO_MAGIC
#endif // CHEVAN_UTILS_ALL

#ifdef CHEVAN_UTILS_COMMON_INCLUDES
#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <assert.h>
#include <stdlib.h>
#include <functional>
#include <string>

#endif // CHEVAN_UTILS_COMMON_INCLUDES

#include <stddef.h>
#include <assert.h>

//includes
#ifdef CHEVAN_UTILS_PRINT
#include <iostream>
#include <string>
#endif // CHEVAN_UTILS_PRINT
#ifdef CHEVAN_UTILS_RANDOM
#include <random>
#endif // CHEVAN_UTILS_RANDOM
#ifdef CHEVAN_UTILS_BYTE_TYPEDEF
#include <stdint.h>
#endif // !CHEVAN_UTILS_BYTE_TYPEDEF
#ifdef CHEVAN_UTILS_VECTOR
#include <stdint.h>
#define toV2(v) v2((v).x, (v).y)
#define toV3(v) v3((v).x, (v).y, (v).z)
#define toV4(v) v4((v).x, (v).y, (v).z, (v).w)
#define toIV2(v) iv2((v).x, (v).y)
#define toIV3(v) iv3((v).x, (v).y, (v).z)
#define toIV4(v) iv4((v).x, (v).y, (v).z, (v).w)

#endif                   // CHEVAN_UTILS_VECTOR
#ifdef CHEVAN_UTILS_MATH // must be used with chevan_vectors or define CHEVAN_UTILS_MATH_VEC
#include <cmath>
#include <string.h>
#define PIf 3.1415926535897f
#define ONE_OVER_SQUARE_ROOT_OF_TWO_PI 0.3989422804
#define ONE_OVER_SQUARE_ROOT_OF_TWO_PIf 0.3989422804f
#define EULERS_NUMBER 2.7182818284590452353602874713527
#define EULERS_NUMBERf 2.7182818284590452353602874713527f

#endif // CHEVAN_UTILS_MATH
#ifdef CHEVAN_UTILS_PERLIN_NOISE
#include <stdint.h>
#endif // CHEVAN_UTILS_PERLIN_NOISE
#ifdef CHEVAN_UTILS_FASTCALC
#include <cmath>
#ifndef CHEVAN_UTILS_FASTCALC_PRECISION
#define CHEVAN_UTILS_FASTCALC_PRECISION 9
#endif

#endif // CHEVAN_UTILS_FASTCALC
#ifdef CHEVAN_UTILS_BASE64
#include <algorithm>
#include <stdexcept>
#include <string>
#if __cplusplus >= 201703L
#include <string_view>
#endif // __cplusplus >= 201703L

#endif // CHEVAN_UTILS_BASE64
#ifdef CHEVAN_UTILS_MACRO_MAGIC
#ifndef CONCAT
#define CONCAT(a, b) a##b
#endif
#ifndef EVAL
#define EVAL(...) EVAL1024(__VA_ARGS__)
#define EVAL1024(...) EVAL512(EVAL512(__VA_ARGS__))
#define EVAL512(...) EVAL256(EVAL256(__VA_ARGS__))
#define EVAL256(...) EVAL128(EVAL128(__VA_ARGS__))
#define EVAL128(...) EVAL64(EVAL64(__VA_ARGS__))
#define EVAL64(...) EVAL32(EVAL32(__VA_ARGS__))
#define EVAL32(...) EVAL16(EVAL16(__VA_ARGS__))
#define EVAL16(...) EVAL8(EVAL8(__VA_ARGS__))
#define EVAL8(...) EVAL4(EVAL4(__VA_ARGS__))
#define EVAL4(...) EVAL2(EVAL2(__VA_ARGS__))
#define EVAL2(...) EVAL1(EVAL1(__VA_ARGS__))
#define EVAL1(...) __VA_ARGS__
#endif

#ifndef MAP
#define FIRST(a, ...) a
#define SECOND(a, b, ...) b
#define DEFER1(m) m EMPTY()
#define DEFER2(m) m EMPTY EMPTY()()
#define DEFER3(m) m EMPTY EMPTY EMPTY()()()
#define DEFER4(m) m EMPTY EMPTY EMPTY EMPTY()()()()

#define IS_PROBE(...) SECOND(__VA_ARGS__, 0)
#define PROBE() ~, 1

#define NOT(x) IS_PROBE(CONCAT(_NOT_, x))
#define _NOT_0 PROBE()

#define BOOL(x) NOT(NOT(x))

#define IF_ELSE(condition) _IF_ELSE(BOOL(condition))
#define _IF_ELSE(condition) CONCAT(_IF_, condition)

#define _IF_1(...) __VA_ARGS__ _IF_1_ELSE
#define _IF_0(...) _IF_0_ELSE

#define _IF_1_ELSE(...)
#define _IF_0_ELSE(...) __VA_ARGS__

#define HAS_ARGS(...) BOOL(FIRST(_END_OF_ARGUMENTS_ __VA_ARGS__)())
#define _END_OF_ARGUMENTS_() 0

#define MAP(m, first, ...)                                                 \
  m(first)                                                                 \
      IF_ELSE(HAS_ARGS(__VA_ARGS__))(                                      \
          DEFER2(_MAP)()(m, __VA_ARGS__))(/* Do nothing, just terminate */ \
      )
#define _MAP() MAP
#endif

#endif // CHEVAN_UTILS_MACRO_MAGIC

namespace chevan_utils
{
#define convertVec2(dest, src) \
  dest.x = src.x;              \
  dest.y = src.y;
#define convertVec3(dest, src) convertVec2(dest, src) dest.z = src.z;
#define convertVec4(dest, src) convertVec3(dest, src) dest.w = src.w;
#define vec2List(v) (v).x, (v).y
#define vec3List(v) vec2List(v), (v).z
#define vec4List(v) vec3List(v), (v).w
#define vec2ToArray(v) \
  {                    \
    vec2List(v)        \
  }
#define vec3ToArray(v) \
  {                    \
    vec3List(v)        \
  }
#define vec4ToArray(v) \
  {                    \
    vec4List(v)        \
  }

  typedef unsigned char uchar;
  typedef unsigned short ushort;
  typedef unsigned int uint;
  typedef unsigned long ulong;
  typedef unsigned long long ullong;
  typedef long long llong;
  template <typename T, typename L = ulong>
  struct Array
  {
    L length = 0;
    T *arr = NULL;
    Array() = default;
    Array(L l)
    {
      length = l;
      arr = new T[l];
    }
    Array(L l, T *arr)
    {
      this->length = l;
      this->arr = arr;
    }
    template <typename I = ulong>
    T &operator[](I i)
    {
      assert(arr);
      assert(i < length + 1);
      return arr[i];
    }
    template <typename I = ulong>
    T *operator+(I i)
    {
      return arr + i;
    }
    Array<T, L> clone()
    {
      T *a = new T[length];
      memcpy(a, arr, sizeof(T) * length);
      return {length, a};
    }
    void free()
    {
      delete[] arr;
      arr = NULL;
    }
  };
  enum class Cardinal8dir
  {
    NORTH,
    SOUTH,
    EAST,
    WEST,
    NORTH_EAST,
    NORTH_WEST,
    SOUTH_EAST,
    SOUTH_WEST,
    CENTER
  };
  static inline float degreeToRad(float degree) { return degree / 180.f * 3.1415926535897f; }
  static inline float radToDegree(float rad) { return rad / 3.1415926535897f * 180; }

#ifdef CHEVAN_UTILS_VECTOR
  struct v2;
  struct v3;
  struct v4;
  struct iv2;
  struct iv3;
  struct iv4;

  struct v2
  {
    union
    {
      struct
      {
        float x, y;
      };
      float elements[2];
    };
    v2()
    {
      this->x = 0;
      this->y = 0;
    }
    v2(float x, float y)
    {
      this->x = x;
      this->y = y;
    }
    v2(float d)
    {
      this->x = d;
      this->y = 0;
    }

    v2(const v2 &base)
    {
      this->x = base.x;
      this->y = base.y;
    }
    v2(const iv2 &base);
    v2(const v3 &base);
    v2(const iv3 &base);
    v2(const v4 &base);
    v2(const iv4 &base);
    v2 operator+(const v2 a) { return v2(x + a.x, y + a.y); }
    v2 operator-(const v2 a) { return v2(x - a.x, y - a.y); }
    void operator+=(const v2 a)
    {
      x += a.x;
      y += a.y;
    }
    void operator-=(const v2 a)
    {
      x -= a.x;
      y -= a.y;
    }
    v2 operator+(const float a) { return v2(x + a, y + a); }
    v2 operator-(const float a) { return v2(x - a, y - a); }
    v2 operator*(const float a) { return v2(x * a, y * a); }
    v2 operator/(const float a) { return v2(x / a, y / a); }
    void operator+=(const float a)
    {
      x += a;
      y += a;
    }
    void operator-=(const float a)
    {
      x -= a;
      y -= a;
    }
    void operator*=(const float a)
    {
      x *= a;
      y *= a;
    }
    void operator/=(const float a)
    {
      x /= a;
      y /= a;
    }
  };
  struct v3
  {
    union
    {
      struct
      {
        float x;
        float y;
        float z;
      };
      struct
      {
        float r;
        float g;
        float b;
      };
      float elements[3];
    };
    v3()
    {
      this->x = 0;
      this->y = 0;
      this->z = 0;
    }
    v3(float x, float y, float z)
    {
      this->x = x;
      this->y = y;
      this->z = z;
    }
    v3(float d)
    {
      this->x = d;
      this->y = 0;
      this->z = 0;
    }

    v3(const v2 &base)
    {
      this->x = base.x;
      this->y = base.y;
      this->z = 0;
    }
    v3(const iv2 &base);
    v3(const v3 &base)
    {
      this->x = base.x;
      this->y = base.y;
      this->z = base.z;
    }
    v3(const iv3 &base);
    v3(const v4 &base);
    v3(const iv4 &base);
    v3 operator+(const v3 a) { return v3(x + a.x, y + a.y, z + a.z); }
    v3 operator-(const v3 a) { return v3(x - a.x, y - a.y, z - a.z); }
    void operator+=(const v3 a)
    {
      x += a.x;
      y += a.y;
      z += a.z;
    }
    void operator-=(const v3 a)
    {
      x -= a.x;
      y -= a.y;
      z -= a.z;
    }
    v3 operator+(const float a) { return v3(x + a, y + a, z + a); }
    v3 operator-(const float a) { return v3(x - a, y - a, z - a); }
    v3 operator*(const float a) { return v3(x * a, y * a, z * a); }
    v3 operator/(const float a) { return v3(x / a, y / a, z / a); }
    void operator+=(const float a)
    {
      x += a;
      y += a;
      z += a;
    }
    void operator-=(const float a)
    {
      x -= a;
      y -= a;
      z -= a;
    }
    void operator*=(const float a)
    {
      x *= a;
      y *= a;
      z *= a;
    }
    void operator/=(const float a)
    {
      x /= a;
      y /= a;
      z /= a;
    }
    v3 &operator=(const v2 &a)
    {
      this->x = a.x;
      this->y = a.y;
      return *this;
    }
  };
  struct v4
  {
    union
    {
      struct
      {
        float x;
        float y;
        union
        {
          struct
          {
            float z;
            float w;
          };
          struct
          {
            float width;
            float height;
          };
        };
      };
      struct
      {
        float r;
        float g;
        float b;
        float a;
      };
      float elements[4];
    };
    v4()
    {
      this->x = 0;
      this->y = 0;
      this->z = 0;
      this->w = 0;
    }
    v4(float x, float y, float z, float w)
    {
      this->x = x;
      this->y = y;
      this->z = z;
      this->w = w;
    }
    v4(float d)
    {
      this->x = d;
      this->y = 0;
      this->z = 0;
      this->w = 0;
    }

    v4(const v2 &base)
    {
      this->x = base.x;
      this->y = base.y;
      this->z = 0;
      this->w = 0;
    }
    v4(const iv2 &base);
    v4(const v3 &base)
    {
      this->x = base.x;
      this->y = base.y;
      this->z = base.z;
      this->w = 0;
    }
    v4(const iv3 &base);
    v4(const v4 &base)
    {
      this->x = base.x;
      this->y = base.y;
      this->z = base.z;
      this->w = base.w;
    }
    v4(const iv4 &base);
    v4 operator+(const v4 a) { return v4(x + a.x, y + a.y, z + a.z, w + a.w); }
    v4 operator-(const v4 a) { return v4(x - a.x, y - a.y, z - a.z, w - a.w); }
    void operator+=(const v4 a)
    {
      x += a.x;
      y += a.y;
      z += a.z;
      w += a.w;
    }
    void operator-=(const v4 a)
    {
      x -= a.x;
      y -= a.y;
      z -= a.z;
      w -= a.w;
    }
    v4 operator+(const float a) { return v4(x + a, y + a, z + a, w + a); }
    v4 operator-(const float a) { return v4(x - a, y - a, z - a, w - a); }
    v4 operator*(const float a) { return v4(x * a, y * a, z * a, w * a); }
    v4 operator/(const float a) { return v4(x / a, y / a, z / a, w / a); }
    void operator+=(const float a)
    {
      x += a;
      y += a;
      z += a;
      z += a;
    }
    void operator-=(const float a)
    {
      x -= a;
      y -= a;
      z -= a;
      z -= a;
    }
    void operator*=(const float a)
    {
      x *= a;
      y *= a;
      z *= a;
      z *= a;
    }
    void operator/=(const float a)
    {
      x /= a;
      y /= a;
      z /= a;
      z /= a;
    }
    v4 &operator=(const v2 &a)
    {
      this->x = a.x;
      this->y = a.y;
      return *this;
    }
    v4 &operator=(const v3 &a)
    {
      this->x = a.x;
      this->y = a.y;
      this->z = a.z;
      return *this;
    }
  };
  struct iv2
  {
    union
    {
      struct
      {
        int32_t x, y;
      };
      int32_t elements[2];
    };
    iv2()
    {
      this->x = 0;
      this->y = 0;
    }
    iv2(int32_t x, int32_t y)
    {
      this->x = x;
      this->y = y;
    }
    iv2(int32_t d)
    {
      this->x = d;
      this->y = 0;
    }

    iv2(const v2 &base)
    {
      this->x = base.x;
      this->y = base.y;
    }
    iv2(const iv2 &base)
    {
      this->x = base.x;
      this->y = base.y;
    }
    iv2(const v3 &base)
    {
      this->x = base.x;
      this->y = base.y;
    }
    iv2(const iv3 &base);
    iv2(const v4 &base)
    {
      this->x = base.x;
      this->y = base.y;
    }
    iv2(const iv4 &base);
    bool operator==(const iv2 &a) { return x == a.x && y == a.y; }
    bool operator!=(const iv2 &a) { return x != a.x || y != a.y; }
    iv2 operator+(const iv2 a) { return iv2(x + a.x, y + a.y); }
    iv2 operator-(const iv2 a) { return iv2(x - a.x, y - a.y); }
    void operator+=(const iv2 a)
    {
      x += a.x;
      y += a.y;
    }
    void operator-=(const iv2 a)
    {
      x -= a.x;
      y -= a.y;
    }
    iv2 operator+(const int32_t a) { return iv2(x + a, y + a); }
    iv2 operator-(const int32_t a) { return iv2(x - a, y - a); }
    iv2 operator*(const int32_t a) { return iv2(x * a, y * a); }
    iv2 operator/(const int32_t a) { return iv2(x / a, y / a); }
    void operator+=(const int32_t a)
    {
      x += a;
      y += a;
    }
    void operator-=(const int32_t a)
    {
      x -= a;
      y -= a;
    }
    void operator*=(const int32_t a)
    {
      x *= a;
      y *= a;
    }
    void operator/=(const int32_t a)
    {
      x /= a;
      y /= a;
    }
  };
  struct iv3
  {
    union
    {
      struct
      {
        int32_t x, y, z;
      };
      int32_t elements[3];
    };
    iv3()
    {
      this->x = 0;
      this->y = 0;
      this->z = 0;
    }
    iv3(int32_t x, int32_t y, int32_t z)
    {
      this->x = x;
      this->y = y;
      this->z = z;
    }
    iv3(int32_t d)
    {
      this->x = d;
      this->y = 0;
      this->z = 0;
    }

    iv3(const v2 &base)
    {
      this->x = base.x;
      this->y = base.y;
      this->z = 0;
    }
    iv3(const iv2 &base)
    {
      this->x = base.x;
      this->y = base.y;
      this->z = 0;
    }
    iv3(const v3 &base)
    {
      this->x = base.x;
      this->y = base.y;
      this->z = base.z;
    }
    iv3(const iv3 &base)
    {
      this->x = base.x;
      this->y = base.y;
      this->z = base.z;
    }
    iv3(const v4 &base)
    {
      this->x = base.x;
      this->y = base.y;
      this->z = base.z;
    }
    iv3(const iv4 &base);
    bool operator==(const iv3 &a) { return x == a.x && y == a.y && z == a.z; }
    bool operator!=(const iv3 &a) { return x != a.x || y != a.y || z != a.z; }
    iv3 operator+(const iv3 a) { return iv3(x + a.x, y + a.y, z + a.z); }
    iv3 operator-(const iv3 a) { return iv3(x - a.x, y - a.y, z - a.z); }
    void operator+=(const iv3 a)
    {
      x += a.x;
      y += a.y;
      z += a.z;
    }
    void operator-=(const iv3 a)
    {
      x -= a.x;
      y -= a.y;
      z -= a.z;
    }
    iv3 operator+(const int32_t a) { return iv3(x + a, y + a, z + a); }
    iv3 operator-(const int32_t a) { return iv3(x - a, y - a, z - a); }
    iv3 operator*(const int32_t a) { return iv3(x * a, y * a, z * a); }
    iv3 operator/(const int32_t a) { return iv3(x / a, y / a, z / a); }
    void operator+=(const int32_t a)
    {
      x += a;
      y += a;
      z += a;
    }
    void operator-=(const int32_t a)
    {
      x -= a;
      y -= a;
      z -= a;
    }
    void operator*=(const int32_t a)
    {
      x *= a;
      y *= a;
      z *= a;
    }
    void operator/=(const int32_t a)
    {
      x /= a;
      y /= a;
      z /= a;
    }
    iv3 &operator=(const iv2 &a)
    {
      this->x = a.x;
      this->y = a.y;
      return *this;
    }
  };
  struct iv4
  {
    union
    {
      struct
      {
        int32_t x, y, z, w;
      };
      int32_t elements[4];
    };
    iv4()
    {
      this->x = 0;
      this->y = 0;
      this->z = 0;
      this->w = 0;
    }
    iv4(int32_t x, int32_t y, int32_t z, int32_t w)
    {
      this->x = x;
      this->y = y;
      this->z = z;
      this->w = w;
    }
    iv4(int32_t d)
    {
      this->x = d;
      this->y = 0;
      this->z = 0;
      this->w = 0;
    }

    iv4(const v2 &base)
    {
      this->x = base.x;
      this->y = base.y;
      this->z = 0;
      this->w = 0;
    }
    iv4(const iv2 &base)
    {
      this->x = base.x;
      this->y = base.y;
      this->z = 0;
      this->w = 0;
    }
    iv4(const v3 &base)
    {
      this->x = base.x;
      this->y = base.y;
      this->z = base.z;
      this->w = 0;
    }
    iv4(const iv3 &base)
    {
      this->x = base.x;
      this->y = base.y;
      this->z = base.z;
      this->w = 0;
    }
    iv4(const v4 &base)
    {
      this->x = base.x;
      this->y = base.y;
      this->z = base.z;
      this->w = base.w;
    }
    iv4(const iv4 &base)
    {
      this->x = (int32_t)base.x;
      this->y = (int32_t)base.y;
      this->z = (int32_t)base.z;
      this->w = (int32_t)base.w;
    }
    bool operator==(const iv4 &a)
    {
      return x == a.x && y == a.y && z == a.z && w == a.w;
    }
    bool operator!=(const iv4 &a)
    {
      return x != a.x || y != a.y || z != a.z || w != a.w;
    }
    iv4 operator+(const iv4 a) { return iv4(x + a.x, y + a.y, z + a.z, w + a.w); }
    iv4 operator-(const iv4 a) { return iv4(x - a.x, y - a.y, z - a.z, w - a.w); }
    void operator+=(const iv4 a)
    {
      x += a.x;
      y += a.y;
      z += a.z;
      w += a.w;
    }
    void operator-=(const iv4 a)
    {
      x -= a.x;
      y -= a.y;
      z -= a.z;
      w -= a.w;
    }
    iv4 operator+(const int32_t a) { return iv4(x + a, y + a, z + a, w + a); }
    iv4 operator-(const int32_t a) { return iv4(x - a, y - a, z - a, w - a); }
    iv4 operator*(const int32_t a) { return iv4(x * a, y * a, z * a, w * a); }
    iv4 operator/(const int32_t a) { return iv4(x / a, y / a, z / a, w / a); }
    void operator+=(const int32_t a)
    {
      x += a;
      y += a;
      z += a;
      z += a;
    }
    void operator-=(const int32_t a)
    {
      x -= a;
      y -= a;
      z -= a;
      z -= a;
    }
    void operator*=(const int32_t a)
    {
      x *= a;
      y *= a;
      z *= a;
      z *= a;
    }
    void operator/=(const int32_t a)
    {
      x /= a;
      y /= a;
      z /= a;
      z /= a;
    }
    iv4 &operator=(const iv2 &a)
    {
      this->x = a.x;
      this->y = a.y;
      return *this;
    }
    iv4 &operator=(const iv3 &a)
    {
      this->x = a.x;
      this->y = a.y;
      this->z = a.z;
      return *this;
    }
  };

  inline v2::v2(const iv2 &base)
  {
    this->x = base.x;
    this->y = base.y;
  }
  inline v2::v2(const v3 &base)
  {
    this->x = base.x;
    this->y = base.y;
  }
  inline v2::v2(const iv3 &base)
  {
    this->x = base.x;
    this->y = base.y;
  }
  inline v2::v2(const v4 &base)
  {
    this->x = base.x;
    this->y = base.y;
  }
  inline v2::v2(const iv4 &base)
  {
    this->x = base.x;
    this->y = base.y;
  }

  inline v3::v3(const iv2 &base)
  {
    this->x = base.x;
    this->y = base.y;
    this->z = 0;
  }
  inline v3::v3(const iv3 &base)
  {
    this->x = base.x;
    this->y = base.y;
    this->z = base.z;
  }
  inline v3::v3(const v4 &base)
  {
    this->x = base.x;
    this->y = base.y;
    this->z = base.z;
  }
  inline v3::v3(const iv4 &base)
  {
    this->x = base.x;
    this->y = base.y;
    this->z = base.z;
  }

  inline v4::v4(const iv2 &base)
  {
    this->x = base.x;
    this->y = base.y;
    this->z = 0;
    this->w = 0;
  }
  inline v4::v4(const iv3 &base)
  {
    this->x = base.x;
    this->y = base.y;
    this->z = base.y;
    this->w = 0;
  }
  inline v4::v4(const iv4 &base)
  {
    this->x = base.x;
    this->y = base.y;
    this->z = base.y;
    this->w = base.w;
  }

  inline iv2::iv2(const iv3 &base)
  {
    this->x = base.x;
    this->y = base.y;
  }
  inline iv2::iv2(const iv4 &base)
  {
    this->x = base.x;
    this->y = base.y;
  }

  inline iv3::iv3(const iv4 &base)
  {
    this->x = base.x;
    this->y = base.y;
    this->z = base.z;
  }

#ifndef CHEVAN_UTILS_MATH_V2
#define CHEVAN_UTILS_MATH_V2 v2
#endif // !CHEVAN_UTILS_MATH_V2
#ifndef CHEVAN_UTILS_MATH_V3
#define CHEVAN_UTILS_MATH_V3 v3
#endif // !CHEVAN_UTILS_MATH_V3
#ifndef CHEVAN_UTILS_MATH_V4
#define CHEVAN_UTILS_MATH_V4 v4
#endif // !CHEVAN_UTILS_MATH_V2

#endif // CHEVAN_UTILS_VECTOR
#ifdef CHEVAN_UTILS_PRINT

  template <typename P>
  static void print(P p);
  template <typename P, typename... P2>
  static void print(P p, P2... p2);
  template <typename P>
  static void println(P p);
  template <typename P, typename... P2>
  static void println(P p, P2... p2);

  template <typename Vector>
  static void printVec(Vector v)
  {
    for (auto t : v)
    {
      std::cout << t;
    }
    std::cout << std::endl;
  }
  template <typename V>
  static void printVec2(V v)
  {
    println("{", v.x, ", ", v.y, "}");
  }
  template <typename V>
  static void printVec3(V v)
  {
    println("{", v.x, ", ", v.y, ", ", v.z, "}");
  }
  template <typename V>
  static void printVec4(V v)
  {
    println("{", v.x, ", ", v.y, ", ", v.z, ", ", v.w, "}");
  }
  static void printMat4(float mat[16])
  {
    for (uint i = 0; i < 4; i++)
    {
      println(mat[0 + i * 4], ", ", mat[1 + i * 4], ", ", mat[2 + i * 4], ", ", mat[3 + i * 4]);
    }
  }
  static void printMat4(double mat[16])
  {
    for (uint i = 0; i < 4; i++)
    {
      println(mat[0 + i * 4], ", ", mat[1 + i * 4], ", ", mat[2 + i * 4], ", ", mat[3 + i * 4]);
    }
  }
  template <typename M>
  static void printMat4(M mat)
  {
    printMat4((float *)&mat);
  }
  template <typename M>
  static void printMat4D(M mat)
  {
    printMat4((double *)&mat);
  }
  static void printMem(void *p, ulong length = 256)
  {
    std::string s = "";
    for (uint i = 0; i < length; i++)
    {
      s += std::to_string(((uchar *)p)[i]);
    }
    std::cout << s << std::endl;
  }
  // ----------------------------------------------
  static void print()
  {
    std::cout << std::endl;
  }
  static void println()
  {
    std::cout << std::endl;
  }
  template <typename Printable>
  static void print(Printable p)
  {
    std::cout << p;
  }
  template <typename Printable>
  static void println(Printable p)
  {
    std::cout << p << std::endl;
  }
  static void print(uchar *p)
  {
    std::cout << (void *)p;
  }
  static void println(uchar *p)
  {
    std::cout << (void *)p << std::endl;
  }
  template <typename L>
  static void print(Array<char, L> arr, L length = 256)
  {
    L l = arr.length < length ? arr.length : length;
    std::cout << std::string(arr.arr, arr.arr + l);
  }
  template <typename L>
  static void println(Array<char, L> arr, L length = 256)
  {
    L l = arr.length < length ? arr.length : length;
    std::cout << std::string(arr.arr, arr.arr + l) << std::endl;
  }
  template <typename L>
  static void print(Array<uchar, L> arr, L length = 256)
  {
    L l = arr.length < length ? arr.length : length;
    std::cout << std::string(arr.arr, arr.arr + l);
  }
  template <typename L>
  static void println(Array<uchar, L> arr, L length = 256)
  {
    L l = arr.length < length ? arr.length : length;
    std::cout << std::string(arr.arr, arr.arr + l) << std::endl;
  }
  template <typename T, typename L>
  static void print(Array<T, L> arr, L length = 256)
  {
    L l = arr.length < length ? arr.length : length;
    std::string s = "{";
    s += std::to_string(arr[0]);
    for (uint i = 1; i < l; i++)
    {
      s += "," + std::to_string(arr[i]);
    }
    s += "}";
    std::cout << s;
  }
  template <typename T, typename L>
  static void println(Array<T, L> arr, L length = 256)
  {
    L l = arr.length < length ? arr.length : length;
    std::string s = "{";
    s += std::to_string(arr[0]);
    for (uint i = 1; i < l; i++)
    {
      s += "," + std::to_string(arr[i]);
    }
    s += "}";
    std::cout << s << std::endl;
  }
#ifdef CHEVAN_UTILS_MATH_V2
  static void print(CHEVAN_UTILS_MATH_V2 v)
  {
    print("{", v.x, ", ", v.y, "}");
  }
  static void println(CHEVAN_UTILS_MATH_V2 v)
  {
    println("{", v.x, ", ", v.y, "}");
  }
#endif
#ifdef CHEVAN_UTILS_MATH_V3
  static void print(CHEVAN_UTILS_MATH_V3 v)
  {
    print("{", v.x, ", ", v.y, ", ", v.z, "}");
  }
  static void println(CHEVAN_UTILS_MATH_V3 v)
  {
    println("{", v.x, ", ", v.y, ", ", v.z, "}");
  }
#endif
#ifdef CHEVAN_UTILS_MATH_V4
  static void print(CHEVAN_UTILS_MATH_V4 v)
  {
    print("{", v.x, ", ", v.y, ", ", v.z, ", ", v.w, "}");
  }
  static void println(CHEVAN_UTILS_MATH_V4 v)
  {
    println("{", v.x, ", ", v.y, ", ", v.z, ", ", v.w, "}");
  }
#endif
  template <typename Printable, typename... Printable2>
  static void print(Printable p, Printable2... p2)
  {
    print(p);
    print(p2...);
  }
  template <typename Printable, typename... Printable2>
  static void println(Printable p, Printable2... p2)
  {
    print(p);
    print(p2...);
    std::cout << std::endl;
  }

#endif // CHEVAN_UTILS_PRINT
#ifdef CHEVAN_UTILS_RANDOM
  static inline float randf()
  {
    return ((float)rand()) / RAND_MAX;
  }
#endif // CHEVAN_UTILS_RANDOM
#ifdef CHEVAN_UTILS_BYTE_TYPEDEF
  typedef int8_t i8;
  typedef int16_t i16;
  typedef int32_t i32;
  typedef int64_t i64;
  typedef uint8_t u8;
  typedef uint16_t u16;
  typedef uint32_t u32;
  typedef uint64_t u64;
  typedef float f32;
  typedef double f64;
#endif // CHEVAN_UTILS_BYTE_TYPEDEF
#ifdef CHEVAN_UTILS_MATH
#ifndef CHEVAN_UTILS_MATH_M4
  struct m4
  {
    // 0,1,2,3
    // 4,5,6,7
    // 8,9,0,1
    // 2,3,4,5
    union
    {
      float array[16] = {0.f};
      float elements[4][4];
    };
    m4() = default;
    m4(float diagonal)
    {
      array[0] = diagonal;
      array[5] = diagonal;
      array[10] = diagonal;
      array[15] = diagonal;
    }
    m4(float *array)
    {
      memcpy(this->array, array, sizeof(float) * 16);
    }
    m4(float **elements)
    {
      for (uint i = 0; i < 4; i++)
      {
        for (uint j = 0; j < 4; j++)
        {
          this->elements[i][j] = elements[i][j];
        }
      }
    }
    float &operator[](uint i)
    {
      return array[i];
    }
    float &get(uint i, uint j)
    {
      return elements[i][j];
    }
#define _chevanut_math_concat(a, b) a##b
#define _chevanut_math_operatorMacro(Op)          \
  template <typename T>                           \
  m4 operator Op(T a)                             \
  {                                               \
    m4 c = m4(array);                             \
    for (uint i = 0; i < 16; i++)                 \
    {                                             \
      c[i] _chevanut_math_concat(Op, =) a;        \
    }                                             \
    return c;                                     \
  }                                               \
  template <typename T>                           \
  void operator _chevanut_math_concat(Op, =)(T a) \
  {                                               \
    (*this) = (*this)Op a;                        \
  }
    _chevanut_math_operatorMacro(+);
    _chevanut_math_operatorMacro(-);
    _chevanut_math_operatorMacro(*);
    _chevanut_math_operatorMacro(/);
#undef _chevanut_math_operatorMacro
#undef _chevanut_math_concat
  };
#else
  typedef CHEVAN_UTILS_MATH_M4 m4;
#endif // !CHEVAN_UTILS_MATH_M4
  typedef CHEVAN_UTILS_MATH_V2 v2;
  typedef CHEVAN_UTILS_MATH_V3 v3;
  typedef CHEVAN_UTILS_MATH_V4 v4;

  static inline float V2Dot(v2 a, v2 b)
  {
    return a.x * b.x + a.y * b.y;
  }
  static inline float V2LengthSquared(v2 v)
  {
    return v.x * v.x + v.y * v.y;
  }
  static inline v2 V2Normalize(v2 v)
  {
    float length = sqrtf(V2LengthSquared(v));
    v.x /= length;
    v.y /= length;
    return v;
  }
  static inline v2 V2AddV2(v2 a, v2 b)
  {
    v2 v = v2(a.x + b.x, a.y + b.y);
    return v;
  }
  static inline v2 V2MinusV2(v2 a, v2 b)
  {
    v2 v = v2(a.x - b.x, a.y - b.y);
    return v;
  }
  static inline v2 V2MultiplyF32(v2 v, float f)
  {
    v.x *= f;
    v.y *= f;
    return v;
  }
  static inline v3 V3AddV3(v3 a, v3 b)
  {
    v3 c = v3(a.x + b.x, a.y + b.y, a.z + b.z);
    return c;
  }
  static inline v3 V3MinusV3(v3 a, v3 b)
  {
    v3 c = v3(a.x - b.x, a.y - b.y, a.z - b.z);
    return c;
  }
  static inline v3 V3MultiplyF32(v3 v, float f)
  {
    v.x *= f;
    v.y *= f;
    v.z *= f;
    return v;
  }
  static inline float V3LengthSquared(v3 a)
  {
    return a.x * a.x + a.y * a.y + a.z * a.z;
  }
  static inline float V3Length(v3 a)
  {
    return sqrtf(V3LengthSquared(a));
  }
  static inline v3 V3Normalize(v3 v)
  {
    float length = V3Length(v);
    v3 result = v3(
        v.x / length,
        v.y / length,
        v.z / length);
    return result;
  }
  static inline float V3Dot(v3 a, v3 b)
  {
    float dot =
        a.x * b.x +
        a.y * b.y +
        a.z * b.z;
    return dot;
  }
  static inline v3 V3Cross(v3 a, v3 b)
  {
    v3 result = v3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x);
    return result;
  }
  static inline float minInV3(v3 v)
  {
    float minimum = v.x;
    if (v.y < minimum)
    {
      minimum = v.y;
    }
    if (v.z < minimum)
    {
      minimum = v.z;
    }
    return minimum;
  }
  static inline float maxInV3(v3 v)
  {
    float maximum = v.x;
    if (v.y > maximum)
    {
      maximum = v.y;
    }
    if (v.z > maximum)
    {
      maximum = v.z;
    }
    return maximum;
  }
  static inline bool V4RectHasPoint(v4 v, v2 p)
  {
    return (p.x >= v.x && p.x <= v.x + v.z &&
            p.y >= v.y && p.y <= v.y + v.w);
  }
  static inline float V4Dot(v4 a, v4 b)
  {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
  }
  static inline v4 V4AddV4(v4 a, v4 b)
  {
    v4 c = v4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
    return c;
  }
  static inline v4 V4MinusV4(v4 a, v4 b)
  {
    v4 c = v4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
    return c;
  }
  static inline v4 V4MultiplyF32(v4 a, float f)
  {
    v4 c = v4(a.x * f, a.y * f, a.z * f, a.w * f);
    return c;
  }
  static inline v4 V4MultiplyV4(v4 a, v4 b)
  {
    v4 c = v4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
    return c;
  }

  static m4 M4InitDiagonal(float diagonal)
  {
    return m4(diagonal); // compatibility's sake
  }
  static inline m4 M4MultiplyM4(m4 a, m4 b)
  {
    m4 c;
    for (uint i = 0; i < 4; i++)
    {
      for (uint j = 0; j < 4; j++)
      {
        c[i + j * 4] = (a[0 + j * 4] * b[i + 0 * 4] +
                        a[1 + j * 4] * b[i + 1 * 4] +
                        a[2 + j * 4] * b[i + 2 * 4] +
                        a[3 + j * 4] * b[i + 3 * 4]);
      }
    }
    return c;
  }
  static inline m4 M4MultiplyF32(m4 a, float b)
  {
    return a * b;
  }
  static inline v4 V4MultiplyM4(v4 v, m4 m)
  {
    v4 result = {0, 0, 0, 0};
    for (int32_t i = 0; i < 4; ++i)
    {
      float elements[] = vec4ToArray(v);
      ((float *)&result)[i] = (elements[0] * m.elements[0][i] +
                               elements[1] * m.elements[1][i] +
                               elements[2] * m.elements[2][i] +
                               elements[3] * m.elements[3][i]);
    }
    return result;
  }
  static inline m4 M4TranslateV3(v3 translation)
  {
    m4 result = M4InitDiagonal(1.f);
    result.elements[3][0] = translation.x;
    result.elements[3][1] = translation.y;
    result.elements[3][2] = translation.z;
    return result;
  }
  static inline m4 M4ScaleV3(v3 scale)
  {
    m4 result = M4InitDiagonal(1.f);
    result.elements[0][0] = scale.x;
    result.elements[1][1] = scale.y;
    result.elements[2][2] = scale.z;
    return result;
  }
  static inline m4 M4Perspective(float fov, float aspect_ratio, float near_z, float far_z)
  {
    m4 result;
    float tan_theta_over_2 = tanf(fov * (PIf / 360.f));
    result.elements[0][0] = 1.f / tan_theta_over_2;
    result.elements[1][1] = aspect_ratio / tan_theta_over_2;
    result.elements[2][3] = -1.f;
    result.elements[2][2] = (near_z + far_z) / (near_z - far_z);
    result.elements[3][2] = (2.f * near_z * far_z) / (near_z - far_z);
    result.elements[3][3] = 0.f;
    return result;
  }
  static inline m4 M4Orthographic(float left, float right, float bottom, float top, float near_depth, float far_depth)
  {
    m4 result;

    result.elements[0][0] = 2.f / (right - left);
    result.elements[1][1] = 2.f / (top - bottom);
    result.elements[2][2] = -2.f / (far_depth - near_depth);
    result.elements[3][3] = 1.f;
    result.elements[3][0] = (left + right) / (left - right);
    result.elements[3][1] = (bottom + top) / (bottom - top);
    result.elements[3][2] = (far_depth + near_depth) / (near_depth - far_depth);

    return result;
  }
  static inline m4 M4LookAt(v3 eye, v3 center, v3 up)
  {
    m4 result;

    v3 f = V3Normalize(V3MinusV3(center, eye));
    v3 s = V3Normalize(V3Cross(f, up));
    v3 u = V3Cross(s, f);

    result.elements[0][0] = s.x;
    result.elements[0][1] = u.x;
    result.elements[0][2] = -f.x;
    result.elements[0][3] = 0.f;

    result.elements[1][0] = s.y;
    result.elements[1][1] = u.y;
    result.elements[1][2] = -f.y;
    result.elements[1][3] = 0.f;

    result.elements[2][0] = s.z;
    result.elements[2][1] = u.z;
    result.elements[2][2] = -f.z;
    result.elements[2][3] = 0.f;

    result.elements[3][0] = -V3Dot(s, eye);
    result.elements[3][1] = -V3Dot(u, eye);
    result.elements[3][2] = V3Dot(f, eye);
    result.elements[3][3] = 1.f;

    return result;
  }
  static inline m4 M4Inverse(m4 m)
  {
    float coef00 = m.elements[2][2] * m.elements[3][3] - m.elements[3][2] * m.elements[2][3];
    float coef02 = m.elements[1][2] * m.elements[3][3] - m.elements[3][2] * m.elements[1][3];
    float coef03 = m.elements[1][2] * m.elements[2][3] - m.elements[2][2] * m.elements[1][3];
    float coef04 = m.elements[2][1] * m.elements[3][3] - m.elements[3][1] * m.elements[2][3];
    float coef06 = m.elements[1][1] * m.elements[3][3] - m.elements[3][1] * m.elements[1][3];
    float coef07 = m.elements[1][1] * m.elements[2][3] - m.elements[2][1] * m.elements[1][3];
    float coef08 = m.elements[2][1] * m.elements[3][2] - m.elements[3][1] * m.elements[2][2];
    float coef10 = m.elements[1][1] * m.elements[3][2] - m.elements[3][1] * m.elements[1][2];
    float coef11 = m.elements[1][1] * m.elements[2][2] - m.elements[2][1] * m.elements[1][2];
    float coef12 = m.elements[2][0] * m.elements[3][3] - m.elements[3][0] * m.elements[2][3];
    float coef14 = m.elements[1][0] * m.elements[3][3] - m.elements[3][0] * m.elements[1][3];
    float coef15 = m.elements[1][0] * m.elements[2][3] - m.elements[2][0] * m.elements[1][3];
    float coef16 = m.elements[2][0] * m.elements[3][2] - m.elements[3][0] * m.elements[2][2];
    float coef18 = m.elements[1][0] * m.elements[3][2] - m.elements[3][0] * m.elements[1][2];
    float coef19 = m.elements[1][0] * m.elements[2][2] - m.elements[2][0] * m.elements[1][2];
    float coef20 = m.elements[2][0] * m.elements[3][1] - m.elements[3][0] * m.elements[2][1];
    float coef22 = m.elements[1][0] * m.elements[3][1] - m.elements[3][0] * m.elements[1][1];
    float coef23 = m.elements[1][0] * m.elements[2][1] - m.elements[2][0] * m.elements[1][1];

    v4 fac0 = v4(coef00, coef00, coef02, coef03);
    v4 fac1 = v4(coef04, coef04, coef06, coef07);
    v4 fac2 = v4(coef08, coef08, coef10, coef11);
    v4 fac3 = v4(coef12, coef12, coef14, coef15);
    v4 fac4 = v4(coef16, coef16, coef18, coef19);
    v4 fac5 = v4(coef20, coef20, coef22, coef23);

    v4 vec0 = v4(m.elements[1][0], m.elements[0][0], m.elements[0][0], m.elements[0][0]);
    v4 vec1 = v4(m.elements[1][1], m.elements[0][1], m.elements[0][1], m.elements[0][1]);
    v4 vec2 = v4(m.elements[1][2], m.elements[0][2], m.elements[0][2], m.elements[0][2]);
    v4 vec3 = v4(m.elements[1][3], m.elements[0][3], m.elements[0][3], m.elements[0][3]);

    v4 inv0 = V4AddV4(V4MinusV4(V4MultiplyV4(vec1, fac0), V4MultiplyV4(vec2, fac1)), V4MultiplyV4(vec3, fac2));
    v4 inv1 = V4AddV4(V4MinusV4(V4MultiplyV4(vec0, fac0), V4MultiplyV4(vec2, fac3)), V4MultiplyV4(vec3, fac4));
    v4 inv2 = V4AddV4(V4MinusV4(V4MultiplyV4(vec0, fac1), V4MultiplyV4(vec1, fac3)), V4MultiplyV4(vec3, fac5));
    v4 inv3 = V4AddV4(V4MinusV4(V4MultiplyV4(vec0, fac2), V4MultiplyV4(vec1, fac4)), V4MultiplyV4(vec2, fac5));

    v4 sign_a = v4(+1, -1, +1, -1);
    v4 sign_b = v4(-1, +1, -1, +1);

    m4 inverse;
    for (uint32_t i = 0; i < 4; ++i)
    {
      float inv0Arr[] = vec4ToArray(inv0);
      float inv1Arr[] = vec4ToArray(inv1);
      float inv2Arr[] = vec4ToArray(inv2);
      float inv3Arr[] = vec4ToArray(inv3);
      float sign_a_arr[] = vec4ToArray(sign_a);
      float sign_b_arr[] = vec4ToArray(sign_b);
      inverse.elements[0][i] = inv0Arr[i] * sign_a_arr[i];
      inverse.elements[1][i] = inv1Arr[i] * sign_b_arr[i];
      inverse.elements[2][i] = inv2Arr[i] * sign_a_arr[i];
      inverse.elements[3][i] = inv3Arr[i] * sign_b_arr[i];
    }

    v4 row0 = v4(inverse.elements[0][0], inverse.elements[1][0], inverse.elements[2][0], inverse.elements[3][0]);
    v4 m0 = v4(m.elements[0][0], m.elements[0][1], m.elements[0][2], m.elements[0][3]);
    v4 dot0 = V4MultiplyV4(m0, row0);
    float dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);

    float one_over_det = 1 / dot1;

    return M4MultiplyF32(inverse, one_over_det);
  }
  static inline m4 M4RemoveRotation(m4 mat)
  {
    v3 scale = v3(
        V3Length(v3(mat.elements[0][0], mat.elements[0][1], mat.elements[0][2])),
        V3Length(v3(mat.elements[1][0], mat.elements[1][1], mat.elements[1][2])),
        V3Length(v3(mat.elements[2][0], mat.elements[2][1], mat.elements[2][2])));

    mat.elements[0][0] = scale.x;
    mat.elements[1][0] = 0.f;
    mat.elements[2][0] = 0.f;

    mat.elements[0][1] = 0.f;
    mat.elements[1][1] = scale.y;
    mat.elements[2][1] = 0.f;

    mat.elements[0][2] = 0.f;
    mat.elements[1][2] = 0.f;
    mat.elements[2][2] = scale.z;

    return mat;
  }
  static inline m4 M4Rotate(float angle, v3 axis)
  {
    m4 result = M4InitDiagonal(1.f);

    axis = V3Normalize(axis);

    float sin_theta = sinf(angle);
    float cos_theta = cosf(angle);
    float cos_value = 1.f - cos_theta;

    result.elements[0][0] = (axis.x * axis.x * cos_value) + cos_theta;
    result.elements[0][1] = (axis.x * axis.y * cos_value) + (axis.z * sin_theta);
    result.elements[0][2] = (axis.x * axis.z * cos_value) - (axis.y * sin_theta);

    result.elements[1][0] = (axis.y * axis.x * cos_value) - (axis.z * sin_theta);
    result.elements[1][1] = (axis.y * axis.y * cos_value) + cos_theta;
    result.elements[1][2] = (axis.y * axis.z * cos_value) + (axis.x * sin_theta);

    result.elements[2][0] = (axis.z * axis.x * cos_value) + (axis.y * sin_theta);
    result.elements[2][1] = (axis.z * axis.y * cos_value) - (axis.x * sin_theta);
    result.elements[2][2] = (axis.z * axis.z * cos_value) + cos_theta;

    return result;
  }
  static inline v3 calculateTriangleNormal(v3 p1, v3 p2, v3 p3)
  {
    v3 p1_to_p2 = v3(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
    v3 p1_to_p3 = v3(p3.x - p1.x, p3.y - p1.y, p3.z - p1.z);
    v3 normal = V3Cross(p1_to_p3, p1_to_p2);
    normal.y *= -1;
    return normal;
  }
  static inline v3 calculateTriangleNormalNormalized(v3 p1, v3 p2, v3 p3)
  {
    v3 normal = calculateTriangleNormal(p1, p2, p3);
    normal = V3Normalize(normal);
    return normal;
  }
  static inline m4 M4Mapper(v3 min, v3 max, v3 targetMin, v3 targetMax)
  {
    // float scale = (n - min.x) / (max.x - min.x) * (targetMax.x - targetMin.x) + targetMin.x;

    m4 transform = M4InitDiagonal(1.f);
    transform = M4MultiplyM4(transform, M4TranslateV3(V3MultiplyF32(targetMin, 1.f)));
    transform = M4MultiplyM4(transform, M4ScaleV3(v3(targetMax.x - targetMin.x, targetMax.y - targetMin.y, targetMax.z - targetMin.z)));
    transform = M4MultiplyM4(transform, M4ScaleV3(v3(1.f / (max.x - min.x), 1.f / (max.y - min.y), 1.f / (max.z - min.z))));
    transform = M4MultiplyM4(transform, M4TranslateV3(V3MultiplyF32(min, -1.f)));

    return transform;
  }
#endif // CHEVAN_UTILS_MATH
#ifdef CHEVAN_UTILS_COLOR
  static inline v3 RGBToHSV(v3 rgb)
  {
    float c_max = maxInV3(rgb);
    float c_min = minInV3(rgb);
    float delta = c_max - c_min;
    bool c_max_is_r = rgb.r > rgb.g && rgb.r > rgb.b;
    bool c_max_is_g = rgb.g > rgb.r && rgb.g > rgb.b;
    bool c_max_is_b = rgb.b > rgb.r && rgb.b > rgb.g;
    float h = (c_max_is_r ? (rgb.g - rgb.b) / delta + 0 : c_max_is_g ? (rgb.b - rgb.r) / delta + 2
                                                      : c_max_is_b   ? (rgb.r - rgb.g) / delta + 4
                                                                     : 0);
    float s = c_max == 0 ? 0 : (delta / c_max);
    float v = c_max;
    v3 hsv = v3(h / 6.f, s, v);
    return hsv;
  }
  static inline v3 HSVToRGB(v3 hsv)
  {
    float h = fmodf(hsv.x * 360.f, 360.f);
    float s = hsv.y;
    float v = hsv.z;

    float c = v * s;
    float x = c * (1 - fabsf(fmodf((h / 60.f), 2) - 1));
    float m = v - c;

    float r;
    float g;
    float b;

    if ((h >= 0.f && h < 60.f) || (h >= 360.f && h < 420.f))
    {
      r = c;
      g = x;
      b = 0;
    }
    else if (h >= 60.f && h < 120.f)
    {
      r = x;
      g = c;
      b = 0;
    }
    else if (h >= 120.f && h < 180.f)
    {
      r = 0;
      g = c;
      b = x;
    }
    else if (h >= 180.f && h < 240.f)
    {
      r = 0;
      g = x;
      b = c;
    }
    else if (h >= 240.f && h < 300.f)
    {
      r = x;
      g = 0;
      b = c;
    }
    else if ((h >= 300.f && h <= 360.f) ||
             (h >= -60.f && h <= 0.f))
    {
      r = c;
      g = 0;
      b = x;
    }
    else
    {
      r = 0;
      g = 0;
      b = 0;
    }

    v3 rgb = v3(r + m, g + m, b + m);
    return rgb;
  }
#endif // CHEVAN_UTILS_COLOR
#ifdef CHEVAN_UTILS_PERLIN_NOISE
  static int32_t global_perlin_noise_seed = 0;
  static int32_t global_perlin_noise_hash[] = {
      208, 34, 231, 213, 32, 248, 233, 56, 161, 78, 24, 140, 71, 48, 140, 254, 245, 255, 247, 247, 40,
      185, 248, 251, 245, 28, 124, 204, 204, 76, 36, 1, 107, 28, 234, 163, 202, 224, 245, 128, 167, 204,
      9, 92, 217, 54, 239, 174, 173, 102, 193, 189, 190, 121, 100, 108, 167, 44, 43, 77, 180, 204, 8, 81,
      70, 223, 11, 38, 24, 254, 210, 210, 177, 32, 81, 195, 243, 125, 8, 169, 112, 32, 97, 53, 195, 13,
      203, 9, 47, 104, 125, 117, 114, 124, 165, 203, 181, 235, 193, 206, 70, 180, 174, 0, 167, 181, 41,
      164, 30, 116, 127, 198, 245, 146, 87, 224, 149, 206, 57, 4, 192, 210, 65, 210, 129, 240, 178, 105,
      228, 108, 245, 148, 140, 40, 35, 195, 38, 58, 65, 207, 215, 253, 65, 85, 208, 76, 62, 3, 237, 55, 89,
      232, 50, 217, 64, 244, 157, 199, 121, 252, 90, 17, 212, 203, 149, 152, 140, 187, 234, 177, 73, 174,
      193, 100, 192, 143, 97, 53, 145, 135, 19, 103, 13, 90, 135, 151, 199, 91, 239, 247, 33, 39, 145,
      101, 120, 99, 3, 186, 86, 99, 41, 237, 203, 111, 79, 220, 135, 158, 42, 30, 154, 120, 67, 87, 167,
      135, 176, 183, 191, 253, 115, 184, 21, 233, 58, 129, 233, 142, 39, 128, 211, 118, 137, 139, 255,
      114, 20, 218, 113, 154, 27, 127, 246, 250, 1, 8, 198, 250, 209, 92, 222, 173, 21, 88, 102, 219};

  static inline int32_t PerlinNoise2(int32_t x, int32_t y)
  {
    int32_t tmp = global_perlin_noise_hash[(y + global_perlin_noise_seed) % 256];
    return global_perlin_noise_hash[(tmp + x) % 256];
  }
  static inline float PerlinLinearlyInterpolate(float x, float y, float s)
  {
    return x + s * (y - x);
  }
  static inline float PerlinSmoothlyInterpolate(float x, float y, float s)
  {
    return PerlinLinearlyInterpolate(x, y, s * s * (3 - 2 * s));
  }
  static inline float PerlinNoise2D(float x, float y)
  {
    int32_t x_int = (int32_t)x;
    int32_t y_int = (int32_t)y;
    float x_frac = x - x_int;
    float y_frac = y - y_int;
    int32_t s = PerlinNoise2(x_int, y_int);
    int32_t t = PerlinNoise2(x_int + 1, y_int);
    int32_t u = PerlinNoise2(x_int, y_int + 1);
    int32_t v = PerlinNoise2(x_int + 1, y_int + 1);
    float low = PerlinSmoothlyInterpolate((float)s, (float)t, x_frac);
    float high = PerlinSmoothlyInterpolate((float)u, (float)v, x_frac);
    return PerlinSmoothlyInterpolate(low, high, y_frac);
  }
  static inline float Perlin2D(float x, float y, float freq, int32_t depth)
  {
    float xa = x * freq;
    float ya = y * freq;
    float amp = 1.0;
    float fin = 0;
    float div = 0.0;

    for (int32_t i = 0; i < depth; i++)
    {
      div += 256 * amp;
      fin += PerlinNoise2D(xa, ya) * amp;
      amp /= 2;
      xa *= 2;
      ya *= 2;
    }

    return fin / div;
  }
#endif // CHEVAN_UTILS_PERLIN_NOISE
#ifdef CHEVAN_UTILS_FASTCALC
  static inline float regularizeDegree(float angle)
  {
    float degree = fmodf(angle, 360);
    if (degree < 0)
      degree += 360;
    return degree;
  }
  static inline float regularizeRad(float angle)
  {
    float rad = fmodf(angle, 2 * PIf);
    if (rad < 0)
      rad += 2 * PIf;
    return rad;
  }

  static inline float fast_cos(float angle)
  {
    float t_angle = regularizeRad(angle) - PIf;
    float res = 0;
    float x = 1;
    int32_t precision = CHEVAN_UTILS_FASTCALC_PRECISION;
    int32_t factorialPrecision = 20;
    unsigned long long factorials[] = {0,
                                       1,
                                       2,
                                       6,
                                       24,
                                       120,
                                       720,
                                       5040,
                                       40320,
                                       362880,
                                       3628800,
                                       39916800,
                                       479001600,
                                       6227020800,
                                       87178291200,
                                       1307674368000,
                                       20922789888000,
                                       355687428096000,
                                       6402373705728000,
                                       121645100408832000};
    for (int32_t i = 0; i <= precision && i < factorialPrecision; i += 4)
    {
      res += x / factorials[i];
      x *= t_angle * t_angle;
      res -= x / factorials[i + 2];
      x *= t_angle * t_angle;
    }
    return -res;
  }
  static inline float fast_sin(float angle)
  {
    float t_angle = regularizeRad(angle) - PIf;
    float res = 0;
    float x = t_angle;
    int32_t precision = CHEVAN_UTILS_FASTCALC_PRECISION;
    int32_t factorialPrecision = 21;
    unsigned long long factorials[] = {0,
                                       1,
                                       2,
                                       6,
                                       24,
                                       120,
                                       720,
                                       5040,
                                       40320,
                                       362880,
                                       3628800,
                                       39916800,
                                       479001600,
                                       6227020800,
                                       87178291200,
                                       1307674368000,
                                       20922789888000,
                                       355687428096000,
                                       6402373705728000,
                                       121645100408832000,
                                       2432902008176640000};
    for (int32_t i = 1; i <= precision && i < factorialPrecision; i += 4)
    {
      res += x / factorials[i];
      x *= t_angle * t_angle;
      res -= x / factorials[i + 2];
      x *= t_angle * t_angle;
    }
    return -res;
  }
  static inline float fast_atan(float a)
  {
    if ((a > -1.5 && a < -0.7) || (a < 1.4 && a >= 0.6))
    {
      return atanf(a);
    }
    if (a > -1 && a < 1)
    {
      float res = 0;
      float x = a;
      int32_t precision = CHEVAN_UTILS_FASTCALC_PRECISION;
      for (int32_t i = 1; i <= precision; i += 2)
      {
        res += x / (4 * i - 3);
        x *= a * a;
        res -= x / (4 * i - 1);
        x *= a * a;
      }
      return res;
    }
    else
    {
      float res = 0;
      float x = -a;
      int32_t precision = 11;
      for (int32_t i = 1; i < precision * 2; i += 2)
      {
        res += 1.f / ((4 * i - 3) * x);
        x *= a * a;
        res -= 1.f / ((4 * i - 1) * x);
        x *= a * a;
      }
      if (x < 0)
      {
        res += PIf / 2;
      }
      else
      {
        res -= PIf / 2;
      }
      return res;
    }
  }

#endif // CHEVAN_UTILS_FASTCALC
#ifdef CHEVAN_UTILS_BASE64
  namespace base64
  {
    static std::string base64_encode(std::string const &s, bool url);
    static std::string base64_encode_pem(std::string const &s);
    static std::string base64_encode_mime(std::string const &s);
    static std::string base64_decode(std::string const &s, bool remove_linebreaks);
    static std::string base64_encode(unsigned char const *, size_t len, bool url);

    namespace
    {
      //
      // Depending on the url parameter in base64_chars, one of
      // two sets of base64 characters needs to be chosen.
      // They differ in their last two characters.
      //
      static const char *base64_chars[2] = {
          "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
          "abcdefghijklmnopqrstuvwxyz"
          "0123456789"
          "+/",

          "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
          "abcdefghijklmnopqrstuvwxyz"
          "0123456789"
          "-_"};

      static unsigned int pos_of_char(const unsigned char chr)
      {
        //
        // Return the position of chr within base64_encode()
        //

        if (chr >= 'A' && chr <= 'Z')
          return chr - 'A';
        else if (chr >= 'a' && chr <= 'z')
          return chr - 'a' + ('Z' - 'A') + 1;
        else if (chr >= '0' && chr <= '9')
          return chr - '0' + ('Z' - 'A') + ('z' - 'a') + 2;
        else if (chr == '+' || chr == '-')
          return 62; // Be liberal with input and accept both url ('-') and non-url ('+') base 64 characters (
        else if (chr == '/' || chr == '_')
          return 63; // Ditto for '/' and '_'
        else
          //
          // 2020-10-23: Throw std::exception rather than const char*
          //(Pablo Martin-Gomez, https://github.com/Bouska)
          //
          throw std::runtime_error("Input is not valid base64-encoded data.");
      }

      static std::string insert_linebreaks(std::string str, size_t distance)
      {
        //
        // Provided by https://github.com/JomaCorpFX, adapted by me.
        //
        if (!str.length())
        {
          return "";
        }

        size_t pos = distance;

        while (pos < str.size())
        {
          str.insert(pos, "\n");
          pos += distance + 1;
        }

        return str;
      }

      template <typename String, unsigned int line_length>
      static std::string encode_with_line_breaks(String s)
      {
        return insert_linebreaks(base64_encode(s, false), line_length);
      }

      template <typename String>
      static std::string encode_pem(String s)
      {
        return encode_with_line_breaks<String, 64>(s);
      }

      template <typename String>
      static std::string encode_mime(String s)
      {
        return encode_with_line_breaks<String, 76>(s);
      }

    };

    template <typename String>
    static std::string encode(String s, bool url = false)
    {
      return base64_encode(reinterpret_cast<const unsigned char *>(s.data()), s.length(), url);
    }
    static std::string base64_encode(unsigned char const *bytes_to_encode, size_t in_len, bool url = false)
    {

      size_t len_encoded = (in_len + 2) / 3 * 4;

      unsigned char trailing_char = url ? '.' : '=';

      //
      // Choose set of base64 characters. They differ
      // for the last two positions, depending on the url
      // parameter.
      // A bool (as is the parameter url) is guaranteed
      // to evaluate to either 0 or 1 in C++ therefore,
      // the correct character set is chosen by subscripting
      // base64_chars with url.
      //
      const char *base64_chars_ = base64_chars[url];

      std::string ret;
      ret.reserve(len_encoded);

      unsigned int pos = 0;

      while (pos < in_len)
      {
        ret.push_back(base64_chars_[(bytes_to_encode[pos + 0] & 0xfc) >> 2]);

        if (pos + 1 < in_len)
        {
          ret.push_back(base64_chars_[((bytes_to_encode[pos + 0] & 0x03) << 4) + ((bytes_to_encode[pos + 1] & 0xf0) >> 4)]);

          if (pos + 2 < in_len)
          {
            ret.push_back(base64_chars_[((bytes_to_encode[pos + 1] & 0x0f) << 2) + ((bytes_to_encode[pos + 2] & 0xc0) >> 6)]);
            ret.push_back(base64_chars_[bytes_to_encode[pos + 2] & 0x3f]);
          }
          else
          {
            ret.push_back(base64_chars_[(bytes_to_encode[pos + 1] & 0x0f) << 2]);
            ret.push_back(trailing_char);
          }
        }
        else
        {

          ret.push_back(base64_chars_[(bytes_to_encode[pos + 0] & 0x03) << 4]);
          ret.push_back(trailing_char);
          ret.push_back(trailing_char);
        }

        pos += 3;
      }

      return ret;
    }
    template <typename String>
    static std::string decode(String encoded_string, bool remove_linebreaks = false)
    {
      //
      // decode(…) is templated so that it can be used with String = const std::string&
      // or std::string_view (requires at least C++17)
      //

      if (encoded_string.empty())
        return std::string();

      if (remove_linebreaks)
      {

        std::string copy(encoded_string);

        copy.erase(std::remove(copy.begin(), copy.end(), '\n'), copy.end());

        return base64_decode(copy, false);
      }

      size_t length_of_string = encoded_string.length();
      size_t pos = 0;

      //
      // The approximate length (bytes) of the decoded string might be one or
      // two bytes smaller, depending on the amount of trailing equal signs
      // in the encoded string. This approximation is needed to reserve
      // enough space in the string to be returned.
      //
      size_t approx_length_of_decoded_string = length_of_string / 4 * 3;
      std::string ret;
      ret.reserve(approx_length_of_decoded_string);

      while (pos < length_of_string)
      {
        //
        // Iterate over encoded input string in chunks. The size of all
        // chunks except the last one is 4 bytes.
        //
        // The last chunk might be padded with equal signs or dots
        // in order to make it 4 bytes in size as well, but this
        // is not required as per RFC 2045.
        //
        // All chunks except the last one produce three output bytes.
        //
        // The last chunk produces at least one and up to three bytes.
        //

        size_t pos_of_char_1 = pos_of_char(encoded_string[pos + 1]);

        //
        // Emit the first output byte that is produced in each chunk:
        //
        ret.push_back(static_cast<std::string::value_type>(((pos_of_char(encoded_string[pos + 0])) << 2) + ((pos_of_char_1 & 0x30) >> 4)));

        if ((pos + 2 < length_of_string) && // Check for data that is not padded with equal signs (which is allowed by RFC 2045)
            encoded_string[pos + 2] != '=' &&
            encoded_string[pos + 2] != '.' // accept URL-safe base 64 strings, too, so check for '.' also.
        )
        {
          //
          // Emit a chunk's second byte (which might not be produced in the last chunk).
          //
          unsigned int pos_of_char_2 = pos_of_char(encoded_string[pos + 2]);
          ret.push_back(static_cast<std::string::value_type>(((pos_of_char_1 & 0x0f) << 4) + ((pos_of_char_2 & 0x3c) >> 2)));

          if ((pos + 3 < length_of_string) &&
              encoded_string[pos + 3] != '=' &&
              encoded_string[pos + 3] != '.')
          {
            //
            // Emit a chunk's third byte (which might not be produced in the last chunk).
            //
            ret.push_back(static_cast<std::string::value_type>(((pos_of_char_2 & 0x03) << 6) + pos_of_char(encoded_string[pos + 3])));
          }
        }

        pos += 4;
      }

      return ret;
    }
    static std::string base64_decode(std::string const &s, bool remove_linebreaks = false)
    {
      return decode(s, remove_linebreaks);
    }
    static std::string base64_encode(std::string const &s, bool url = false)
    {
      return encode(s, url);
    }
    static std::string base64_encode_pem(std::string const &s)
    {
      return encode_pem(s);
    }
    static std::string base64_encode_mime(std::string const &s)
    {
      return encode_mime(s);
    }
#if __cplusplus >= 201703L
    static std::string base64_encode(std::string_view s, bool url);
    static std::string base64_encode_pem(std::string_view s);
    static std::string base64_encode_mime(std::string_view s);
    static std::string base64_decode(std::string_view s, bool remove_linebreaks);

    static std::string base64_encode(std::string_view s, bool url = false)
    {
      return encode(s, url);
    }
    static std::string base64_encode_pem(std::string_view s)
    {
      return encode_pem(s);
    }
    static std::string base64_encode_mime(std::string_view s)
    {
      return encode_mime(s);
    }
    static std::string base64_decode(std::string_view s, bool remove_linebreaks = false)
    {
      return decode(s, remove_linebreaks);
    }

#endif // __cplusplus >= 201703L
  }
#endif // CHEVAN_UTILS_BASE64

};

#endif
