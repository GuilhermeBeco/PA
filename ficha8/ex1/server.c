/**
* @file: server.c
* @date: 2016-11-17
* @author: autor
*/
#include <stdio.h>
#include <sys/types.h>
#include <assert.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include "debug.h"
#include "common.h"
#include "server_opt.h"
/* According to POSIX.1-2001, POSIX.1-2008 */
#include <sys/select.h>
/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#define MAX(x,y) (x)>(y)?(x):(y)
int check_port(int port){
   if (port<=0||port>C_MAX_PORT) {
      fprintf(stderr, "ERROR: invalid port\n");
      exit(EXIT_FAILURE);
   }
   return port;
}


int main(int argc, char *argv[]){
   /* Estrutura gerada pelo utilitario gengetopt */
   struct gengetopt_args_info args_info;

   /* Processa os parametros da linha de comando */
   if (cmdline_parser (argc, argv, &args_info) != 0){
      exit(ERR_ARGS);
   }
   int port=check_port(args_info.port_arg);

   int tcp_server_socket;
   if ((tcp_server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
      ERROR(51, "Can't create tcp_server_socket (IPv4): %s\n", strerror(errno));
   }
   int udp_server_socket;
   if ((udp_server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
      ERROR(31, "Can't create udp_server_socket (IPv4): %s\n", strerror(errno));
   }
   struct sockaddr_in tcp_server_endpoint;
   memset(&tcp_server_endpoint, 0, sizeof(struct sockaddr_in));
   tcp_server_endpoint.sin_family = AF_INET;
   tcp_server_endpoint.sin_addr.s_addr = htonl(INADDR_ANY); 		// Todas as interfaces de rede
   tcp_server_endpoint.sin_port = htons(port);		// Server port
   if (bind(tcp_server_socket, (struct sockaddr *) &tcp_server_endpoint, sizeof(struct sockaddr_in)) == -1){
      ERROR(52, "Can't bind @tcp_server_endpoint: %s\n", strerror(errno));
   }

   // TCP IPv4: "listen" por clientes
   int tcp_max_simultaneous_clients = 1;
   if (listen(tcp_server_socket, tcp_max_simultaneous_clients)  == -1){
      ERROR(53, "Can't listen for %d clients: %s\n", tcp_max_simultaneous_clients, strerror(errno));
   }
   //////////////////////////////////////////////

   struct sockaddr_in udp_server_endpoint;
   memset(&udp_server_endpoint, 0, sizeof(struct sockaddr_in));
   udp_server_endpoint.sin_family = AF_INET;
   udp_server_endpoint.sin_addr.s_addr = htonl(INADDR_ANY);  	// Todas as interfaces de rede
   udp_server_endpoint.sin_port = htons(port);	// Server port
   if (bind(udp_server_socket, (struct sockaddr *) &udp_server_endpoint, sizeof(struct sockaddr_in)) == -1){
      ERROR(32, "Can't bind @udp_server_endpoint info: %s\n", strerror(errno));
   }

   my_select(tcp_server_socket,udp_server_socket);
   if (close(tcp_server_socket) == -1){
      ERROR(56, "Can't close tcp_server_socket (IPv4): %s\n", strerror(errno));
   }
   printf("ligação fechada. ok. \n");

   if (close(udp_server_socket) == -1){
      ERROR(33, "Can't close udp_server_socket (IPv4): %s\n", strerror(errno));
   }
   cmdline_parser_free(&args_info);

   return 0;
}
int my_select(int tcp_server_socket,int udp_server_socket){
   assert(tcp_server_socket>0);
   assert(udp_server_socket>0);
   int max = MAX(udp_server_socket,tcp_server_socket);
   fd_set readset;
   struct timeval timeout;

   while(1){
      FD_ZERO(&readset);
      // ativa monitorização para os descritores sock_UDP, sock_TCP1 e sock_TCP2.
      FD_SET(udp_server_socket,&readset);
      FD_SET(tcp_server_socket,&readset);
      timeout.tv_sec = 30;
      timeout.tv_usec = 0;
      int ret_select = select(max+1, &readset, NULL, NULL, &timeout);
      if(ret_select == -1 ){
         ERROR(1,"Cannot select");
      }
      if(ret_select == 0 ){
         printf("Timeout has expired\n");
         continue;
      }
      if( FD_ISSET(udp_server_socket, &readset) ){
         printf("Event in sock_UDP\n");
         continue;
      }
      if( FD_ISSET(tcp_server_socket,&readset) ){
         printf("Event in sock_TCP1\n");
         continue;
      }
   }// while (1);

}
