#pragma once
#ifndef CHEVAN_UTILS_H
#define CHEVAN_UTILS_H
#define CHEVAN_UTILS_VERSION "2.1.1"
#define CHEVAN_UTILS_INLINE inline

#define PIf 3.1415926535897f
#define ONE_OVER_SQUARE_ROOT_OF_TWO_PI 0.3989422804
#define EULERS_NUMBER 2.7182818284590452353602874713527
#ifndef CHEVAN_UTILS_FASTCALC_PRECISION
#define CHEVAN_UTILS_FASTCALC_PRECISION 9
#endif

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
#include "chevan_utils.hpp"
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>
#include <stdarg.h>

#define convertVec2(dest, src) \
  dest.x = (src).x;            \
  dest.y = (src).y;
#define convertVec3(dest, src) convertVec2(dest, src) dest.z = (src).z;
#define convertVec4(dest, src) convertVec3(dest, src) dest.w = (src).w;
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
#define membuild(type, name, data) \
  type name;                       \
  memcpy(&name, data, sizeof(type));

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ullong;
typedef long long llong;

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

#define _chevanut_vec_op_macro(name, op)                                               \
  static CHEVAN_UTILS_INLINE struct vec2 vec2_##name(struct vec2 a, struct vec2 b)     \
  {                                                                                    \
    return vec2(a.x op b.x, a.y op b.y);                                               \
  }                                                                                    \
  static CHEVAN_UTILS_INLINE struct vec3 vec3_##name(struct vec3 a, struct vec3 b)     \
  {                                                                                    \
    return vec3(a.x op b.x, a.y op b.y, a.z op b.z);                                   \
  }                                                                                    \
  static CHEVAN_UTILS_INLINE struct vec4 vec4_##name(struct vec4 a, struct vec4 b)     \
  {                                                                                    \
    return vec4(a.x op b.x, a.y op b.y, a.z op b.z, a.w op b.w);                       \
  }                                                                                    \
  static CHEVAN_UTILS_INLINE struct ivec2 ivec2_##name(struct ivec2 a, struct ivec2 b) \
  {                                                                                    \
    return ivec2(a.x op b.x, a.y op b.y);                                              \
  }                                                                                    \
  static CHEVAN_UTILS_INLINE struct ivec3 ivec3_##name(struct ivec3 a, struct ivec3 b) \
  {                                                                                    \
    return ivec3(a.x op b.x, a.y op b.y, a.z op b.z);                                  \
  }                                                                                    \
  static CHEVAN_UTILS_INLINE struct ivec4 ivec4_##name(struct ivec4 a, struct ivec4 b) \
  {                                                                                    \
    return ivec4(a.x op b.x, a.y op b.y, a.z op b.z, a.w op b.w);                      \
  }
_chevanut_vec_op_macro(add, +);
_chevanut_vec_op_macro(sub, -);
_chevanut_vec_op_macro(mult, *);
_chevanut_vec_op_macro(div, /);
#undef _chevanut_vec_op_macro

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
static CHEVAN_UTILS_INLINE float degreeToRad(float degree) { return degree / 180.f * 3.1415926535897f; }
static CHEVAN_UTILS_INLINE float radToDegree(float rad) { return rad / 3.1415926535897f * 180; }
static CHEVAN_UTILS_INLINE float randf() { return rand() / RAND_MAX; }

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
static void chevanut_print_vec2(struct vec2 v) { printf("{%f, %f}", v.x, v.y); }
static void chevanut_print_vec3(struct vec3 v) { printf("{%f, %f, %f}", v.x, v.y, v.z); }
static void chevanut_print_vec4(struct vec4 v) { printf("{%f, %f, %f, %f}", v.x, v.y, v.z, v.w); }
static void chevanut_print_ivec2(struct ivec2 v) { printf("{%d, %d}", v.x, v.y); }
static void chevanut_print_ivec3(struct ivec3 v) { printf("{%d, %d, %d}", v.x, v.y, v.z); }
static void chevanut_print_ivec4(struct ivec4 v) { printf("{%d, %d, %d, %d}", v.x, v.y, v.z, v.w); }

#ifdef CHEVAN_UTILS_MACRO_MAGIC
#define _chevanut_println_recurse_MAP(x) chevanut_print(x);
#define chevanut_println_recurse(...) EVAL(MAP(_chevanut_println_recurse_MAP, __VA_ARGS__, "\n"))
#endif

#define chevanut_print(x) _Generic((x),                     \
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
                                     struct vec2            \
                                   : chevanut_print_vec2,   \
                                     struct vec3            \
                                   : chevanut_print_vec3,   \
                                     struct vec4            \
                                   : chevanut_print_vec4,   \
                                     struct ivec2           \
                                   : chevanut_print_ivec2,  \
                                     struct ivec3           \
                                   : chevanut_print_ivec3,  \
                                     struct ivec4           \
                                   : chevanut_print_ivec4)(x)

#define chevanut_println(x) \
  chevanut_print(x);        \
  printf("\n")

#endif

#endif