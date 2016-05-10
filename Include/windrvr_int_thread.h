#ifndef _WINDRVR_INT_THREAD_H_
#define _WINDRVR_INT_THREAD_H_

#if defined(WIN32) && !defined(WINCE)   
	#include <process.h>
#endif

#include "windrvr.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus 

#if defined(__KERNEL__)

    typedef void (*INT_HANDLER_FUNC)(PVOID pData);
    typedef struct 
    {
        HANDLE hWD;
        WD_INTERRUPT *pInt;
        INT_HANDLER_FUNC func;
        PVOID pData;
    } INT_THREAD_DATA;

    static void __cdecl InterruptThreadHandler (PVOID pContext)
    {
        INT_THREAD_DATA *pThread = (INT_THREAD_DATA *) pContext;

        WD_IntCount(pThread->hWD, pThread->pInt);
        pThread->func(pThread->pData);
    }

    static BOOL InterruptThreadEnable(HANDLE *phThread, HANDLE hWD, 
		WD_INTERRUPT *pInt, INT_HANDLER_FUNC func, PVOID pData)
    {
        INT_THREAD_DATA *pThread;
        *phThread = NULL;

        pThread = (INT_THREAD_DATA *) malloc(sizeof (INT_THREAD_DATA));
        if (!pThread)
            return FALSE;

        pInt->kpCall.hKernelPlugIn = WD_KERNEL_DRIVER_PLUGIN_HANDLE;
        pInt->kpCall.dwMessage = (DWORD) InterruptThreadHandler;
        pInt->kpCall.pData = pThread;
        WD_IntEnable (hWD, pInt);
		// check if WD_IntEnable failed
        if (!pInt->fEnableOk)
            return FALSE;
        
        pThread->pInt = pInt;
        pThread->hWD = hWD;
        pThread->func = func;
        pThread->pData = pData;

        *phThread = (HANDLE) pThread;
        return TRUE;
    }

    static void InterruptThreadDisable(HANDLE hThread)
    {
        if (hThread)
        {
            INT_THREAD_DATA *pThread = (INT_THREAD_DATA *) hThread;
            WD_IntDisable(pThread->hWD, pThread->pInt);
            free (pThread);
        }
    }
