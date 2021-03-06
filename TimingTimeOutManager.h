/*
 * TimingTimeOutManager.h
 *
 *  Created on: 2022年5月6日
 *      Author: ZhouXin
 */

/**
 * 	release note:
 * 	2022.06.27:
 * 		1. version: 1.0
 * 		2. delete thread callback function "static status", support multi thread function.
 */
#ifndef TIMINGTIMEOUTMANAGER_H_
#define TIMINGTIMEOUTMANAGER_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define TTOM_VERSION		"1.0"
#define TTOM_TASK_RUNNING	(1)

typedef enum _TaskStatus
{
	TTOM_STOP = 0,
	TTOM_START,
}TTOMStatus_e;

typedef enum _TaskResult
{
	TTOM_FAILED = 0,
	TTOM_SUEECSS = 1,
}TTOMResult_e;

typedef TTOMResult_e (*TTOMCallBack)(void*);

typedef struct _PrivInfo
{
	TTOMStatus_e process_status;   //task status
    unsigned int task_running;		//task thread running or not
    unsigned int time_out;			//task timeout time
    unsigned int timing;			//task timing time
    sem_t s1;						//task semaphore
    TTOMCallBack timing_call_back_func;   //timing task function
    void* timing_call_back_argv;	   	  //timing task function params
    TTOMCallBack success_call_back_func;   //timing task sucess function
    void* success_call_back_argv;	      //timing task function sucess params
    TTOMCallBack time_out_call_back_func;  //timeout call back function
    void* time_out_call_back_argv;	   //timeout call back function params
}TTOMPrivInfo_s;

/**
 * @fn void TTOM_Init(TTOMPrivInfo_s*, unsigned int, unsigned int)
 * @brief
 * 		  initialization  TTOM
 * @param info		TTOMPrivInfo_s struct pointer
 * @param time_out	timeout time, unit: s
 * @param timing	timing time: unit: s
 */
void TTOM_Init(TTOMPrivInfo_s* info,  unsigned int time_out, unsigned int timing);

/**
 * @fn void TTOM_SetTimingCallBackFunc(TTOMPrivInfo_s*, TTOMCallBack,)
 * @brief
 * 		  setting timing time out callback function,  must be called before TTOM_Start
 * @param info	TTOMPrivInfo_s struct pointer
 * @param timing_call_back_func  timing time out call back function pointer
 * @param timing_call_back_argv	 timing time out call back function parameter
 */
void TTOM_SetTimingCallBackFunc(TTOMPrivInfo_s* info, TTOMCallBack timing_call_back_func, void* timing_call_back_argv);
/**
 * @fn void TTOM_SetSuccessCallBackFunc(TTOMPrivInfo_s*, TTOMCallBack,)
 * @brief
 * 		  setting timing task successful callback function,  must be called before TTOM_Start
 * @param info	TTOMPrivInfo_s struct pointer
 * @param success_call_back_func  timing task successful call back function pointer
 * @param success_call_back_argv	 timing task successful call back function parameter
 */
void TTOM_SetSuccessCallBackFunc(TTOMPrivInfo_s* info, TTOMCallBack success_call_back_func, void* success_call_back_argv);
/**
 * @fn void TTOM_SetTimeOutCallBackFunc(TTOMPrivInfo_s*, TTOMCallBack)
 * @brief
 * 		  setting time out callback function,  must be called before TTOM_Start
 * @param info	TTOMPrivInfo_s struct pointer
 * @param time_out_call_back_func 	time out call back function pointer
 * @param time_out_call_back_argv	time out call back function pointer
 */
void TTOM_SetTimeOutCallBackFunc(TTOMPrivInfo_s* info, TTOMCallBack time_out_call_back_func, void* time_out_call_back_argv);
/**
 * @fn void TTOM_DeInit(TTOMPrivInfo_s*)
 * @brief
 * 		  uninitialization TTOM, It is best to call TTOM_Stop before calling this function.
 * @param info	TTOMPrivInfo_s struct pointer
 */
void TTOM_DeInit(TTOMPrivInfo_s* info);
/**
 * @fn void TTOM_Stop(TTOMPrivInfo_s*)
 * @brief
 *		  stop TTOM
 * @param info	TTOMPrivInfo_s struct pointer
 */
void TTOM_Stop(TTOMPrivInfo_s* info);
/**
 * @fn void TTOM_Start(TTOMPrivInfo_s*)
 * @brief
 * 		  start TTOM, use initialization function timeout time and timing time
 * @param info	TTOMPrivInfo_s struct pointer
 */
void TTOM_Start(TTOMPrivInfo_s* info);
/**
 * @fn void TTOM_Start2(TTOMPrivInfo_s*, unsigned int, unsigned int)
 * @brief
 * 		  start TTOM, use newer timeout time and timing time
 * @param info		TTOMPrivInfo_s struct pointer
 * @param time_out	timeout time, unit: s
 * @param timing	timing time: unit: s
 */
