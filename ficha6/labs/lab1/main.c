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
#include "args.h"
#define C_ERR_INET_PTON (1)
int main(int argc, char *argv[]){
    /* Receber endereço IPv4 em formato texto */
    struct gengetopt_args_info args;
    cmdline_parser(argc, argv, &args);

    char dst[4];
    printf("Given address: '%s'\n",args.address_arg);
    if(inet_pton(AF_INET,args.address_arg,dst)==0){
        if(errno!=0){
        ERROR(C_ERR_INET_PTON,"Cannot convert IPv4 '%s'",args.address_arg);
        }
        else{
            fprintf(stderr,"Cannot convert IPv4 '%s'\n",args.address_arg);
            exit(C_ERR_INET_PTON);
        }
    }
    for(size_t i=0;i<sizeof(dst);i++){
        printf("[%02ld]:%x\n",i,dst[i]);
    }

    /* Libertar memória alocada pelo gengetopt */
    cmdline_parser_free(&args);
	return 0;
}

