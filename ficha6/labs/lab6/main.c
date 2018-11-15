#include "debug.h"
#include "common.h"
#include "server_opt.h"
#include <stdin.h>

#define C_ERR_CANT_CREATE_SOCKET (2)
#define MAX_PORT  ((1<<16)-1)
#define C_ERR_INVALID_PORT (1)

int main(int argc,char* argv[]){
  struct gengetopt_agrs_info args_info;
  if(cmdline_parser(argc,argv,&args_info)!=0){
    exit(ERR_ARGS);
  }

   int my_port=args_info.port_arg;
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
   //3:loop:recvfrom/sendto
   //4:close socket


}
