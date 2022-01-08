#include <time.h>
#include <stdio.h>

typedef struct cchrono{
  time_t start,end;
} cchrono;

static inline void cchrono_start(cchrono *chrono) { chrono->start = clock(); }
#define cchrono_start(chrono) cchrono_start(&chrono)
static inline void cchrono_end(cchrono *chrono) { chrono->end = clock(); }
static inline double cchrono_elapsed(cchrono *chrono)
{
  cchrono_end(chrono);
  return ((double)(chrono->end - chrono->start)) / CLOCKS_PER_SEC;
}
#define cchrono_end(chrono) cchrono_end(&chrono)
#define cchrono_elapsed(chrono) cchrono_elapsed(&chrono)

#define cchrono_functionImpact(chrono, ...) \
  cchrono_start(chrono);                    \
  __VA_ARGS__;                              \
  printf("%F\n", cchrono_elapsed(chrono));
