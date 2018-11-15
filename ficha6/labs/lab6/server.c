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

#define C_ERR_CANT_CREATE_SOCKET (2)
#define MAX_PORT  ((1<<16)-1)
#define C_ERR_INVALID_PORT (1)
#define C_ERR_CANT_CLOSE_SOCKET (3)
    #define C_EER_CANT_BIND (4)


int main(int argc, char *argv[]){
    /* Estrutura gerada pelo utilitario gengetopt */
    struct gengetopt_args_info args_info;

    /* Processa os parametros da linha de comando */
    if (cmdline_parser (argc, argv, &args_info) != 0){
        exit(ERR_ARGS);
    }
    int my_port=args_info.port_arg;
    printf("%d\n",my_port);
    if(my_port<=0||my_port> MAX_PORT){
      fprintf(stderr, "Invalid port given: %d (wanted:[1,%d])\n",my_port,MAX_PORT);
      exit(C_ERR_INVALID_PORT);
    }

    //1: create socket
    // UDP IPv4: cria socket

    int udp_server_socket;
   if ((udp_server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
     ERROR(C_ERR_CANT_CREATE_SOCKET, "Can't create udp_server_socket (IPv4)");
    //2:bind socket

    struct sockaddr_in udp_server_endpoint;
   memset(&udp_server_endpoint, 0, sizeof(struct sockaddr_in));
   udp_server_endpoint.sin_family = AF_INET;
   udp_server_endpoint.sin_addr.s_addr = htonl(INADDR_ANY);  	// Todas as interfaces de rede
   udp_server_endpoint.sin_port = htons(args.port_arg);	// Server port
   int ret_bind=bind(udp_server_socket,(struct *sockaddr)&udp_server_endpoint,sizeof(udp_server_endpoint));
   if(ret_bind==-1){
     fprintf(stderr, "ERROR: cannot bind at port %d:%s\n",my_port,strerror(errno));
     exit(C_ERR_CANT_BIND);
   }
   if (bind(udp_server_socket, (struct sockaddr *) &udp_server_endpoint, sizeof(struct sockaddr_in)) == -1)
     ERROR(32, "Can't bind @udp_server_endpoint info");

    //3:loop:recvfrom/sendto

    //4:close socket

    // liberta recurso: socket UDP IPv4
    if (close(udp_server_socket) == -1)
      ERROR(C_ERR_CANT_CLOSE_SOCKET, "Can't close udp_server_socket (IPv4)");
    cmdline_parser_free(&args_info);

    return 0;
}

int handle_client(int serv_sock){
  uint16_t clnt_value;
  struct sockaddr_in clnt_addr;
  sockelen_t clnt_addr_len=sizeof(clnt_addr);

  recvfrom(serv_sock,&clnt_value,sizeof(clnt_value),0,(struct sockaddr*)&clnt_addr,&clnt_addr_len);
}