void TTOM_Start2(TTOMPrivInfo_s* info, unsigned int time_out, unsigned int timing);
/**
 * @fn TTOTMStatus_e TTOM_Get_Status(TTOMPrivInfo_s*)
 * @brief
 * 		  get task status, stop or start
 * @param info	TTOMPrivInfo_s struct pointer
 * @return
 */
TTOMStatus_e TTOM_Get_Status(TTOMPrivInfo_s* info);


/*example test code*/
#if 0
/*Test1: single thread*/
#include "TimingTimeOutManager.h"
#include <stdio.h>
#include <string.h>

#define TIME_OUT	(10) //s
#define TIMING		(1) //s

static TTOMResult_e timing_call_back_func(void* argv)
{
	static int count = 5;
	int tmp = *(int*)argv;
	printf("hello [%d]\n", tmp);
	return --count == 0 ? TTOM_SUEECSS :TTOM_FAILED;
}
static TTOMResult_e sueecss_call_back_func(void* argv)
{
	int tmp = *(int*)argv;
	printf("success [%d]\n", tmp);
	return TTOM_SUEECSS;
}
static TTOMResult_e timeout_call_back_func(void* argv)
{
	int tmp = *(int*)argv;
	printf("timeout [%d]\n", tmp);
	return TTOM_SUEECSS;
}

int main(void)
{
	setbuf(stdout,NULL);
	TTOMPrivInfo_s info;
	int tmp = 100;
	int tmp1 = 1000;
	memset(&info, 0, sizeof(TTOMPrivInfo_s));

	TTOM_Init(&info, TIME_OUT, TIMING);
	TTOM_SetTimingCallBackFunc(&info, timing_call_back_func,  (void*)&tmp);
	TTOM_SetSuccessCallBackFunc(&info, sueecss_call_back_func,  (void*)&tmp);
	TTOM_SetTimeOutCallBackFunc(&info, timeout_call_back_func,  (void*)&tmp1);
	sleep(5);
	TTOM_Start(&info);
	sleep(TIME_OUT+1);

	return EXIT_SUCCESS;
}
/*Test2: multi thread*/
#include "TimingTimeOutManager.h"
#include <stdio.h>
#include <string.h>

#define TIME_OUT	(10) //s
#define TIMING		(1) //s
#define TIME_OUT1	(8) //s
#define TIMING1		(2) //s

static TTOMResult_e timing_call_back_func(void* argv)
{
	static int count = 10;
	int tmp = *(int*)argv;
	printf("hello [%d]\n", tmp);
	return --count == 0 ? TTOM_SUEECSS :TTOM_FAILED;
}
static TTOMResult_e sueecss_call_back_func(void* argv)
{
	int tmp = *(int*)argv;
	printf("success [%d]\n", tmp);
	return TTOM_SUEECSS;
}
static TTOMResult_e timeout_call_back_func(void* argv)
{
	int tmp = *(int*)argv;
	printf("timeout [%d]\n", tmp);
	return TTOM_SUEECSS;
}
static TTOMResult_e timing_call_back_func1(void* argv)
{
	static int count = 5;
	int tmp = *(int*)argv;
	printf("hello1 [%d]\n", tmp);
	return --count == 0 ? TTOM_SUEECSS :TTOM_FAILED;
}
static TTOMResult_e sueecss_call_back_func1(void* argv)
{
	int tmp = *(int*)argv;
	printf("success1 [%d]\n", tmp);
	return TTOM_SUEECSS;
}
static TTOMResult_e timeout_call_back_func1(void* argv)
{
	int tmp = *(int*)argv;
	printf("timeout1 [%d]\n", tmp);
	return TTOM_SUEECSS;
}

int main(void)

{
	setbuf(stdout,NULL);
	TTOMPrivInfo_s info;
	int tmp = 100;
	int tmp1 = 1000;
	memset(&info, 0, sizeof(TTOMPrivInfo_s));

	TTOM_Init(&info, TIME_OUT, TIMING);
	TTOM_SetTimingCallBackFunc(&info, timing_call_back_func,  (void*)&tmp);
	TTOM_SetSuccessCallBackFunc(&info, sueecss_call_back_func,  (void*)&tmp);
	TTOM_SetTimeOutCallBackFunc(&info, timeout_call_back_func,  (void*)&tmp1);

	TTOMPrivInfo_s info1;
	int tmp11 = 100;
	int tmp12 = 1000;
	memset(&info1, 0, sizeof(TTOMPrivInfo_s));

	TTOM_Init(&info1, TIME_OUT1, TIMING1);
	TTOM_SetTimingCallBackFunc(&info1, timing_call_back_func1,  (void*)&tmp11);
	TTOM_SetSuccessCallBackFunc(&info1, sueecss_call_back_func1,  (void*)&tmp11);
	TTOM_SetTimeOutCallBackFunc(&info1, timeout_call_back_func1,  (void*)&tmp12);
	sleep(1);
	TTOM_Start(&info);
	TTOM_Start(&info1);
	pause();

	return EXIT_SUCCESS;
}

#endif
#endif /* TIMINGTIMEOUTMANAGER_H_ */
