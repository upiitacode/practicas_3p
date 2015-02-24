/********/
/*Este proyecto hace uso del Semaforo*/
#include "TM4C123.h"                    // Device header
#include "retarget_tm4c.h"
#include <stdio.h>
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX

//thread prototypes
void producer(void const * args);
void consumer(void const * args);
//Tread ID's
osThreadId producerID;
osThreadId consumerID;
//Thread definition macros
osThreadDef(producer,osPriorityNormal,1,0);
osThreadDef(consumer,osPriorityNormal,1,0);

#define signal_mutex_not_z (0x1<<2)
#define signal_empty_not_z (0x1<<1)
#define signal_full_not_z  (0x1<<3)

//definicion del Semaphore
osMutexDef(SM_mutex);
osMutexId SM_mutex_Id;
osMutexDef(SM_empty);
osMutexId SM_empty_Id;
osMutexDef(SM_full);
osMutexId SM_full_Id;

/*Codigo del ejemplo de mutex*/
#define N 100 /*numero  de ranuras en el buffer*/
typedef struct{
	int counter;
	int signalN;
	osMutexId* pMutex;
}semaphore;

semaphore mutex = {0,signal_mutex_not_z,&SM_mutex_Id};
semaphore empty = {N,signal_empty_not_z,&SM_empty_Id};
semaphore full = {0,signal_full_not_z,&SM_full_Id};

int main(){
	UART0_init();
	osKernelInitialize();
	//creamos MUTEX
	SM_mutex_Id=osMutexCreate(osMutex(SM_mutex));
	SM_empty_Id=osMutexCreate(osMutex(SM_empty));
	SM_full_Id=osMutexCreate(osMutex(SM_full));
	//creamos Threads
	producerID=osThreadCreate(osThread(producer),NULL);
	consumerID=osThreadCreate(osThread(consumer),NULL);
	osKernelStart();
	while(1){
		//osMutexWait(printLock_Id,osWaitForever);
		//printf("Main\n");
		//osMutexRelease(printLock_Id);
		osThreadYield();
	}
	
}

/*Required functino prototypes*/
/*Data sink and source functions*/
int produce_item(void item);
void consume_item(int item);
/*semaphore control functions*/
void down(semaphore*);
void up(semaphore*);
/*buffer fucntions*/
void enter_item(int*);
void remove_item(int*);

/*Producer thread fucntion*/
void producer(void const * args){
	int item;
	while(1){
		item=produce_item();
		down(&empty);
		down(&mutex);
		enter_item(&item);
		up(&mutex);
		up(&full);
	}
}
/*Consumer thread fucntion*/
void consumer(void const * args){
	int item;
	while(TRUE){
		down(&full);
		down(&mutex);
		remove_item(&item);
		up(&mutex);
		up(&empty);
		consume_item(item);
	}
} 

void down(semaphore *pSemaphore){

}

void up(semaphore *pSemaphore){

}

void enter_item(int *pItem){

}

void remove_item(int *pItem){

}

int produce_item(void){
	return 0
}

void consume_item(int item){

}
