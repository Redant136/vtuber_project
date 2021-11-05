#pragma once
#ifndef CHEVAN_UTILS_H
#define CHEVAN_UTILS_H 1
#define CHEVAN_UTILS_VERSION "2.2.1"
#define CHEVAN_UTILS_INLINE inline

#define PIf 3.1415926535897f
#ifndef CHEVAN_UTILS_FASTCALC_PRECISION
#define CHEVAN_UTILS_FASTCALC_PRECISION 9
#endif

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
#define maxInVec2(v) ((v.x > v.y) ? v.x : v.y)
#define maxInVec3(v) ((maxInVec2(v) > v.z) ? maxInVec2(v) : v.z)
#define maxInVec4(v) ((maxInVec3(v) > v.w) ? maxInVec3(v) : v.w)
#define minInVec2(v) ((v.x < v.y) ? v.x : v.y)
#define minInVec3(v) ((minInVec2(v) < v.z) ? minInVec2(v) : v.z)
#define minInVec4(v) ((minInVec3(v) < v.w) ? minInVec3(v) : v.w)

#define membuild(type, name, data) \
  type name;                       \
  memcpy(&name, data, sizeof(type));
#define sizeofArr(arr) (sizeof(arr) / sizeof(arr[0]))

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

#define EMPTY()
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