#else
	#if !defined(VXWORKS)
	    #include <malloc.h>
    	#if defined(UNIX)
        	#include <pthread.h>
		#endif
    #endif
    
    typedef void (*HANDLER_FUNC)(void *data);

	#if defined(VXWORKS)
		#define WAIT_FOR_EVER 0
		static void vxTask_wait(int taskId, int waitTime)
		{
			SEM_ID waitSem;
			
			if(waitTime == WAIT_FOR_EVER)
			{
				/* Loop while task is still alive */
				while(taskIdVerify(taskId) == OK)
					taskDelay(3);
			}
			else
			{
				/* 
				 * create a dummy semaphore and try to take it for the specified
				 * time.
				 */ 
				waitSem = semBCreate(SEM_Q_PRIORITY , SEM_EMPTY);
				semTake(waitSem, waitTime);
				semDelete(waitSem);
			}
		}
	#endif

	typedef struct 
	{
		HANDLER_FUNC func;
		void *data;
	} thread_struct_t;

	#if defined(WIN32) && !defined (WINCE)
       static unsigned int WINAPI thread_handler(void *data)
        #elif defined(WINCE)
            static unsigned long WINAPI thread_handler(void *data)
	#elif defined(OS2)
        static void thread_handler(void *data)
        #else
	static void* thread_handler(void *data)
	#endif
	{
		thread_struct_t *t = (thread_struct_t *)data;
		t->func(t->data);
		free(t);
		#if !defined(OS2)
			return 0;
		#endif
	}
	
	static void *thread_start(HANDLER_FUNC func, void *data)
	{
		thread_struct_t *t = (thread_struct_t *) malloc(sizeof(thread_struct_t));
		void *ret = NULL;
		
		t->func = func;
		t->data = data;
		#if defined(WIN32) && !defined(WINCE)
			ret = (void *) _beginthreadex(NULL, 0x1000, 
				thread_handler, (void *) t, 0, (unsigned int *) &GlobalDW);
		#elif defined(WINCE)
			ret = (void *) CreateThread ( NULL, 0x1000, 
				thread_handler,(void *) t, 0, (unsigned long *) &GlobalDW);
		#elif defined(OS2)
			ret = (void *) _beginthread(thread_handler, NULL, 0x4000, (void *)t);
		#elif defined(VXWORKS)
			{
				int priority;
				if (taskPriorityGet(taskIdSelf(), &priority)!=ERROR)
					ret = (PVOID) taskSpawn(NULL, priority, 0, 2000, 
						(FUNCPTR)thread_handler, (int)t, 0, 0, 0, 0, 0,
						0, 0, 0, 0);
				if ((int) ret == ERROR)
					ret = NULL;
			}
        #elif defined(UNIX)
			{
				int err;
				ret = malloc(sizeof(pthread_t));
            	err = pthread_create((pthread_t *)ret, NULL, thread_handler, (PVOID) t);
				if (err)
				{
					free(ret);
					ret = NULL;
				}
			}
        #endif
		if (!ret)
			free(t);
		return ret;
	}

	static void thread_stop(void *thread)
	{
		#if defined(WIN32)
		    WaitForSingleObject((HANDLE) thread, INFINITE);
            CloseHandle(thread);
		#elif defined(OS2)
			DosWaitThread((ULONG *)&thread, DCWW_WAIT);
		#elif defined(VXWORKS)
			vxTask_wait((int) thread, WAIT_FOR_EVER);
		#elif defined(UNIX)
            pthread_join((pthread_t) thread, NULL);
			free(thread);
        #endif
	}
	
    typedef struct 
    {
        HANDLE hWD;
        HANDLER_FUNC func;
        PVOID pData;    
        WD_INTERRUPT *pInt;     
		void *thread;
    } INT_THREAD_DATA;

      static void interrupt_thread_handler(void *data)
	{
		INT_THREAD_DATA *pThread = (INT_THREAD_DATA *) data;
        for (;;)
        {
            WD_IntWait (pThread->hWD, pThread->pInt);
            if (pThread->pInt->fStopped)
                break;
            pThread->func(pThread->pData);
        }
	}
	
    static BOOL InterruptThreadEnable(HANDLE *phThread, HANDLE hWD, 
		WD_INTERRUPT *pInt, HANDLER_FUNC func, PVOID pData)
    {
		INT_THREAD_DATA *pThread;
        *phThread = NULL;

        WD_IntEnable (hWD, pInt);
        // check if WD_IntEnable failed
        if (!pInt->fEnableOk)
            return FALSE;
        
        pThread = (INT_THREAD_DATA *) malloc(sizeof (INT_THREAD_DATA));
		BZERO(*pThread);
        pThread->func = func;
        pThread->pData = pData;
        pThread->hWD = hWD;
        pThread->pInt = pInt;

		pThread->thread = thread_start(interrupt_thread_handler, (void *)pThread);
		if(!pThread->thread)
		{
			free(pThread);
			return FALSE;
		}
		*phThread = (HANDLE) pThread;
		return TRUE;
    }
	
    static VOID InterruptThreadDisable(HANDLE hThread)
    {
        if (hThread)
        {
            INT_THREAD_DATA *pThread = (INT_THREAD_DATA *) hThread;
			WD_IntDisable(pThread->hWD, pThread->pInt);
			thread_stop(pThread->thread);
            free (pThread);
        }
    }

    typedef void (*cpci_event_func)(WD_CPCI_EVENT *event, void *data);

    typedef struct
    {
        cpci_event_func func;
        HANDLE hWD;
        void *data;
        HANDLE thread; 
        WD_INTERRUPT Int;
    } hs_event_handle_t; 

    static void hs_event_handler(void *h)
    {
        hs_event_handle_t *handle = (hs_event_handle_t *)h;
        WD_HS_REGISTER_EVENT pull;

        BZERO(pull);
        pull.hInterrupt = handle->Int.hInterrupt;
        WD_HsEventPull(handle->hWD, &pull);
        if (pull.cpciEvent.dwAction)
            handle->func(&pull.cpciEvent, handle->data);
    }

    static hs_event_handle_t *hs_register_event(HANDLE hWD, WD_CPCI_EVENT *event, cpci_event_func func, void *data)
    {
        hs_event_handle_t *handle = NULL;
        WD_HS_REGISTER_EVENT event_register;

        handle = (hs_event_handle_t *)malloc(sizeof(*handle));
        if (!handle)
            goto Error;
        BZERO(*handle);
        handle->func = func;
        handle->hWD = hWD; 
        handle->data = data;

        BZERO(event_register);
        event_register.cpciEvent = *event;
        WD_HsEventRegister(hWD, &event_register);
        if (!event_register.hInterrupt)
            goto Error;
        handle->Int.hInterrupt = event_register.hInterrupt; 
        if (!InterruptThreadEnable(&handle->thread, hWD, &handle->Int, hs_event_handler, (PVOID) handle))
            goto Error;
        if (!handle->Int.fEnableOk)
            goto Error;
        return handle;

    Error:
        if (handle && handle->Int.hInterrupt)
            WD_HsEventUnregister(hWD, handle);
        if (handle)
            free(handle);
        return NULL;
    }

    static void hs_unregister_event(HANDLE hWD, hs_event_handle_t *handle)
    {
        WD_HS_REGISTER_EVENT event;
        BZERO(event);
        event.hInterrupt = handle->Int.hInterrupt;
        InterruptThreadDisable(handle->thread);
        WD_HsEventUnregister(hWD, &event);
        free(handle);
    }

#endif

#ifdef __cplusplus
}
#endif  // __cplusplus 

#endif

