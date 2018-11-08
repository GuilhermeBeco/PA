/**
* @file main.c
* @brief Description
* @date 2018-1-1
* @author name of author
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <arpa/inet.h>
#include "debug.h"
#include "memory.h"

int main(int argc, char *argv[]){
	/* Disable warnings */
	(void)argc; (void)argv;

	short s=0x1122;

	if(sizeof(short)!=2){
		fprintf(stderr,"Sizeof(short)=%zu",s);
		exit(1);
	}
	if(htons(s)==s){
		printf("BIGENDING,(no conversion)");
	}
	else{
		printf("LITTLE ENDING(conversion) %x ",htons(s));
	}



    /* Determinar endianess da máquina local */

	return 0;
}

