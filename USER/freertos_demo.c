#include "freertos_demo.h"
#include "../SYSTEM/usart/usart.h"
/*FreeRTOS*********************************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx.h"
/******************************************************************************************************/
/*FreeRTOS配置*/

/* 动态创建	*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO 1                   /* 任务优先级 */
#define START_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            StartTask_Handler;  /* 任务句柄 */
StackType_t 						StartTask_stack[START_STK_SIZE];
StaticTask_t 						start_task_tcb;
void start_task(void *pvParameters);        /* 任务函数 */

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO      2                   /* 任务优先级 */
#define TASK1_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task1Task_Handler;  /* 任务句柄 */
StackType_t 						Task1_stack[TASK1_STK_SIZE];
StaticTask_t 						task1_tcb;
void task1(void *pvParameters);             /* 任务函数 */


/* TASK3 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK3_PRIO      4                   /* 任务优先级 */
#define TASK3_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task3Task_Handler;  /* 任务句柄 */
StackType_t 						Task3_stack[TASK3_STK_SIZE];
StaticTask_t 						task3_tcb;
void task3(void *pvParameters);             /* 任务函数 */


/*	静态创建	*/


/* 空闲任务内存分配 -> 静态使用	*/
StaticTask_t idle_task_tcb;
StackType_t idle_task_stack[configMINIMAL_STACK_SIZE];
void vApplicationGetIdleTaskMemory(
		StaticTask_t ** ppxIdleTaskTCBBuffer,
		StackType_t	** ppxIdleTaskStackBuffer,
		uint32_t * pulIdleTaskStackSize
){
	* ppxIdleTaskTCBBuffer = &idle_task_tcb;
	* ppxIdleTaskStackBuffer = idle_task_stack;
	* pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* 软件定时器内存分配 */
StaticTask_t timer_task_tcb;
StackType_t timer_task_stack[configTIMER_TASK_STACK_DEPTH];
void vApplicationGetTimerTaskMemory(
		StaticTask_t ** ppxTimerTaskTCBBuffer,
		StackType_t	** ppxTimerTaskStackBuffer,
		uint32_t * pulTimerTaskStackSize
){
	* ppxTimerTaskTCBBuffer = &timer_task_tcb;
	* ppxTimerTaskStackBuffer = timer_task_stack;
	* pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

/******************************************************************************************************/
/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{	
	// 动态
	xTaskCreate(
		(TaskFunction_t) start_task,
		(char *) "_start_task",
		(configSTACK_DEPTH_TYPE) START_STK_SIZE,
		(void *) NULL,
		(UBaseType_t) START_TASK_PRIO,
		(TaskHandle_t *) &StartTask_Handler
	);
  vTaskStartScheduler();
}

/**
 * @brief       start_task
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */ 
void start_task(void *pvParameters){
	while(1){
		taskENTER_CRITICAL();		// 进入临界区
		
		/* 动态创建	*/
		xTaskCreate(
			(TaskFunction_t) task1,
			(char *) "_task1",
			(configSTACK_DEPTH_TYPE) TASK1_STK_SIZE,
			(void *) NULL,
			(UBaseType_t) TASK1_PRIO,
			(TaskHandle_t *) &Task1Task_Handler
		);
		xTaskCreate(
			(TaskFunction_t) task3,
			(char *) "_task3",
			(configSTACK_DEPTH_TYPE) TASK3_STK_SIZE,
			(void *) NULL,
			(UBaseType_t) TASK3_PRIO,
			(TaskHandle_t *) &Task3Task_Handler
		);
			
		vTaskDelete(NULL);
		taskEXIT_CRITICAL();	// 退出临界区
	}
}

/**
 * @brief       task1
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task1(void *pvParameters){
	while(1){
		
	}
}

/**
 * @brief       task3
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task3(void *pvParameters){
	while(1){

	}
}
