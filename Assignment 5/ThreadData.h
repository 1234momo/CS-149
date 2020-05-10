#ifndef THREADDATA_H
#define THREADDATA_H
#include <pthread.h>

struct THREADDATA_STRUCT {
  pthread_t creator;
} ThreadData;

#endif