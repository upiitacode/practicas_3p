/********/
/*Este proyecto hace uso del Semaforo*/
#include "TM4C123.h"                    // Device header
#include "retarget_tm4c.h"
#include <stdio.h>
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX


//definicion de threads
void tarea1(void const * args);
void tarea2(void const * args); 

osThreadId tarea1ID;
osThreadId tarea2ID;

osThreadDef(tarea1,osPriorityNormal,1,0);
osThreadDef(tarea2,osPriorityNormal,1,0);

#define N 100 /*numero  de ranuras en el buffer*/

#define signal_empty_not_z 1
#define signal_mutex_not_z 2
#define signal_full_not_z 4

//definicion del Semaphore
osMutexDef(myMutex);
osMutexId myMutex_Id;

int main(){
	UART0_init();
	osKernelInitialize();
	
	//creamos MUTEX
	myMutex_Id=osMutexCreate(osMutex(myMutex));  
	//creamos Threads
	tarea1ID=osThreadCreate(osThread(tarea1),NULL);
	tarea2ID=osThreadCreate(osThread(tarea2),NULL);
	osKernelStart();
	while(1){
		//osMutexWait(printLock_Id,osWaitForever);
		//printf("Main\n");
		//osMutexRelease(printLock_Id);
		osThreadYield();
	}
	
}

void tarea1(void const * args){
  osStatus status;
	while(1){
		//osThreadYield();
	  status=osMutexWait(myMutex_Id,osWaitForever);
		if(status==osOK){
			printf("Tarea1\n");
			osMutexRelease(myMutex_Id);
		}
		osDelay(500);
	}
	
}

void tarea2(void const * args){
	osStatus status;
	while(1){
		osThreadYield();
		status = osMutexWait(myMutex_Id,osWaitForever);
		if(status==osOK){
			printf("    Tarea2\n");
			osMutexRelease(myMutex_Id);
		}
		osDelay(100);
	}
} 
