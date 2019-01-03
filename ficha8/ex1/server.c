/**
 * @file: server.c
 * @date: 2016-11-17
 * @author: autor
 */
#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "debug.h"
#include "common.h"
#include "server_opt.h"


int main(int argc, char *argv[]){
    /* Estrutura gerada pelo utilitario gengetopt */
    struct gengetopt_args_info args_info;

    /* Processa os parametros da linha de comando */
    if (cmdline_parser (argc, argv, &args_info) != 0){
        exit(ERR_ARGS);
    }
     
    int tcp_server_socket;
  	if ((tcp_server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  		ERROR(51, "Can't create tcp_server_socket (IPv4): %s\n", strerror(errno));

    int udp_server_socket;
    if ((udp_server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
      ERROR(31, "Can't create udp_server_socket (IPv4): %s\n", strerror(errno));


    cmdline_parser_free(&args_info);

    return 0;
}
