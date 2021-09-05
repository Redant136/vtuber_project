#define CHEVAN_UTILS_PRINT
#define CHEVAN_UTILS_VEC
#define CHEVAN_UTILS_MATH
#define CHEVAN_UTILS_INCL
#include "utils.hpp"
using namespace chevan_utils;
#define print println


int main()
{
  print("a");
  v3 a=v3(v2(1,0),1);
  print(a);

  m4 m=m4(1);
  m[1]=1;
  printMat4(m.array);
  print(m[1]);
  print(m.elements[1][0]);
  return 0;
}