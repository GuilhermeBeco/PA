/**
* @file: client.c
* @date: 2016-11-17
* @author: autor
*/
#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include "memory.h"
#include "debug.h"
#include "common.h"
#include <string.h>
#include "client_opt.h"
#define C_MAX_STATUS ((1<<16)-1)

int check_port(int port){
  if (port<=0||port>C_MAX_STATUS ) {
    fprintf(stderr, "ERROR: invalid port\n");
    exit(EXIT_FAILURE);
  }
  return port;
}
int check_request(char request [7]){
  if(strcmp(request,"on")==0){
    return 1;
  }
  else if(strcmp(request, "off")==0){
    return 2;
  }
  else if (strcmp(request, "status")==0){
    return 0;
  }else{
    fprintf(stderr, "ERROR: invalid request %s\n");
    exit(EXIT_FAILURE);
  }

}
int check_device(int device,char request [7]){
  if(device!=0&& strcmp(request, "status")!=0){
    fprintf(stderr,"ERROR: request or device wrong %s\n");
    exit(EXIT_FAILURE);
  }
  return 1;
}
void showLegenda(){
  printf("1: portao\n");
  printf("2: hall\n");
  printf("3: sala\n");
  printf("4: jardim\n");
  printf("5: persiana 1\n");
  printf("6: persiana 2\n");
  printf("7: persiana 3\n");
  printf("8: piso\n");

}
int16_t build_responde(int device,int req){
  int16_t ret;
  if(req==0){
    ret=0;
  }else if(req==1){
    ret=(1>>9)|1;
    ret=(1>>device)|1;
  }
  else{
    ret=(1>>9)&0;
    ret=(1>>device)|1;
  }
  return ret;
}
void build_rcv(int16_t response,int req,int device){
int c=1;
showLegenda();
  if(req==0){
    for(int i =0;i<8;i++){
      if(response[i]==1){
        printf("%d está ligado\n",c);
      }
      else{
        printf("%d está desligado\n",c);
      }
      c++;
    }
  }
  else{
    if(response[device]==1){
      printf("%d está ligado\n",c);
    }
    else{
      printf("%d está desligado\n",c);
    }
  }
}


int main(int argc, char *argv[]){
  /* Estrutura gerada pelo utilitario gengetopt */
  struct gengetopt_args_info args_info;

  /* Processa os parametros da linha de comando */
  if (cmdline_parser (argc, argv, &args_info) != 0){
    exit(ERR_ARGS);
  }

  int port=check_port(args_info.port_arg);
  char *s = args_info.request_arg;
  int n = strlen(s);
  char request[n+1] ;
  strcpy(request, s) ;
  int req=check_request(request);
  int device=0;
  if(args_info.device_given){
    device=args_info.device_arg;
  }
  check_device(device, request);

  int16_t response=build_responde(device, req);

  int udp_client_socket;
  if ((udp_client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
    ERROR(EXIT_FAILURE, "Can't create udp_client_socket (IPv4): %s\n", strerror(errno));
  }
  /*struct timeval timeout;
  timeout.tv_sec;
  timeout.tv_usec;
  int ret =setsockopt(udp_client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
  if(ret==-1){
  ERROR(EXIT_FAILURE,"Cannot set socket");
}
*/
// UDP IPv4: informação do servidor UDP
socklen_t udp_server_endpoint_length = sizeof(struct sockaddr_in);
struct sockaddr_in udp_server_endpoint;
memset(&udp_server_endpoint, 0, sizeof(struct sockaddr_in));
udp_server_endpoint.sin_family = AF_INET;
int ret_inet=inet_pton(AF_INET,args_info.ip_arg, &udp_server_endpoint.sin_addr.s_addr);
if ( ret_inet==-1){
  ERROR(EXIT_FAILURE, "Wrong family: %s\n", strerror(errno));

}
if(ret_inet==0){
  ERROR(EXIT_FAILURE, "Can't convert IP address (IPv4): %s\n", strerror(errno));
}

udp_server_endpoint.sin_port = htons(port);

/*  int16_t request;
request=0;
*/
ssize_t udp_read_bytes, udp_sent_bytes;
//char buffer[];
// UDP IPv4: "sendto" para o servidor
printf("a enviar dados para o servidor... ");
fflush(stdout);
udp_sent_bytes = sendto(udp_client_socket, &response, sizeof(response), 0, (struct sockaddr *) &udp_server_endpoint, udp_server_endpoint_length);
if (udp_sent_bytes == -1){
  ERROR(EXIT_FAILURE, "Can't sendto server: %s\n", strerror(errno));
}
printf("ok.  (%d bytes enviados)\n", (int)udp_sent_bytes);

printf("à espera de dados do servidor... ");
fflush(stdout);
response=0;
udp_read_bytes = recvfrom(udp_client_socket, &response, sizeof(response), 0, (struct sockaddr *) &udp_server_endpoint, &udp_server_endpoint_length);
if (udp_read_bytes == -1){
  ERROR(EXIT_FAILURE, "Can't recvfrom server: %s\n", strerror(errno));
}
printf("ok.  (%d bytes recebidos)\n", (int)udp_read_bytes);
build_rcv(response, req, device);


if (close(udp_client_socket) == -1){
  ERROR(23, "Can't close udp_client_socket (IPv4): %s\n", strerror(errno));
}
cmdline_parser_free(&args_info);

return 0;
}
