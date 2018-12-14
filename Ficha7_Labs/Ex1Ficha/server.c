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
#include <errno.h>

#include "debug.h"
#include "common.h"
#include "server_opt.h"
#define C_MAX_STATUS ((1<<16)-1)


void showLegenda();
int showStatus(int casa[8]);
int16_t check_status(int status){
  if (status<=0||status>C_MAX_STATUS) {
    fprintf(stderr, "ERROR: invalid status\n");
    exit(EXIT_FAILURE);
  }
  return status;
}
int16_t trata_rcv(int casa[8], int16_t recv){
  int c=1;
  int16_t ret=0;
  if(recv ==0 ){
    return recv;
  }
  else{
    if(((recv>>9)|0)==0){
      while (c!=9) {
        if(((recv>>c)|0)==1){
          c--;
          casa[c]=0;
          c++;
        }
        c++;
      }
    }else{
      while (c!=9) {
        if(((recv>>c)|0)==1){
          c--;
          casa[c]=1;
          c++;
        }
        c++;
      }
    }
    for (int i=0;i<8;i++){
      ret=(1 >> i) & casa[i];
    }
    return ret;
  }
}
  int check_port(int port){
    if (port<=0||port>C_MAX_STATUS) {
      fprintf(stderr, "ERROR: invalid port\n");
      exit(EXIT_FAILURE);
    }
    return port;
  }

  int main(int argc, char *argv[]){
    /* Estrutura gerada pelo utilitario gengetopt */
    struct gengetopt_args_info args_info;
    int casa[8];

    /* Processa os parametros da linha de comando */
    if (cmdline_parser (argc, argv, &args_info) != 0){
      exit(ERR_ARGS);
    }
    int16_t status = check_status(args_info.status_arg);
    int port = check_port(args_info.port_arg);

    for(int i =0;i<8;i++){
      casa[i]= (status >> i) & 1;
    }
    showStatus(casa);

    int udp_server_socket;
    if ((udp_server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
      ERROR(31, "Can't create udp_server_socket (IPv4): %s\n", strerror(errno));
    }
    // UDP IPv4: bind a IPv4/porto
    struct sockaddr_in udp_server_endpoint;
    memset(&udp_server_endpoint, 0, sizeof(struct sockaddr_in));
    udp_server_endpoint.sin_family = AF_INET;
    udp_server_endpoint.sin_addr.s_addr = htonl(INADDR_ANY);  	// Todas as interfaces de rede
    udp_server_endpoint.sin_port = htons(port);	// Server port
    if (bind(udp_server_socket, (struct sockaddr *) &udp_server_endpoint, sizeof(struct sockaddr_in)) == -1){
      ERROR(EXIT_FAILURE, "Can't bind @udp_server_endpoint info: %s\n", strerror(errno));
    }

    socklen_t udp_client_endpoint_length = sizeof(struct sockaddr_in);
    struct sockaddr_in udp_client_endpoint;
    ssize_t udp_read_bytes, udp_sent_bytes;
    //char buffer[];
    //...
    int16_t rcv;

    // UDP IPv4: "recvfrom" do cliente (bloqueante)
    printf("à espera de dados do cliente... "); fflush(stdout);
    if ((udp_read_bytes = recvfrom(udp_server_socket, rcv, sizeof(rcv), 0, (struct sockaddr *) &udp_client_endpoint, &udp_client_endpoint_length)) == -1)
    ERROR(34, "Can't recvfrom client: %s\n", strerror(errno));
    printf("ok.  (%d bytes recebidos)\n", (int)udp_read_bytes);
    int16_t send = trata_rcv(casa,rcv);
    // UDP IPv4: "sendto" para o cliente
    printf("a enviar dados para o cliente... "); fflush(stdout);
    if ((udp_sent_bytes = sendto(udp_server_socket, send, sizeof(send), 0, (struct sockaddr *) &udp_client_endpoint, udp_client_endpoint_length)) == -1)
    ERROR(35, "Can't sendto client: %s\n", strerror(errno));
    printf("ok.  (%d bytes enviados)\n", (int)udp_sent_bytes);

    if (close(udp_server_socket) == -1){
      ERROR(EXIT_FAILURE, "Can't close udp_server_socket (IPv4): %s\n", strerror(errno));
    }
    cmdline_parser_free(&args_info);

    return 0;
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

  int showStatus(int casa[8]){
    showLegenda();
    int c=1;
    for(int i =0;i<8;i++){
      if(casa[i]==1){
        printf("%d está ligado\n",c);
        c++;
      }else{
        printf("%d está desligado \n",c);
        c++;
      }
    }
  }
