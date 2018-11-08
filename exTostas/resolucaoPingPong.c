#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "debug.h"
#include "memory.h"
#include <pthread.h>
#define C_PING		(1)
#define C_PONG		(2)
#define C_NUM_TIMES	(10)

typedef struct thread_parameters{
	int status;
	pthread_mutex_t *ptr_mutex;
	pthread_cond_t *ptr_cond;
}thread_params_t;


void *ping(void *arg);
void *pong(void *arg);

int main(void){
	pthread_mutex_t mutex;
	pthread_cond_t cond;

	thread_params_t thread_params;
	thread_params.ptr_mutex = &mutex;
	thread_params.ptr_cond = &cond;
	thread_params.status = C_PING;

	// init mutex
	if ((errno = pthread_mutex_init(&mutex, NULL)) != 0)
		ERROR(12, "pthread_mutex_init() failed");
	// Var.Condição: inicializa variável de condição
	if ((errno = pthread_cond_init(&cond, NULL)) != 0)
		ERROR(14, "pthread_cond_init() failed!");
	//-----------------------------------------------------------
	pthread_t tid_ping, tid_pong;
	// cria uma thread + passagem de parâmetro
	if ((errno=pthread_create(&tid_ping,NULL,ping,&thread_params) != 0))
		ERROR(10, "Erro no pthread_create()!");
	if ((errno=pthread_create(&tid_pong,NULL,pong,&thread_params) != 0))
		ERROR(10, "Erro no pthread_create()!");
	
	// espera que a thread termine	
	if ((errno = pthread_join(tid_ping, NULL)) != 0)
		ERROR(11, "Erro no pthread_join()!\n");
	if ((errno = pthread_join(tid_pong, NULL)) != 0)
		ERROR(11, "Erro no pthread_join()!\n");

	// free resources
	if ((errno = pthread_mutex_destroy(&mutex)) != 0)
		ERROR(13, "pthread_mutex_destroy() failed");

	// Var.Condição: destroi a variável de condição 
	if ((errno = pthread_cond_destroy(&cond)) != 0)
		ERROR(15,"pthread_cond_destroy failed!");

	return 0;
}

void *ping(void *arg) {
	// cast para o tipo de dados enviado pela 'main thread'
	thread_params_t *params = (thread_params_t *) arg;

	for(int i=0;i<C_NUM_TIMES;i++){
		if ((errno = pthread_mutex_lock(params->ptr_mutex)) != 0){
			WARNING("pthread_mutex_lock() failed");
			return NULL;
		}

		while( params->status != C_PING ){
			if ((errno = pthread_cond_wait(params->ptr_cond,
						params->ptr_mutex)!=0)){   
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
		}
		printf("[#%02d] PING\n", i);
		params->status = C_PONG;
		
		if ((errno = pthread_cond_signal(params->ptr_cond)) != 0){
			WARNING("pthread_cond_signal() failed");
			return NULL;
		}

		// unlock
		if ((errno = pthread_mutex_unlock(params->ptr_mutex))!= 0){
			WARNING("pthread_mutex_unlock() failed");
			return NULL;
		}
	}//for i

	return NULL;
}

void *pong(void *arg) {
	// cast para o tipo de dados enviado pela 'main thread'
	thread_params_t *params = (thread_params_t *) arg;

	for(int i=0;i<C_NUM_TIMES;i++){
		if ((errno = pthread_mutex_lock(params->ptr_mutex)) != 0){
			WARNING("pthread_mutex_lock() failed");
			return NULL;
		}

		while( params->status != C_PONG ){
			if ((errno = pthread_cond_wait(params->ptr_cond,
						params->ptr_mutex)!=0)){   
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
		}
		printf("[#%02d] pong\n", i);
		params->status = C_PING;
		
		if ((errno = pthread_cond_signal(params->ptr_cond)) != 0){
			WARNING("pthread_cond_signal() failed");
			return NULL;
		}

		// unlock
		if ((errno = pthread_mutex_unlock(params->ptr_mutex))!= 0){
			WARNING("pthread_mutex_unlock() failed");
			return NULL;
		}
	}//for i

	return NULL;
}
