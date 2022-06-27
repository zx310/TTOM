/*
 * TimingTimeOutManager.c
 *
 *  Created on: 2022年5月6日
 *      Author: ZhouXin
 */
#include "TimingTimeOutManager.h"

#define return_if_fail(p) if((p) == 0){printf ("[%s]:func error!\n", __func__);return;}

static void* pthread_func (void* argv)
{
	TTOMStatus_e status = TTOM_STOP;
	TTOMPrivInfo_s *info = (TTOMPrivInfo_s*)argv;
	unsigned int count = 0;

	pthread_detach(pthread_self());
    while (TTOM_TASK_RUNNING == info->task_running)
    {
    	//! An external API can control the completion of a task
    	if(info->process_status == TTOM_STOP)
    	{
    		status = TTOM_STOP;
    	}

    	if(TTOM_STOP == status)
    	{
    		info->process_status = TTOM_STOP;
			sem_wait(&info->s1);//阻塞在这里

			count = info->time_out / info->timing;
			//! count must greater than zero
			status = (count > 0) ? TTOM_START : TTOM_STOP;
			continue;
    	}
		else if(TTOM_START == status)
		{
			if(NULL != info->timing_call_back_func)
			{
				TTOMResult_e ret = info->timing_call_back_func(info->timing_call_back_argv);
				//! If the expected result is obtained during scheduled invocation, the task is automatically terminated
				if(TTOM_SUEECSS == ret)
				{
					status = TTOM_STOP;
					if(NULL != info->success_call_back_func)
					{
						info->success_call_back_func(info->success_call_back_argv);
					}
					continue;
				}
			}

			count--;
			//! If the expected result is not obtained during the scheduled call until the timeout, perform the following steps
			if(count == 0)
			{
				status = TTOM_STOP;
				if(NULL != info->time_out_call_back_func)
				{
					info->time_out_call_back_func(info->time_out_call_back_argv);
				}
				continue;
			}
    	}
        sleep (info->timing);
    }

    pthread_exit(NULL);
}

void TTOM_Init(TTOMPrivInfo_s* info,  unsigned int time_out, unsigned int timing)
{
	return_if_fail (info != NULL);
	pthread_t pt = 0;
	int ret = 0;

	if(info->task_running != TTOM_TASK_RUNNING)
	{
		info->task_running = TTOM_TASK_RUNNING;
		info->process_status = TTOM_STOP;
		info->time_out = time_out;
		info->timing = timing;
		info->timing_call_back_func = NULL;
		info->time_out_call_back_func = NULL;
		info->success_call_back_func = NULL;
		info->timing_call_back_argv = NULL;
		info->time_out_call_back_argv = NULL;
		info->success_call_back_argv = NULL;

		sem_init (&info->s1, 0, 0);

		ret = pthread_create (&pt, NULL, pthread_func, (void*)info);
		if (ret != 0)
		{
			info->task_running = !TTOM_TASK_RUNNING;
			sem_destroy(&info->s1);
			printf("%s: pthread_create error\n", __func__);
		}
	}
}

void TTOM_SetTimingCallBackFunc(TTOMPrivInfo_s* info, TTOMCallBack timing_call_back_func, void* timing_call_back_argv)
{
	return_if_fail (info != NULL);
	if(info->process_status != TTOM_START)
	{
		info->timing_call_back_func = timing_call_back_func;
		info->timing_call_back_argv = timing_call_back_argv;
	}
}

void TTOM_SetSuccessCallBackFunc(TTOMPrivInfo_s* info, TTOMCallBack success_call_back_func, void* success_call_back_argv)
{
	return_if_fail (info != NULL);
		if(info->process_status != TTOM_START)
		{
			info->success_call_back_func = success_call_back_func;
			info->success_call_back_argv = success_call_back_argv;
		}
}
void TTOM_SetTimeOutCallBackFunc(TTOMPrivInfo_s* info, TTOMCallBack time_out_call_back_func, void* time_out_call_back_argv)
{
	return_if_fail (info != NULL);
	if(info->process_status != TTOM_START)
	{
		info->time_out_call_back_func = time_out_call_back_func;
		info->time_out_call_back_argv = time_out_call_back_argv;
	}
}

void TTOM_DeInit(TTOMPrivInfo_s* info)
{
	return_if_fail (info != NULL);
	info->task_running = !TTOM_TASK_RUNNING;
	info->timing_call_back_func = NULL;
	info->time_out_call_back_func = NULL;
	info->timing_call_back_argv = NULL;
	info->time_out_call_back_argv = NULL;
	info->success_call_back_func = NULL;
	info->success_call_back_argv = NULL;
	sem_destroy(&info->s1);
	if(info->process_status != TTOM_STOP)
	{
		info->process_status = TTOM_STOP;
	}
}

void TTOM_Stop(TTOMPrivInfo_s* info)
{
	return_if_fail (info != NULL);
	info->process_status = TTOM_STOP;
}

void TTOM_Start(TTOMPrivInfo_s* info)
{
	return_if_fail (info != NULL);
	if(info->process_status != TTOM_START)
	{
		info->process_status = TTOM_START;
		sem_post(&info->s1);
	}
}

void TTOM_Start2(TTOMPrivInfo_s* info, unsigned int time_out, unsigned int timing)
{
	return_if_fail (info != NULL);
	if(info->process_status != TTOM_START)
	{
		info->time_out = time_out;
		info->timing = timing;
		info->process_status = TTOM_START;
		sem_post(&info->s1);
	}
}


TTOMStatus_e TTOM_Get_Status(TTOMPrivInfo_s* info)
{
	if(NULL == info)
	{
		return TTOM_STOP;
	}
	return info->process_status;
}

