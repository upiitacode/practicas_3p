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

#define signal_mutex_not_z (0x1<<0)
#define signal_empty_not_z (0x1<<1)
#define signal_full_not_z  (0x1<<2)

//definicion del Semaphore
osMutexDef(SM_mutex);
osMutexId SM_mutex_Id;
osMutexDef(SM_empty);
osMutexId SM_empty_Id;
osMutexDef(SM_full);
osMutexId SM_full_Id;

/*Codigo del ejemplo de mutex*/
#define N 20 /*numero  de ranuras en el buffer*/
typedef struct{
	int counter;
	int signalN;
	osMutexId* pMutex;
	osThreadId* pSleepingThread;
}semaphore;

semaphore mutex = {1,signal_mutex_not_z,&SM_mutex_Id,NULL};
semaphore empty = {N,signal_empty_not_z,&SM_empty_Id,NULL};
semaphore full = {0,signal_full_not_z,&SM_full_Id,NULL};


int main(){
	UART0_init();
	printf("\nkernel init");
	osKernelInitialize();
	printf("\t[ready]\n");
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
int produce_item(void);
void consume_item(int item);
/*semaphore control functions*/
void down(semaphore*,osThreadId*);
void up(semaphore*);
/*buffer fucntions*/
void enter_item(int*);
void remove_item(int*);

/*Producer thread fucntion*/
void producer(void const * args){
	int item;
	while(1){
		item=produce_item();
		down(&empty,&producerID);
		down(&mutex,&producerID);
		enter_item(&item);
		up(&mutex);
		up(&full);
	}
}
/*Consumer thread fucntion*/
void consumer(void const * args){
	int item;
	while(1){
		down(&full,&consumerID);
		down(&mutex,&consumerID);
		remove_item(&item);
		up(&mutex);
		up(&empty);
		consume_item(item);
	}
}

void down(semaphore *pSemaphore,osThreadId *p_callerId){
	osMutexWait(*(pSemaphore->pMutex),osWaitForever);//wait for mutex on this semaphore
	if((pSemaphore->counter<=0)){
		pSemaphore->pSleepingThread=p_callerId;//Save treadId so this thread recives the wakeup signal 
		osMutexRelease(*(pSemaphore->pMutex));//release mutex to avoid deadlock
		osSignalWait(pSemaphore->signalN,osWaitForever);//wait  for wakeup signal
		osSignalClear(*p_callerId,pSemaphore->signalN);
		osMutexWait(*(pSemaphore->pMutex),osWaitForever);//wait for mutex on this semaphore
	}
	pSemaphore->counter--;//decrease coutner
	osMutexRelease(*(pSemaphore->pMutex));//release mutex
}

void up(semaphore *pSemaphore){
	osMutexWait(*(pSemaphore->pMutex),osWaitForever);//wait to get the mutex on this semaphore
	if((pSemaphore->pSleepingThread)){//if valid pointer
		osSignalSet(*(pSemaphore->pSleepingThread),pSemaphore->signalN);//send signal to sleeping thread
	}
	pSemaphore->counter++;//increase semaphore counter
	osMutexRelease(*(pSemaphore->pMutex));//release mutex
}

int buffer_array[N];
int index_head=0;
int index_tail=0;

void enter_item(int *pItem){
	buffer_array[index_head]= *pItem;
	index_head++;
	index_head%=N;
}

void remove_item(int *pItem){
	*pItem=buffer_array[index_tail];
	index_tail++;
	index_tail%=N;
}

int mycounter=0;
int produce_item(void){
	mycounter++;
	if((mycounter%25)==0){
		osDelay(1000);
	}
	return mycounter;
}

void consume_item(int item){
	printf("i=%d\n",item);
}
