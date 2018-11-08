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
	int statusStart;
	int statusLavaExterior;
	int statusEnxaguaExterior;
	int statusAspira;
	int statusEstofos;
	int statusVidrosInterior;
	int statusPlastico;
	int statusJantes;
	int statusEspelhos;
	pthread_mutex_t *ptr_mutex;
	pthread_cond_t *ptr_cond;
}thread_params_t;


void *pthread1(void *arg);
void *pthread2(void *arg);

int main(void){
	pthread_mutex_t mutex;
	pthread_cond_t cond;

	thread_params_t thread_params;
	thread_params.ptr_mutex = &mutex;
	thread_params.ptr_cond = &cond;
	thread_params.statusStart =1;
	thread_params.statusLavaExterior =0;
	thread_params.statusEnxaguaExterior =0;
	thread_params.statusAspira =0;
	thread_params.statusEstofos =0;
	thread_params.statusVidrosInterior =0;
	thread_params.statusPlastico =0;
	thread_params.statusJantes =0;
	thread_params.statusEspelhos =0;

	// init mutex
	if ((errno = pthread_mutex_init(&mutex, NULL)) != 0)
		ERROR(12, "pthread_mutex_init() failed");
	// Var.Condição: inicializa variável de condição
	if ((errno = pthread_cond_init(&cond, NULL)) != 0)
		ERROR(14, "pthread_cond_init() failed!");
	//-----------------------------------------------------------
	pthread_t tid_ping, tid_pong;
	
	// cria uma thread + passagem de parâmetro
	if ((errno=pthread_create(&tid_ping,NULL,pthread1,&thread_params) != 0))
		ERROR(10, "Erro no pthread_create()!");
	if ((errno=pthread_create(&tid_pong,NULL,pthread2,&thread_params) != 0))
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

void *pthread1(void *arg) {
	// cast para o tipo de dados enviado pela 'main thread'
	thread_params_t *params = (thread_params_t *) arg;
			
	
		if ((errno = pthread_mutex_lock(params->ptr_mutex)) != 0){
			WARNING("pthread_mutex_lock() failed");
			return NULL;
		}
	
		while( params->statusStart != 1 ){
			if ((errno = pthread_cond_wait(params->ptr_cond,
						params->ptr_mutex)!=0)){   
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
				printf("HI1	\n");
		}
		printf("P1- A iniciar a lavagem");
		params->statusStart = 0;
		params->statusLavaExterior=1;

		while( params->statusLavaExterior != 1 ){
			if ((errno = pthread_cond_wait(params->ptr_cond,
						params->ptr_mutex)!=0)){   
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
		}
		printf("P1- Molhei e lavei os exteriores");
		params->statusLavaExterior=0;
		params->statusEnxaguaExterior=1;

		while( params->statusEnxaguaExterior != 1 ){
			if ((errno = pthread_cond_wait(params->ptr_cond,
						params->ptr_mutex)!=0)){   
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
		}
		printf("P1- Enxaguei e abrilhantei os exteriores");
		params->statusEnxaguaExterior=0;
		params->statusAspira=1;

		while( params->statusVidrosInterior != 1 ){
			if ((errno = pthread_cond_wait(params->ptr_cond,
						params->ptr_mutex)!=0)){   
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
		}
		printf("P1- Limpei os vidros no interior");
		params->statusVidrosInterior=0;
		params->statusPlastico=1;

		while( params->statusEspelhos != 1 ){
			if ((errno = pthread_cond_wait(params->ptr_cond,
						params->ptr_mutex)!=0)){   
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
		}
		printf("P1- Limpei os espelhos retrovisores");
		params->statusEspelhos=0;
		params->statusStart=1;

		if ((errno = pthread_cond_signal(params->ptr_cond)) != 0){
			WARNING("pthread_cond_signal() failed");
			return NULL;
		}

		// unlock
		if ((errno = pthread_mutex_unlock(params->ptr_mutex))!= 0){
			WARNING("pthread_mutex_unlock() failed");
			return NULL;
		}


	return NULL;
}

void *pthread2(void *arg) {
	// cast para o tipo de dados enviado pela 'main thread'
	thread_params_t *params = (thread_params_t *) arg;

	
		if ((errno = pthread_mutex_lock(params->ptr_mutex)) != 0){
			WARNING("pthread_mutex_lock() failed");
			return NULL;
		}

		while( params->statusAspira != 1 ){
			if ((errno = pthread_cond_wait(params->ptr_cond,
						params->ptr_mutex)!=0)){   
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
		}
		printf("Aspirei os interiores.");
		params->statusAspira = 0;
		params->statusEstofos=1;

		while( params->statusEstofos != 1 ){
			if ((errno = pthread_cond_wait(params->ptr_cond,
						params->ptr_mutex)!=0)){   
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
		}

		printf("Lavei os estofos.");
		params->statusEstofos = 0;
		params->statusVidrosInterior=1;

		while( params->statusPlastico != 1 ){
			if ((errno = pthread_cond_wait(params->ptr_cond,
						params->ptr_mutex)!=0)){   
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
		}
		printf("Abrilhantei os plásticos.");
		params->statusPlastico = 0;
		params->statusJantes=1;

		while( params->statusJantes != 1 ){
			if ((errno = pthread_cond_wait(params->ptr_cond,
						params->ptr_mutex)!=0)){   
				WARNING("pthread_cond_wait() failed");
				return NULL;
			}
		}
		printf("Abrilhantei as jantes e pneus.");
		params->statusJantes = 0;
		params->statusEspelhos = 1;
		
		if ((errno = pthread_cond_signal(params->ptr_cond)) != 0){
			WARNING("pthread_cond_signal() failed");
			return NULL;
		}

		// unlock
		if ((errno = pthread_mutex_unlock(params->ptr_mutex))!= 0){
			WARNING("pthread_mutex_unlock() failed");
			return NULL;
		}


	return NULL;
}
