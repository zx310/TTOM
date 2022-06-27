# 介绍
用于在time_out时间内每隔timing时间去做一件事。
本项目用于自我学习使用，欢迎大家交流沟通。

# 设计灵感
## 需求：有一个linux项目需要60s内每隔1s去找局域网内是否存在需要配对的hub。
## 分析：设计一个适合如上相同目的的管理器，以实现在time_out时间内每隔timing时间去做一件事，一旦这件事做成功，则做成功后需要做的事，否则直到超时后执行超时需要做的事。

example code:
```
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
```
