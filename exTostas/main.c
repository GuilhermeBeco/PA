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
#define C_CORTA		(1)
#define C_QUEIJO	(2)
#define C_FIAMBRE	(3)
#define C_DONE		(4)
#define C_NUM_TIMES	(1)

void *fatias(void *arg); 
void *queijo(void *arg); 
void *fiambre(void *arg); 
void *done(void *arg) ;

typedef struct thread_parameters{
	int status;
	int statusStart;
	 //tosta a sair
	pthread_mutex_t *ptr_mutex;
	pthread_cond_t *ptr_cond;
}thread_params_t;




int main(void){
	pthread_mutex_t mutex;
	pthread_cond_t cond;

	thread_params_t thread_params;
	thread_params.ptr_mutex = &mutex;
	thread_params.ptr_cond = &cond;
	thread_params.status = C_CORTA ;


	// init mutex
	if ((errno = pthread_mutex_init(&mutex, NULL)) != 0)
		ERROR(12, "pthread_mutex_init() failed");
	// Var.Condição: inicializa variável de condição
	if ((errno = pthread_cond_init(&cond, NULL)) != 0)
		ERROR(14, "pthread_cond_init() failed!");
	//-----------------------------------------------------------
	pthread_t tid_fatias, tid_queijo , tid_fiambre , tid_done;
	// cria uma thread + passagem de parâmetro
	if ((errno=pthread_create(&tid_fatias,NULL,fatias,&thread_params) != 0))
		ERROR(10, "Erro no pthread_create()!");
	if ((errno=pthread_create(&tid_queijo,NULL,queijo,&thread_params) != 0))
		ERROR(10, "Erro no pthread_create()!");
	if ((errno=pthread_create(&tid_fiambre,NULL,fiambre,&thread_params) != 0))
		ERROR(10, "Erro no pthread_create()!");
	if ((errno=pthread_create(&tid_done,NULL,done,&thread_params) != 0))
		ERROR(10, "Erro no pthread_create()!");
	
	
	// espera que a thread termine	
	if ((errno = pthread_join(tid_fatias, NULL)) != 0)
		ERROR(11, "Erro no pthread_join()!\n");
	if ((errno = pthread_join(tid_queijo, NULL)) != 0)
		ERROR(11, "Erro no pthread_join()!\n");
	if ((errno = pthread_join(tid_fiambre, NULL)) != 0)
		ERROR(11, "Erro no pthread_join()!\n");
	if ((errno = pthread_join(tid_done, NULL)) != 0)
		ERROR(11, "Erro no pthread_join()!\n");

	// free resources
	if ((errno = pthread_mutex_destroy(&mutex)) != 0)
		ERROR(13, "pthread_mutex_destroy() failed");

	// Var.Condição: destroi a variável de condição 
	if ((errno = pthread_cond_destroy(&cond)) != 0)
		ERROR(15,"pthread_cond_destroy failed!");

	return 0;
}

void *fatias(void *arg) {
	// cast para o tipo de dados enviado pela 'main thread'
	thread_params_t *params = (thread_params_t *) arg;

	for(int i=0;i<C_NUM_TIMES;i++){
		if ((errno = pthread_mutex_lock(params->ptr_mutex)) != 0){
			WARNING("pthread_mutex_lock() failed");
			return NULL;
		}

		while( params->status != C_CORTA ){
			if ((errno = pthread_cond_wait(params->ptr_cond,
						params->ptr_mutex)!=0)){   
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
		}
		printf("Fatias cortadas\n");
		params->status=C_QUEIJO;
		
		if ((errno = pthread_cond_broadcast(params->ptr_cond)) != 0){
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

void *queijo(void *arg) {
	// cast para o tipo de dados enviado pela 'main thread'
	thread_params_t *params = (thread_params_t *) arg;
	int nFatias=0;
	for(int i=0;i<C_NUM_TIMES;i++){
		if ((errno = pthread_mutex_lock(params->ptr_mutex)) != 0){
			WARNING("pthread_mutex_lock() failed");
			return NULL;
		}

		while( params->status != C_QUEIJO ){
			if ((errno = pthread_cond_wait(params->ptr_cond,
						params->ptr_mutex)!=0)){   
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
		}
		
		printf("Fatia de queijo no 1\n");
		
		
		params->status=C_FIAMBRE;
		printf("\n%d",params->status);
		if ((errno = pthread_cond_broadcast(params->ptr_cond)) != 0){
			WARNING("pthread_cond_signal() failed");
			return NULL;
		}
		while( params->status != 5 ){
			if ((errno = pthread_cond_wait(params->ptr_cond,
						params->ptr_mutex)!=0)){   
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
		}
		printf("Fatia de queijo no 2\n");
		params->status=C_DONE;
		if ((errno = pthread_cond_broadcast(params->ptr_cond)) != 0){
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

void *fiambre(void *arg) {
	// cast para o tipo de dados enviado pela 'main thread'
	thread_params_t *params = (thread_params_t *) arg;
	
	for(int i=0;i<C_NUM_TIMES;i++){
		if ((errno = pthread_mutex_lock(params->ptr_mutex)) != 0){
			WARNING("pthread_mutex_lock() failed");
			return NULL;
		}

		while( params->status != C_FIAMBRE ){
			if ((errno = pthread_cond_wait(params->ptr_cond,
						params->ptr_mutex)!=0)){   
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
		}
		
		printf("Fatia de fiambre \n");
		
		params->status=5;
		
		
		if ((errno = pthread_cond_broadcast(params->ptr_cond)) != 0){
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

void *done(void *arg) {
	// cast para o tipo de dados enviado pela 'main thread'
	thread_params_t *params = (thread_params_t *) arg;
	printf("HI\n");
	for(int i=0;i<C_NUM_TIMES;i++){
		if ((errno = pthread_mutex_lock(params->ptr_mutex)) != 0){
			WARNING("pthread_mutex_lock() failed");
			return NULL;
		}

		while( params->status==C_DONE ){
			if ((errno = pthread_cond_wait(params->ptr_cond,
						params->ptr_mutex)!=0)){   
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
		}
	
		printf("Toasting \n");
		
		
		printf("Entrgue\n");
		 params->status=C_CORTA;
	
		
		if ((errno = pthread_cond_broadcast(params->ptr_cond)) != 0){
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


