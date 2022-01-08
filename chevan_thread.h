// platform translation layer for c threads
#pragma once
#ifndef chevan_thread_h
#define chevan_thread_h

#if defined(__unix__) || defined(__APPLE__) || __has_include(<pthread.h>)
#include <pthread.h>
typedef pthread_t cthread_t;

#define cthread_create(threadID, Func, ARG) pthread_create(&threadID, NULL, Func, ARG)
#define cthread_join(threadID, ret) pthread_join(threadID, ret)
#define cthread_detach(threadID) pthread_detach(threadID)

#define cthread_create_att(threadID, attribute, Func, ARG) pthread_create(threadID, attribute, Func, ARG)

#elif defined(__WIN32)
#include <windows.h>

typedef HANDLE cthread_t;

#define cthread_create(threadID, Func, ARG) threadID=CreateThread(NULL,0,Func,ARG,NULL)
#define cthread_join(threadID, ret) WaitForSingleObject(threadID, INFINITE);CloseHandle(threadID)
#define cthread_detach(threadID) CloseHandle(threadID)

#endif

#endif

/**
 * template for threaded loop
 * 
 * create thread array
 * for(int _thread=0;_thread<THREAD_COUNT;_thread++){
 *    launchThread(_thread,THREAD_COUNT)
 *        {
 *          for(int i=_thread;_thread<MAX;_thread+=THREAD_COUNT){
 *            ...
 *          }
 *        }
 * }
 * join threads
 * 
 **/