#define _parseEnumCheck(dst, src, valLoc, val) \
  if (std::string(src) == #val)                \
  {                                            \
    dst = valLoc val;                          \
  }

#define parseEnum2(dst, src, valLoc, firstEnum, ...) \
  _parseEnumCheck(dst, src, valLoc, firstEnum)       \
      IF_ELSE(HAS_ARGS(__VA_ARGS__))(                \
          DEFER2(_parseEnum2)()(dst, src, valLoc, __VA_ARGS__))()
#define _parseEnum2() parseEnum2

#define parseEnum(dst, src, valLoc, ...) \
  EVAL(parseEnum2(dst, src, valLoc, __VA_ARGS__))

#endif // CHEVAN_UTILS_MACRO_MAGIC

#ifdef __cplusplus
#include <iostream>
#include <string>
#include <stdint.h>
#include <string.h>
#include <vector>
#include <cmath>
#include <algorithm>

#ifndef CHEVAN_UTILS_NO_EXTRA_INCLUDES
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <stdexcept>
#include <functional>
#endif

namespace chevan_utils
{
  typedef unsigned char uchar;
  typedef unsigned short ushort;
  typedef unsigned int uint;
  typedef unsigned long ulong;
  typedef unsigned long long ullong;
  typedef long long llong;
  enum class Cardinal8dir
  {
    CENTER,
    NORTH,
    SOUTH,
    EAST,
    WEST,
    NORTH_EAST,
    NORTH_WEST,
    SOUTH_EAST,
    SOUTH_WEST
  };

  template <typename T, typename L = size_t>
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
    template <typename I = size_t>
    T &operator[](I i)
    {
      assert(arr);
      assert(i < length + 1);
      return arr[i];
    }
    template <typename I = size_t>
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
  static std::string toLowerCase(std::string &s)
  {
    std::string r = s;
    for (uint i = 0; i < s.length(); i++)
    {
      r[i] = std::tolower(s[i]);
    }
    return r;
  }
  static std::string toUpperCase(std::string &s)
  {
    std::string r = s;
    for (uint i = 0; i < s.length(); i++)
    {
      r[i] = std::toupper(s[i]);
    }
    return r;
  }

  namespace chevanut_byte_typedef
  {
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
  }

  namespace chevanut_vec
  {
    template <typename T>
    struct Vec2;
    template <typename T>
    struct Vec3;
    template <typename T>
    struct Vec4;

#define _chevanut_apply_all_types(F) \
  F(float);                          \
  F(double);                         \
  F(char);                           \
  F(uchar);                          \
  F(short);                          \
  F(ushort);                         \
  F(int);                            \
  F(uint);                           \
  F(long);                           \
  F(ulong);                          \
  F(ullong);

    template <typename T>
    struct Vec2
    {
      T x, y;
      constexpr Vec2() = default;
#define _chevanut_v2_init(type) \
  Vec2(type x, type y)          \
  {                             \
    this->x = x;                \
    this->y = y;                \
  }                             \
  Vec2(type a)                  \
  {                             \
    this->x = a;                \
    this->y = 0;                \
  }

      _chevanut_apply_all_types(_chevanut_v2_init);
#undef _chevan_v2_init

      template <typename Vec>
      Vec2(const Vec &base)
      {
        this->x = base.x;
        this->y = base.y;
      }

      Vec2<T> operator+(const Vec2<T> a) { return Vec2<T>(x + a.x, y + a.y); }
      Vec2<T> operator-(const Vec2<T> a) { return Vec2<T>(x - a.x, y - a.y); }
      void operator+=(const Vec2<T> a)
      {
        x += a.x;
        y += a.y;
      }
      void operator-=(const Vec2<T> a)
      {
        x -= a.x;
        y -= a.y;
      }
#define _chevanut_v2_op(T)                                       \
  Vec2<T> operator+(const T a) { return Vec2<T>(x + a, y + a); } \
  Vec2<T> operator-(const T a) { return Vec2<T>(x - a, y - a); } \
  Vec2<T> operator*(const T a) { return Vec2<T>(x * a, y * a); } \
  Vec2<T> operator/(const T a) { return Vec2<T>(x / a, y / a); }
      _chevanut_apply_all_types(_chevanut_v2_op);
#undef _chevanut_v2_op
      template <typename I>
      void operator+=(const I a)
      {
        (*this) = (*this) + a;
      }
      template <typename I>
      void operator-=(const I a)
      {
        (*this) = (*this) - a;
      }
      template <typename I>
      void operator*=(const I a)
      {
        (*this) = (*this) * a;
      }
      template <typename I>
      void operator/=(const I a)
      {
        (*this) = (*this) / a;
      }
    };
    template <typename T>
    struct Vec3
    {
      union
      {
        T x;
        T r;
      };
      union
      {
        T y;
        T g;
      };
      union
      {
        T z;
        T b;
      };
      constexpr Vec3() = default;
#define _chevanut_v3_init(type) \
  Vec3(type x, type y, type z)  \
  {                             \
    this->x = x;                \
    this->y = y;                \
    this->z = z;                \
  }                             \
  Vec3(type a)                  \
  {                             \
    this->x = a;                \
  }                             \
  Vec3(type a, type b)          \
  {                             \
    this->x = a;                \
    this->y = b;                \
  }
      _chevanut_apply_all_types(_chevanut_v3_init);

#undef _chevan_v3_init

      template <typename Vec>
      Vec3(const Vec &base, T z)
      {
        this->x = base.x;
        this->y = base.y;
        this->z = z;
      }
      template <typename Vec>
      Vec3(const Vec &base)
      {
        this->x = base.x;
        this->y = base.y;
        this->z = base.z;
      }

      Vec3<T> operator+(const Vec3<T> a) { return Vec3<T>(x + a.x, y + a.y, z + a.z); }
      Vec3<T> operator-(const Vec3<T> a) { return Vec3<T>(x - a.x, y - a.y, z - a.z); }
      void operator+=(const Vec3<T> a)
      {
        x += a.x;
        y += a.y;
        z += a.z;
      }
      void operator-=(const Vec3<T> a)
      {
        x -= a.x;
        y -= a.y;
        z -= a.z;
      }
#define _chevanut_v3_op(T)                                              \
  Vec3<T> operator+(const T a) { return Vec3<T>(x + a, y + a, z + a); } \
  Vec3<T> operator-(const T a) { return Vec3<T>(x - a, y - a, z - a); } \
  Vec3<T> operator*(const T a) { return Vec3<T>(x * a, y * a, z * a); } \
  Vec3<T> operator/(const T a) { return Vec3<T>(x / a, y / a, z / a); }
      _chevanut_apply_all_types(_chevanut_v3_op);
#undef _chevanut_v3_op

      void operator+=(const T a)
      {
        x += a;
        y += a;
        z += a;
      }
      void operator-=(const T a)
      {
        x -= a;
        y -= a;
        z -= a;
      }
      void operator*=(const T a)
      {
        x *= a;
        y *= a;
        z *= a;
      }
      void operator/=(const T a)
      {
        x /= a;
        y /= a;
        z /= a;
      }
    };
    template <typename T>
    struct Vec4
    {
      union
      {
        T x;
        T r;
      };
      union
      {
        T y;
        T g;
      };
      union
      {
        T z;
        T width;
        T b;
      };
      union
      {
        T w;
        T height;
        T a;
      };
      constexpr Vec4() = default;
#define _chevanut_v4_init(type)        \
  Vec4(type x, type y, type z, type w) \
  {                                    \
    this->x = x;                       \
    this->y = y;                       \
    this->z = z;                       \
    this->w = w;                       \
  }                                    \
  Vec4(type a)                         \
  {                                    \
    this->x = a;                       \
  }                                    \
  Vec4(type a, type b)                 \
  {                                    \
    this->x = a;                       \
    this->y = b;                       \
  }                                    \
  Vec4(type a, type b, type c)         \
  {                                    \
    this->x = a;                       \
    this->y = b;                       \
    this->z = c;                       \
  }
      _chevanut_v4_init(float);
      _chevanut_v4_init(double);
      _chevanut_v4_init(char);
      _chevanut_v4_init(uchar);
      _chevanut_v4_init(short);
      _chevanut_v4_init(ushort);
      _chevanut_v4_init(int);
      _chevanut_v4_init(uint);
      _chevanut_v4_init(long);
      _chevanut_v4_init(ulong);
      _chevanut_v4_init(ullong);

#undef _chevan_v4_init

      template <typename Vec>
      Vec4(const Vec &base, T z, T w)
      {
        this->x = base.x;
        this->y = base.y;
        this->z = z;
        this->w = w;
      }
      template <typename Vec>
      Vec4(const Vec &base, T w)
      {
        this->x = base.x;
        this->y = base.y;
        this->z = base.z;
        this->w = w;
      }
      template <typename Vec>
      Vec4(const Vec &base)
      {
        this->x = base.x;
        this->y = base.y;
        this->z = base.z;
        this->w = w;
      }

      Vec4<T> operator+(const Vec4<T> a) { return Vec4<T>(x + a.x, y + a.y, z + a.z, w + a.w); }
      Vec4<T> operator-(const Vec4<T> a) { return Vec4<T>(x - a.x, y - a.y, z - a.z, w - a.w); }
      void operator+=(const Vec4<T> a)
      {
        x += a.x;
        y += a.y;
        z += a.z;
        w += a.w;
      }
      void operator-=(const Vec4<T> a)
      {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        w -= a.w;
      }
#define _chevanut_v4_op(T)                                                     \
  Vec4<T> operator+(const T a) { return Vec4<T>(x + a, y + a, z + a, w + a); } \
  Vec4<T> operator-(const T a) { return Vec4<T>(x - a, y - a, z - a, w - a); } \
  Vec4<T> operator*(const T a) { return Vec4<T>(x * a, y * a, z * a, w * a); } \
  Vec4<T> operator/(const T a) { return Vec4<T>(x / a, y / a, z / a, w / a); }
      _chevanut_apply_all_types(_chevanut_v4_op);
#undef _chevanut_v4_op
      void operator+=(const T a)
      {
        x += a;
        y += a;
        z += a;
        z += a;
      }
      void operator-=(const T a)
      {
        x -= a;
        y -= a;
        z -= a;
        z -= a;
      }
      void operator*=(const T a)
      {
        x *= a;
        y *= a;
        z *= a;
        z *= a;
      }
      void operator/=(const T a)
      {
        x /= a;
        y /= a;
        z /= a;
        z /= a;
      }
    };

#ifdef chevanut_staticVec
    struct vec2
    {
      float x, y;
      vec2 operator+(const vec2 a) { return {x + a.x, y + a.y}; }
      vec2 operator-(const vec2 a) { return {x - a.x, y - a.y}; }
      void operator+=(const vec2 a)
      {
        x += a.x;
        y += a.y;
      }
      void operator-=(const vec2 a)
      {
        x -= a.x;
        y -= a.y;
      }
      vec2 operator+(const float a) { return {x + a, y + a}; }
      vec2 operator-(const float a) { return {x - a, y - a}; }
      vec2 operator*(const float a) { return {x * a, y * a}; }
      vec2 operator/(const float a) { return {x / a, y / a}; }
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
    struct vec3
    {
      union
      {
        float x, r;
      };
      union
      {
        float y, g;
      };
      union
      {
        float z, b;
      };
      vec3 operator+(const vec3 &a) { return {x + a.x, y + a.y, z + a.z}; }
      vec3 operator-(const vec3 &a) { return {x - a.x, y - a.y, z - a.z}; }
      void operator+=(const vec3 &a)
      {
        x += a.x;
        y += a.y;
        z += a.z;
      }
      void operator-=(const vec3 &a)
      {
        x -= a.x;
        y -= a.y;
        z -= a.z;
      }
      vec3 operator+(const float &a) { return {x + a, y + a, z + a}; }
      vec3 operator-(const float &a) { return {x - a, y - a, z - a}; }
      vec3 operator*(const float &a) { return {x * a, y * a, z * a}; }
      vec3 operator/(const float &a) { return {x / a, y / a, z / a}; }
      void operator+=(const float &a)
      {
        x += a;
        y += a;
        z += a;
      }
      void operator-=(const float &a)
      {
        x -= a;
        y -= a;
        z -= a;
      }
      void operator*=(const float &a)
      {
        x *= a;
        y *= a;
        z *= a;
      }
      void operator/=(const float &a)
      {
        x /= a;
        y /= a;
        z /= a;
      }
    };
    struct vec4
    {
      union
      {
        float x;
        float r;
      };
      union
      {
        float y;
        float g;
      };
      union
      {
        float z;
        float width;
        float b;
      };
      union
      {
        float w;
        float height;
        float a;
      };
      vec4 operator+(const vec4 &a) { return {x + a.x, y + a.y, z + a.z, w + a.w}; }
      vec4 operator-(const vec4 &a) { return {x - a.x, y - a.y, z - a.z, w - a.w}; }
      void operator+=(const vec4 &a)
      {
        x += a.x;
        y += a.y;
        z += a.z;
        w += a.w;
      }
      void operator-=(const vec4 &a)
      {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        w -= a.w;
      }
      vec4 operator+(const float &a) { return {x + a, y + a, z + a, w + a}; }
      vec4 operator-(const float &a) { return {x - a, y - a, z - a, w - a}; }
      vec4 operator*(const float &a) { return {x * a, y * a, z * a, w * a}; }
      vec4 operator/(const float &a) { return {x / a, y / a, z / a, w / a}; }
      void operator+=(const float &a)
      {
        x += a;
        y += a;
        z += a;
        z += a;
      }
      void operator-=(const float &a)
      {
        x -= a;
        y -= a;
        z -= a;
        z -= a;
      }
      void operator*=(const float &a)
      {
        x *= a;
        y *= a;
        z *= a;
        z *= a;
      }
      void operator/=(const float &a)
      {
        x /= a;
        y /= a;
        z /= a;
        z /= a;
      }
    };

    struct ivec2
    {
      int32_t x, y;
      ivec2 operator+(const ivec2 &a) { return {x + a.x, y + a.y}; }
      ivec2 operator-(const ivec2 &a) { return {x - a.x, y - a.y}; }
      void operator+=(const ivec2 &a)
      {
        x += a.x;
        y += a.y;
      }
      void operator-=(const ivec2 &a)
      {
        x -= a.x;
        y -= a.y;
      }
      ivec2 operator+(const int32_t &a) { return {x + a, y + a}; }
      ivec2 operator-(const int32_t &a) { return {x - a, y - a}; }
      ivec2 operator*(const int32_t &a) { return {x * a, y * a}; }
      ivec2 operator/(const int32_t &a) { return {x / a, y / a}; }
      void operator+=(const int32_t &a)
      {
        x += a;
        y += a;
      }
      void operator-=(const int32_t &a)
      {
        x -= a;
        y -= a;
      }
      void operator*=(const int32_t &a)
      {
        x *= a;
        y *= a;
      }
      void operator/=(const int32_t &a)
      {
        x /= a;
        y /= a;
      }
    };
    struct ivec3
    {
      union
      {
        int32_t x, r;
      };
      union
      {
        int32_t y, g;
      };
      union
      {
        int32_t z, b;
      };
      ivec3 operator+(const ivec3 &a) { return {x + a.x, y + a.y, z + a.z}; }
      ivec3 operator-(const ivec3 &a) { return {x - a.x, y - a.y, z - a.z}; }
      void operator+=(const ivec3 &a)
      {
        x += a.x;
        y += a.y;
        z += a.z;
      }
      void operator-=(const ivec3 &a)
      {
        x -= a.x;
        y -= a.y;
        z -= a.z;
      }
      ivec3 operator+(const int32_t &a) { return {x + a, y + a, z + a}; }
      ivec3 operator-(const int32_t &a) { return {x - a, y - a, z - a}; }
      ivec3 operator*(const int32_t &a) { return {x * a, y * a, z * a}; }
      ivec3 operator/(const int32_t &a) { return {x / a, y / a, z / a}; }
      void operator+=(const int32_t &a)
      {
        x += a;
        y += a;
        z += a;
      }
      void operator-=(const int32_t &a)
      {
        x -= a;
        y -= a;
        z -= a;
      }
      void operator*=(const int32_t &a)
      {
        x *= a;
        y *= a;
        z *= a;
      }
      void operator/=(const int32_t &a)
      {
        x /= a;
        y /= a;
        z /= a;
      }
    };
    struct ivec4
    {
      union
      {
        int32_t x;
        int32_t r;
      };
      union
      {
        int32_t y;
        int32_t g;
      };
      union
      {
        int32_t z;
        int32_t width;
        int32_t b;
      };
      union
      {
        int32_t w;
        int32_t height;
        int32_t a;
      };
      ivec4 operator+(const ivec4 &a) { return {x + a.x, y + a.y, z + a.z, w + a.w}; }
      ivec4 operator-(const ivec4 &a) { return {x - a.x, y - a.y, z - a.z, w - a.w}; }
      void operator+=(const ivec4 &a)
      {
        x += a.x;
        y += a.y;
        z += a.z;
        w += a.w;
      }
      void operator-=(const ivec4 &a)
      {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        w -= a.w;
      }
      ivec4 operator+(const int32_t &a) { return {x + a, y + a, z + a, w + a}; }
      ivec4 operator-(const int32_t &a) { return {x - a, y - a, z - a, w - a}; }
      ivec4 operator*(const int32_t &a) { return {x * a, y * a, z * a, w * a}; }
      ivec4 operator/(const int32_t &a) { return {x / a, y / a, z / a, w / a}; }
      void operator+=(const int32_t &a)
      {
        x += a;
        y += a;
        z += a;
        z += a;
      }
      void operator-=(const int32_t &a)
      {
        x -= a;
        y -= a;
        z -= a;
        z -= a;
      }
      void operator*=(const int32_t &a)
      {
        x *= a;
        y *= a;
        z *= a;
        z *= a;
      }
      void operator/=(const int32_t &a)
      {
        x /= a;
        y /= a;
        z /= a;
        z /= a;
      }
    };

#else
    typedef Vec2<float> vec2;
    typedef Vec3<float> vec3;
    typedef Vec4<float> vec4;
    typedef Vec2<int32_t> ivec2;
    typedef Vec3<int32_t> ivec3;
    typedef Vec4<int32_t> ivec4;

#endif

#ifndef CHEVAN_UTILS_VEC2
#define CHEVAN_UTILS_VEC2 chevan_utils::chevanut_vec::vec2
#endif // !CHEVAN_UTILS_VEC2
#ifndef CHEVAN_UTILS_VEC3
#define CHEVAN_UTILS_VEC3 chevan_utils::chevanut_vec::vec3
#endif // !CHEVAN_UTILS_VEC3
#ifndef CHEVAN_UTILS_VEC4
#define CHEVAN_UTILS_VEC4 chevan_utils::chevanut_vec::vec4
#endif // !CHEVAN_UTILS_VEC4
  }

  namespace chevanut_print
  {
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
    static void printSep()
    {
      std::cout << "---------------------------------" << std::endl;
    }
    static void print(uchar *p)
    {
      std::cout << (void *)p;
    }
    static void print(uchar p)
    {
      print((int)p);
    }
    template <typename L>
    static void print(Array<char, L> arr, L length = 256)
    {
      L l = arr.length < length ? arr.length : length;
      std::cout << std::string(arr.arr, arr.arr + l);
    }
    template <typename L>
    static void print(Array<uchar, L> arr, L length = 256)
    {
      L l = arr.length < length ? arr.length : length;
      std::cout << std::string(arr.arr, arr.arr + l);
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
    template <typename T>
    static void print(std::vector<T> vec)
    {
      for (auto t : vec)
      {
        std::cout << t;
      }
    }
#ifdef CHEVAN_UTILS_VEC2
    static void print(CHEVAN_UTILS_VEC2 v)
    {
      print("{", v.x, ", ", v.y, "}");
    }
#endif
#ifdef CHEVAN_UTILS_VEC3
    static void print(CHEVAN_UTILS_VEC3 v)
    {
      print("{", v.x, ", ", v.y, ", ", v.z, "}");
    }
#endif
#ifdef CHEVAN_UTILS_VEC4
    static void print(CHEVAN_UTILS_VEC4 v)
    {
      print("{", v.x, ", ", v.y, ", ", v.z, ", ", v.w, "}");
    }
#endif
    static void print(Cardinal8dir dir)
    {
      switch (dir)
      {
      case Cardinal8dir::CENTER:
        std::cout << "CENTER" << std::endl;
        break;
      case Cardinal8dir::NORTH:
        std::cout << "NORTH" << std::endl;
        break;
      case Cardinal8dir::SOUTH:
        std::cout << "SOUTH" << std::endl;
        break;
      case Cardinal8dir::EAST:
        std::cout << "EAST" << std::endl;
        break;
      case Cardinal8dir::WEST:
        std::cout << "WEST" << std::endl;
        break;
      case Cardinal8dir::NORTH_EAST:
        std::cout << "NORTH_EAST" << std::endl;
        break;
      case Cardinal8dir::NORTH_WEST:
        std::cout << "NORTH_WEST" << std::endl;
        break;
      case Cardinal8dir::SOUTH_EAST:
        std::cout << "SOUTH_EAST" << std::endl;
        break;
      case Cardinal8dir::SOUTH_WEST:
        std::cout << "SOUTH_WEST" << std::endl;
        break;
      default:
        break;
      }
    }
    template <typename Printable>
    static void print(Printable p)
    {
      std::cout << p;
    }
    template <typename Printable>
    static void println(Printable p)
    {
      print(p);
      std::cout << std::endl;
    }
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

#define ch_print(x) chevan_utils::chevanut_print::print(x)
#define ch_println(x) chevan_utils::chevanut_print::println(x)
  }

  namespace chevanut_math
  {
#define v2 CHEVAN_UTILS_VEC2
#define v3 CHEVAN_UTILS_VEC3
#define v4 CHEVAN_UTILS_VEC4
    static CHEVAN_UTILS_INLINE float dot(v2 a, v2 b)
    {
      return a.x * b.x + a.y * b.y;
    }
    static CHEVAN_UTILS_INLINE float dot(v3 a, v3 b)
    {
      return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    static CHEVAN_UTILS_INLINE float dot(v4 a, v4 b)
    {
      return a.x * b.x + a.y * b.y + a.z * b.z + a.w * a.w;
    }

    static CHEVAN_UTILS_INLINE v3 cross(v3 a, v3 b)
    {
      return v3(
          a.y * b.z - a.z * b.y,
          a.z * b.x - a.x * b.z,
          a.x * b.y - a.y * b.x);
    }

    static CHEVAN_UTILS_INLINE float lengthSquared(v2 v)
    {
      return v.x * v.x + v.y * v.y;
    }
    static CHEVAN_UTILS_INLINE float lengthSquared(v3 v)
    {
      return v.x * v.x + v.y * v.y + v.z * v.z;
    }
    static CHEVAN_UTILS_INLINE float lengthSquared(v4 v)
    {
      return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
    }

    static CHEVAN_UTILS_INLINE float vecLength(v2 v)
    {
      return sqrtf(lengthSquared(v));
    }
    static CHEVAN_UTILS_INLINE float vecLength(v3 v)
    {
      return sqrtf(lengthSquared(v));
    }
    static CHEVAN_UTILS_INLINE float vecLength(v4 v)
    {
      return sqrtf(lengthSquared(v));
    }

    static CHEVAN_UTILS_INLINE v2 normalize(v2 v)
    {
      float length = sqrtf(lengthSquared(v));
      v.x /= length;
      v.y /= length;
      return v;
    }
    static CHEVAN_UTILS_INLINE v3 normalize(v3 v)
    {
      float length = sqrtf(lengthSquared(v));
      v.x /= length;
      v.y /= length;
      v.z /= length;
      return v;
    }
    static CHEVAN_UTILS_INLINE v4 normalize(v4 v)
    {
      float length = sqrtf(lengthSquared(v));
      v.x /= length;
      v.y /= length;
      v.z /= length;
      v.w /= length;
      return v;
    }

    static CHEVAN_UTILS_INLINE float vecMin(v2 a)
    {
      float min = a.x;
      if (a.y < min)
      {
        min = a.y;
      }
      return min;
    }
    static CHEVAN_UTILS_INLINE float vecMin(v3 a)
    {
      float min = a.x;
      if (a.y < min)
      {
        min = a.y;
      }
      if (a.z < min)
      {
        min = a.z;
      }
      return min;
    }
    static CHEVAN_UTILS_INLINE float vecMin(v4 a)
    {
      float min = a.x;
      if (a.y < min)
      {
        min = a.y;
      }
      if (a.z < min)
      {
        min = a.z;
      }
      if (a.w < min)
      {
        min = a.w;
      }
      return min;
    }

    static CHEVAN_UTILS_INLINE float vecMax(v2 a)
    {
      float max = a.x;
      if (a.y > max)
      {
        max = a.y;
      }
      return max;
    }
    static CHEVAN_UTILS_INLINE float vecMax(v3 a)
    {
      float max = a.x;
      if (a.y > max)
      {
        max = a.y;
      }
      if (a.z < max)
      {
        max = a.z;
      }
      return max;
    }
    static CHEVAN_UTILS_INLINE float vecMax(v4 a)
    {
      float max = a.x;
      if (a.y < max)
      {
        max = a.y;
      }
      if (a.z < max)
      {
        max = a.z;
      }
      if (a.w < max)
      {
        max = a.w;
      }
      return max;
    }

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
      static m4 translation(v3 translate)
      {
        m4 result = m4(1.f);
        result.elements[3][0] = translate.x;
        result.elements[3][1] = translate.y;
        result.elements[3][2] = translate.z;
        return result;
      }
      static m4 rotation(float angle, v3 axis)
      {
        m4 result = m4(1.f);

        axis = normalize(axis);

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
      static m4 scale(v3 scale)
      {
        m4 result = m4(1.f);
        result.elements[0][0] = scale.x;
        result.elements[1][1] = scale.y;
        result.elements[2][2] = scale.z;
        return result;
      }
      static m4 perspective(float fov, float aspect_ratio, float near_z, float far_z)
      {
        m4 result;
        float tan_theta_over_2 = tanf(fov * (M_PI / 360.f));
        result.elements[0][0] = 1.f / tan_theta_over_2;
        result.elements[1][1] = aspect_ratio / tan_theta_over_2;
        result.elements[2][3] = -1.f;
        result.elements[2][2] = (near_z + far_z) / (near_z - far_z);
        result.elements[3][2] = (2.f * near_z * far_z) / (near_z - far_z);
        result.elements[3][3] = 0.f;
        return result;
      }
      static m4 orthographic(float left, float right, float bottom, float top, float near_depth, float far_depth)
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
      static m4 lookAt(v3 eye, v3 center, v3 up)
      {
        m4 result;
        v3 f = normalize(center - eye);
        v3 s = normalize(cross(f, up));
        v3 u = cross(s, f);

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

        result.elements[3][0] = -dot(s, eye);
        result.elements[3][1] = -dot(u, eye);
        result.elements[3][2] = dot(f, eye);
        result.elements[3][3] = 1.f;

        return result;
      }
      static m4 map(v3 min, v3 max, v3 targetMin, v3 targetMax)
      {
        m4 transform = m4(1.f);
        transform = transform * translation(targetMin);
        transform = transform * scale(targetMax - targetMin);
        transform = transform * scale(v3(1.f / (max.x - min.x), 1.f / (max.y - min.y), 1.f / (max.z - min.z)));
        transform = transform * translation(min * -1.f);
        return transform;
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
#define _chevanut_math_operatorMacro_t(type, Op)     \
  m4 operator Op(type a)                             \
  {                                                  \
    m4 c = m4(array);                                \
    for (uint i = 0; i < 16; i++)                    \
    {                                                \
      c[i] _chevanut_math_concat(Op, =) a;           \
    }                                                \
    return c;                                        \
  }                                                  \
  void operator _chevanut_math_concat(Op, =)(type a) \
  {                                                  \
    (*this) = (*this)Op a;                           \
  }

#define _chevanut_math_operatorMacro(Op)      \
  _chevanut_math_operatorMacro_t(float, Op);  \
  _chevanut_math_operatorMacro_t(double, Op); \
  _chevanut_math_operatorMacro_t(char, Op);   \
  _chevanut_math_operatorMacro_t(uchar, Op);  \
  _chevanut_math_operatorMacro_t(short, Op);  \
  _chevanut_math_operatorMacro_t(ushort, Op); \
  _chevanut_math_operatorMacro_t(int, Op);    \
  _chevanut_math_operatorMacro_t(uint, Op);   \
  _chevanut_math_operatorMacro_t(long, Op);   \
  _chevanut_math_operatorMacro_t(ulong, Op);  \
  _chevanut_math_operatorMacro_t(ullong, Op);

      _chevanut_math_operatorMacro(+);
      _chevanut_math_operatorMacro(-);
      _chevanut_math_operatorMacro(*);
      _chevanut_math_operatorMacro(/);

#undef _chevanut_math_operatorMacro
#undef _chevanut_math_operatorMacro_t
#undef _chevanut_math_concat

      m4 operator*(m4 mat)
      {
        m4 m;
        for (uint i = 0; i < 4; i++)
        {
          for (uint j = 0; j < 4; j++)
          {
            m[i + j * 4] = (array[0 + j * 4] * mat[i + 0 * 4] +
                            array[1 + j * 4] * mat[i + 1 * 4] +
                            array[2 + j * 4] * mat[i + 2 * 4] +
                            array[3 + j * 4] * mat[i + 3 * 4]);
          }
        }
        return m;
      }
      void operator*=(m4 mat)
      {
        (*this) = (*this) * mat;
      }
      v4 operator*(v4 v)
      {
        v4 result;
        for (uint i = 0; i < 4; ++i)
        {
          ((float *)&result)[i] = (v.x * elements[0][i] +
                                   v.y * elements[1][i] +
                                   v.z * elements[2][i] +
                                   v.w * elements[3][i]);
        }

        return result;
      }
    };
#define CHEVAN_UTILS_MATH_M4 m4
#else
    typedef CHEVAN_UTILS_MATH_M4 m4;
#endif // !CHEVAN_UTILS_MATH_M4
    static void translate(m4 &mat, v3 translate)
    {
      mat.elements[3][0] = mat.elements[0][0] * translate.x + mat.elements[1][0] * translate.y + mat.elements[2][0] * translate.z + mat.elements[3][0];
      mat.elements[3][1] = mat.elements[0][1] * translate.x + mat.elements[1][1] * translate.y + mat.elements[2][1] * translate.z + mat.elements[3][1];
      mat.elements[3][2] = mat.elements[0][2] * translate.x + mat.elements[1][2] * translate.y + mat.elements[2][2] * translate.z + mat.elements[3][2];
      mat.elements[3][3] = mat.elements[0][3] * translate.x + mat.elements[1][3] * translate.y + mat.elements[2][3] * translate.z + mat.elements[3][3];
    }
    static void rotate(m4 &mat, float angle, v3 axis)
    {
      axis = normalize(axis);
      float sin_theta = sinf(angle);
      float cos_theta = cosf(angle);
      float cos_value = 1.f - cos_theta;

      float rows[3][3] =
          {
              {(axis.x * axis.x * cos_value) + cos_theta, (axis.x * axis.y * cos_value) + (axis.z * sin_theta), (axis.x * axis.z * cos_value) - (axis.y * sin_theta)},
              {(axis.y * axis.x * cos_value) - (axis.z * sin_theta), (axis.y * axis.y * cos_value) + cos_theta, (axis.y * axis.z * cos_value) + (axis.x * sin_theta)},
              {(axis.z * axis.x * cos_value) + (axis.y * sin_theta), (axis.z * axis.y * cos_value) - (axis.x * sin_theta), (axis.z * axis.z * cos_value) + cos_theta}};

      for (uint i = 0; i < 4; i++)
      {
        for (uint j = 0; j < 3; j++)
        {
          mat.elements[j][i] = mat.elements[0][i] * rows[j][0] + mat.elements[1][i] * rows[j][1] + mat.elements[2][i] * rows[j][2];
        }
      }
    }
    // NOTE(ANT) complete matrix math here

#if 0
  static m4 scale1(v3 scale)
  {
    m4 result = m4(1.f);
    result.elements[0][0] = scale.x;
    result.elements[1][1] = scale.y;
    result.elements[2][2] = scale.z;
    return result;
  }
  static m4 perspective1(float fov, float aspect_ratio, float near_z, float far_z)
  {
    m4 result;
    float tan_theta_over_2 = tanf(fov * (M_PI / 360.f));
    result.elements[0][0] = 1.f / tan_theta_over_2;
    result.elements[1][1] = aspect_ratio / tan_theta_over_2;
    result.elements[2][3] = -1.f;
    result.elements[2][2] = (near_z + far_z) / (near_z - far_z);
    result.elements[3][2] = (2.f * near_z * far_z) / (near_z - far_z);
    result.elements[3][3] = 0.f;
    return result;
  }
  static m4 orthographic1(float left, float right, float bottom, float top, float near_depth, float far_depth)
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
  static m4 lookAt1(v3 eye, v3 center, v3 up)
  {
    m4 result;
    v3 f = normalize(center - eye);
    v3 s = normalize(cross(f, up));
    v3 u = cross(s, f);

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

    result.elements[3][0] = -dot(s, eye);
    result.elements[3][1] = -dot(u, eye);
    result.elements[3][2] = dot(f, eye);
    result.elements[3][3] = 1.f;

    return result;
  }
  static m4 map1(v3 min, v3 max, v3 targetMin, v3 targetMax)
  {
    m4 transform = m4(1.f);
    // transform = transform * translation(targetMin);
    // transform = transform * scale(targetMax - targetMin);
    // transform = transform * scale(v3(1.f / (max.x - min.x), 1.f / (max.y - min.y), 1.f / (max.z - min.z)));
    // transform = transform * translation(min * -1.f);
    return transform;
  }
#endif

#undef v2
#undef v3
#undef v4
  }

  namespace chevanut_perlin_noise
  {
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

    static CHEVAN_UTILS_INLINE int32_t PerlinNoise2(int32_t x, int32_t y) { return global_perlin_noise_hash[(global_perlin_noise_hash[(y + global_perlin_noise_seed) % 256] + x) % 256]; }
    static CHEVAN_UTILS_INLINE float PerlinLinearlyInterpolate(float x, float y, float s) { return x + s * (y - x); }
    static CHEVAN_UTILS_INLINE float PerlinSmoothlyInterpolate(float x, float y, float s) { return PerlinLinearlyInterpolate(x, y, s * s * (3 - 2 * s)); }
    static CHEVAN_UTILS_INLINE float PerlinNoise2D(float x, float y)
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
    static CHEVAN_UTILS_INLINE float Perlin2D(float x, float y, float freq, int32_t depth)
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
  }

  namespace chevanut_base64
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

}

#else
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>
#include <ctype.h>

#ifndef CHEVAN_UTILS_NO_EXTRA_INCLUDES
#include <stdarg.h>
#endif
typedef unsigned char bool;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ullong;
typedef long long llong;
enum Cardinal8dir
{
  CENTER,
  NORTH,
  SOUTH,
  EAST,
  WEST,
  NORTH_EAST,
  NORTH_WEST,
  SOUTH_EAST,
  SOUTH_WEST
};
struct Array
{
  size_t length;
  void *arr;
};
static char *toLowerCase(const char *src)
{
  uint l = 0;
  char tmp[128];
  const char *c = src;
  while (*c != '\0')
  {
    if (l < 128)
      tmp[l] = tolower(*src);
    l++;
    c++;
  }
  char *res = malloc(l);
  memcpy(res, tmp, l);
  if (l < 128)
    return res;
  c = src + 128;
  l = 128;
  while (*c != '\0')
  {
    res[l] = tolower(*c);
    l++;
    c++;
  }
  return res;
}
static char *toUpperCase(const char *src)
{
  uint l = 0;
  char tmp[128];
  const char *c = src;
  while (*c != '\0')
  {
    if (l < 128)
      tmp[l] = toupper(*src);
    l++;
    c++;
  }
  char *res = malloc(l);
  memcpy(res, tmp, l);
  if (l < 128)
    return res;
  c = src + 128;
  l = 128;
  while (*c != '\0')
  {
    res[l] = toupper(*c);
    l++;
    c++;
  }
  return res;
}
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
#endif
struct vec2
{
  float x;
  float y;
};
struct vec3
{
  union
  {
    float x, r;
  };
  union
  {
    float y, g;
  };
  union
  {
    float z, b;
  };
};
struct vec4
{
  union
  {
    float x, r;
  };
  union
  {
    float y, g;
  };
  union
  {
    float z, width, b;
  };
  union
  {
    float w, height, a;
  };
};
static CHEVAN_UTILS_INLINE struct vec2 vec2_init(float x, float y)
{
  struct vec2 v = {x, y};
  return v;
}
static CHEVAN_UTILS_INLINE struct vec3 vec3_init(float x, float y, float z)
{
  struct vec3 v = {x, y, z};
  return v;
}
static CHEVAN_UTILS_INLINE struct vec4 vec4_init(float x, float y, float z, float w)
{
  struct vec4 v = {x, y, z, w};
  return v;
}
#define vec2(x, y) vec2_init(x, y)
#define vec3(x, y, z) vec3_init(x, y, z)
#define vec4(x, y, z, w) vec4_init(x, y, z, w)

struct ivec2
{
  int32_t x;
  int32_t y;
};
struct ivec3
{
  union
  {
    int32_t x, r;
  };
  union
  {
    int32_t y, g;
  };
  union
  {
    int32_t z, b;
  };
};
struct ivec4
{
  union
  {
    int32_t x, r;
  };
  union
  {
    int32_t y, g;
  };
  union
  {
    int32_t z, width, b;
  };
  union
  {
    int32_t w, height, a;
  };
};
static CHEVAN_UTILS_INLINE struct ivec2 ivec2_init(int32_t x, int32_t y)
{
  struct ivec2 v = {x, y};
  return v;
}
static CHEVAN_UTILS_INLINE struct ivec3 ivec3_init(int32_t x, int32_t y, int32_t z)
{
  struct ivec3 v = {x, y, z};
  return v;
}
static CHEVAN_UTILS_INLINE struct ivec4 ivec4_init(int32_t x, int32_t y, int32_t z, int32_t w)
{
  struct ivec4 v = {x, y, z, w};
  return v;
}
#define ivec2(x, y) ivec2_init(x, y)
#define ivec3(x, y, z) ivec3_init(x, y, z)
#define ivec4(x, y, z, w) ivec4_init(x, y, z, w)

#ifndef CHEVAN_UTILS_VEC2
#define CHEVAN_UTILS_VEC2 struct vec2
#endif // !CHEVAN_UTILS_VEC2
#ifndef CHEVAN_UTILS_VEC3
#define CHEVAN_UTILS_VEC3 struct vec3
#endif // !CHEVAN_UTILS_VEC3
#ifndef CHEVAN_UTILS_VEC4
#define CHEVAN_UTILS_VEC4 struct vec4
#endif // !CHEVAN_UTILS_VEC2

#define to_vec2(v) vec2((v).x, (v).y)
#define to_vec3(v) vec3((v).x, (v).y, (v).y)
#define to_vec4(v) vec4((v).x, (v).y, (v).y, (v).z)
#define to_ivec2(v) ivec2((v).x, (v).y)
#define to_ivec3(v) ivec3((v).x, (v).y, (v).y)
#define to_ivec4(v) ivec4((v).x, (v).y, (v).y, (v).z)

#define _chevanut_concat(a, b) a##b
#define _chevanut_vec_op_macro(name, op)                                                               \
  static CHEVAN_UTILS_INLINE struct vec2 vec2_##name(struct vec2 a, struct vec2 b)                     \
  {                                                                                                    \
    return vec2(a.x op b.x, a.y op b.y);                                                               \
  }                                                                                                    \
  static CHEVAN_UTILS_INLINE struct vec2 _chevanut_concat(vec2_##name, _float)(struct vec2 a, float b) \
  {                                                                                                    \
    return vec2(a.x op b, a.y op b);                                                                   \
  }                                                                                                    \
  static CHEVAN_UTILS_INLINE struct vec3 vec3_##name(struct vec3 a, struct vec3 b)                     \
  {                                                                                                    \
    return vec3(a.x op b.x, a.y op b.y, a.z op b.z);                                                   \
  }                                                                                                    \
  static CHEVAN_UTILS_INLINE struct vec3 _chevanut_concat(vec3_##name, _float)(struct vec3 a, float b) \
  {                                                                                                    \
    return vec3(a.x op b, a.y op b, a.z op b);                                                         \
  }                                                                                                    \
  static CHEVAN_UTILS_INLINE struct vec4 vec4_##name(struct vec4 a, struct vec4 b)                     \
  {                                                                                                    \
    return vec4(a.x op b.x, a.y op b.y, a.z op b.z, a.w op b.w);                                       \
  }                                                                                                    \
  static CHEVAN_UTILS_INLINE struct vec4 _chevanut_concat(vec4_##name, _float)(struct vec4 a, float b) \
  {                                                                                                    \
    return vec4(a.x op b, a.y op b, a.z op b, a.w op b);                                               \
  }                                                                                                    \
  static CHEVAN_UTILS_INLINE struct ivec2 ivec2_##name(struct ivec2 a, struct ivec2 b)                 \
  {                                                                                                    \
    return ivec2(a.x op b.x, a.y op b.y);                                                              \
  }                                                                                                    \
  static CHEVAN_UTILS_INLINE struct ivec3 ivec3_##name(struct ivec3 a, struct ivec3 b)                 \
  {                                                                                                    \
    return ivec3(a.x op b.x, a.y op b.y, a.z op b.z);                                                  \
  }                                                                                                    \
  static CHEVAN_UTILS_INLINE struct ivec4 ivec4_##name(struct ivec4 a, struct ivec4 b)                 \
  {                                                                                                    \
    return ivec4(a.x op b.x, a.y op b.y, a.z op b.z, a.w op b.w);                                      \
  }
_chevanut_vec_op_macro(add, +);
_chevanut_vec_op_macro(sub, -);
_chevanut_vec_op_macro(mult, *);
_chevanut_vec_op_macro(div, /);
#undef _chevanut_vec_op_macro
#undef _chevanut_concat
#define printVec2(v) printf("{%f, %f}\n", v.x, v.y)
#define printVec3(v) printf("{%f, %f, %f}\n", v.x, v.y, v.z)
#define printVec4(v) printf("{%f, %f, %f, %f}\n", v.x, v.y, v.z, v.w)

static void printMat4(float mat[16])
{
  for (uint i = 0; i < 4; i++)
  {
    printf("%f, %f, %f, %f\n", mat[0 + i * 4], mat[1 + i * 4], mat[2 + i * 4], mat[3 + i * 4]);
  }
}
static void printMem(void *p, ulong length)
{
  char *s = malloc(length + 1);
  for (uint i = 0; i < length; i++)
  {
    s[i] = ((uchar *)p)[i];
  }
  s[length] = 0;
  printf("%s", s);
  free(s);
}
static void printSep()
{
  printf("---------------------------------\n");
}

static void chevanut_print_str(const char *s) { printf("%s", s); }
static void chevanut_print_char(const char c) { printf("%c", c); }
static void chevanut_print_uchar(const uchar c) { printf("%c", c); }
#define _chevanut_print_macro(type)                                    \
  static void chevanut_print_##type(const type t) { printf("%d", t); } \
  static void chevanut_print_u##type(const type t) { printf("%d", t); }
_chevanut_print_macro(short);
_chevanut_print_macro(int);
_chevanut_print_macro(long);
_chevanut_print_macro(llong);
#undef _chevanut_print_macro
static void chevanut_print_float(const float f)
{
  printf("%f", f);
}
static void chevanut_print_double(const double f) { printf("%f", f); }
static void chevanut_print_ptr(const void *p) { printf("%p", p); }
static void chevanut_print_8dir(enum Cardinal8dir dir)
{
  switch (dir)
  {
  case CENTER:
    printf("CENTER\n");
    break;
  case NORTH:
    printf("NORTH\n");
    break;
  case SOUTH:
    printf("SOUTH\n");
    break;
  case EAST:
    printf("EAST\n");
    break;
  case WEST:
    printf("WEST\n");
    break;
  case NORTH_EAST:
    printf("NORTH_EAST\n");
    break;
  case NORTH_WEST:
    printf("NORTH_WEST\n");
    break;
  case SOUTH_EAST:
    printf("SOUTH_EAST\n");
    break;
  case SOUTH_WEST:
    printf("SOUTH_WEST\n");
    break;
  default:
    break;
  }
}
static void chevanut_print_vec2(CHEVAN_UTILS_VEC2 v) { printf("{%f, %f}", v.x, v.y); }
static void chevanut_print_vec3(CHEVAN_UTILS_VEC3 v) { printf("{%f, %f, %f}", v.x, v.y, v.z); }
static void chevanut_print_vec4(CHEVAN_UTILS_VEC4 v) { printf("{%f, %f, %f, %f}", v.x, v.y, v.z, v.w); }
static void chevanut_print_chevanutVec2(struct vec2 v) { printf("{%f, %f}", v.x, v.y); }
static void chevanut_print_chevanutVec3(struct vec3 v) { printf("{%f, %f, %f}", v.x, v.y, v.z); }
static void chevanut_print_chevanutVec4(struct vec4 v) { printf("{%f, %f, %f, %f}", v.x, v.y, v.z, v.w); }
static void chevanut_print_ivec2(struct ivec2 v) { printf("{%d, %d}", v.x, v.y); }
static void chevanut_print_ivec3(struct ivec3 v) { printf("{%d, %d, %d}", v.x, v.y, v.z); }
static void chevanut_print_ivec4(struct ivec4 v) { printf("{%d, %d, %d, %d}", v.x, v.y, v.z, v.w); }

#ifdef CHEVAN_UTILS_MACRO_MAGIC
#define _chevanut_println_recurse_MAP(x) ch_print(x);
#define chevanut_println_recurse(...) EVAL(MAP(_chevanut_println_recurse_MAP, __VA_ARGS__, "\n"))
#endif

#define ch_print(x) _Generic((x),                     \
                             char *                   \
                             : chevanut_print_str,    \
                               uchar                  \
                             : chevanut_print_uchar,  \
                               char                   \
                             : chevanut_print_char,   \
                               ushort                 \
                             : chevanut_print_ushort, \
                               short                  \
                             : chevanut_print_short,  \
                               uint                   \
                             : chevanut_print_uint,   \
                               int                    \
                             : chevanut_print_int,    \
                               ulong                  \
                             : chevanut_print_ulong,  \
                               long                   \
                             : chevanut_print_long,   \
                               ullong                 \
                             : chevanut_print_ullong, \
                               llong                  \
                             : chevanut_print_llong,  \
                               float                  \
                             : chevanut_print_float,  \
                               double                 \
                             : chevanut_print_double, \
                               void *                 \
                             : chevanut_print_ptr,    \
                               CHEVAN_UTILS_VEC2      \
                             : chevanut_print_vec2,   \
                               CHEVAN_UTILS_VEC3      \
                             : chevanut_print_vec3,   \
                               CHEVAN_UTILS_VEC4      \
                             : chevanut_print_vec4,   \
                               struct ivec2           \
                             : chevanut_print_ivec2,  \
                               struct ivec3           \
                             : chevanut_print_ivec3,  \
                               struct ivec4           \
                             : chevanut_print_ivec4,  \
                               default                \
                             : chevanut_print_ptr)(x)

#define ch_println(x) \
  ch_print(x);        \
  printf("\n")

#endif

// common to c and c++
#ifdef __cplusplus
namespace chevan_utils
{
#endif
  static CHEVAN_UTILS_INLINE float degreeToRad(float deg)
  {
    return deg / 180.f * M_PI;
  }
  static CHEVAN_UTILS_INLINE float radToDegree(float rad)
  {
    return rad / M_PI * 180;
  }
  static CHEVAN_UTILS_INLINE float randf() { return rand() / RAND_MAX; }
  static CHEVAN_UTILS_INLINE float regularizeDegree(float angle)
  {
    float degree = fmodf(angle, 360);
    if (degree < 0)
      degree += 360;
    return degree;
  }
  static CHEVAN_UTILS_INLINE float regularizeRad(float angle)
  {
    float rad = fmodf(angle, 2 * M_PI);
    if (rad < 0)
      rad += 2 * M_PI;
    return rad;
  }

  static CHEVAN_UTILS_INLINE float fast_cos(float angle)
  {
    float t_angle = regularizeRad(angle) - M_PI;
    float res = 0;
    float x = 1;
    int32_t precision = CHEVAN_UTILS_FASTCALC_PRECISION;
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
    for (int32_t i = 0; i <= precision && i < sizeofArr(factorials); i += 4)
    {
      res += x / factorials[i];
      x *= t_angle * t_angle;
      res -= x / factorials[i + 2];
      x *= t_angle * t_angle;
    }
    return -res;
  }
  static CHEVAN_UTILS_INLINE float fast_sin(float angle)
  {
    float t_angle = regularizeRad(angle) - M_PI;
    float res = 0;
    float x = t_angle;
    int32_t precision = CHEVAN_UTILS_FASTCALC_PRECISION;
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
    for (int32_t i = 1; i <= precision && i < sizeof(factorials); i += 4)
    {
      res += x / factorials[i];
      x *= t_angle * t_angle;
      res -= x / factorials[i + 2];
      x *= t_angle * t_angle;
    }
    return -res;
  }
  static CHEVAN_UTILS_INLINE float fast_atan(float a)
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
      int32_t precision = CHEVAN_UTILS_FASTCALC_PRECISION;
      for (int32_t i = 1; i < precision * 2; i += 2)
      {
        res += 1.f / ((4 * i - 3) * x);
        x *= a * a;
        res -= 1.f / ((4 * i - 1) * x);
        x *= a * a;
      }
      if (x < 0)
      {
        res += M_PI / 2;
      }
      else
      {
        res -= M_PI / 2;
      }
      return res;
    }
  }

  static CHEVAN_UTILS_VEC3 RGBToYUV(CHEVAN_UTILS_VEC3 rgb)
  {
    float Y = 0.299 * rgb.r + 0.587 * rgb.g + 0.114 * rgb.b;
    return (CHEVAN_UTILS_VEC3){Y, 0.492f * (rgb.b - Y), 0.877f * (rgb.r - Y)};
  }
  static CHEVAN_UTILS_VEC3 YUVToRGB(CHEVAN_UTILS_VEC3 yuv)
  {
    return (CHEVAN_UTILS_VEC3){yuv.x + 1.14 * yuv.z, yuv.x - 0.395 * yuv.y - 0.581 * yuv.z, yuv.x + 2.033 * yuv.y};
  }
  static CHEVAN_UTILS_VEC3 RGBToHSV(CHEVAN_UTILS_VEC3 rgb)
  {
    float c_max = maxInVec3(rgb);
    float c_min = minInVec3(rgb);
    float delta = c_max - c_min;
    bool c_max_is_r = rgb.r > rgb.g && rgb.r > rgb.b;
    bool c_max_is_g = rgb.g > rgb.r && rgb.g > rgb.b;
    bool c_max_is_b = rgb.b > rgb.r && rgb.b > rgb.g;
    float h = (c_max_is_r ? (rgb.g - rgb.b) / delta + 0 : c_max_is_g ? (rgb.b - rgb.r) / delta + 2
                                                      : c_max_is_b   ? (rgb.r - rgb.g) / delta + 4
                                                                     : 0);
    float s = c_max == 0 ? 0 : (delta / c_max);
    float v = c_max;
    return (CHEVAN_UTILS_VEC3){h / 6.f, s, v};
  }
  static CHEVAN_UTILS_VEC3 HSVToRGB(CHEVAN_UTILS_VEC3 hsv)
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

    return (CHEVAN_UTILS_VEC3){r + m, g + m, b + m};
  }

  static void print_chevanut_version()
  {
#ifdef __cplusplus
    std::cout << CHEVAN_UTILS_VERSION << std::endl;
#else
  printf("%s\n", CHEVAN_UTILS_VERSION);
#endif
  }

#ifdef __cplusplus
}
#endif

#endif