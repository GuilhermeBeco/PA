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
#include "memory.h"
#include "debug.h"
#include "common.h"
#include "client_opt.h"
int16_t trata_send(char * request,int device);
void show_legenda();
void trata_recv(int16_t recv,int16_t send);
int main(int argc, char *argv[]){
    /* Estrutura gerada pelo utilitario gengetopt */
    struct gengetopt_args_info args_info;

    /* Processa os parametros da linha de comando */
    if (cmdline_parser (argc, argv, &args_info) != 0){
	    exit(ERR_ARGS);
    }
    char * request = args_info.request_arg;
    int device = args_info.device_arg;

    int udp_client_socket;
  	if ((udp_client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
  		ERROR(21, "Can't create udp_client_socket (IPv4): %s\n", strerror(errno));

  	// UDP IPv4: informação do servidor UDP
  	socklen_t udp_server_endpoint_length = sizeof(struct sockaddr_in);
  	struct sockaddr_in udp_server_endpoint;
  	memset(&udp_server_endpoint, 0, sizeof(struct sockaddr_in));
  	udp_server_endpoint.sin_family = AF_INET;
  	if (inet_pton(AF_INET, args_info.ip_arg, &udp_server_endpoint.sin_addr.s_addr) < 1)
  		ERROR(22, "Can't convert IP address (IPv4): %s\n", strerror(errno));
  	udp_server_endpoint.sin_port = htons(args_info.port_arg);

    ssize_t udp_read_bytes, udp_sent_bytes;
  	//char buffer[];
  	//...
    int16_t send=trata_send(request,device);
  	// UDP IPv4: "sendto" para o servidor
  	printf("a enviar dados para o servidor... "); fflush(stdout);
  	if ((udp_sent_bytes = sendto(udp_client_socket, send, sizeof(send), 0, (struct sockaddr *) &udp_server_endpoint, udp_server_endpoint_length)) == -1)
  		ERROR(24, "Can't sendto server: %s\n", strerror(errno));
  	printf("ok.  (%d bytes enviados)\n", (int)udp_sent_bytes);

  	// UDP IPv4: "recvfrom" do servidor (bloqueante)
    int16_t recv=0;
  	printf("à espera de dados do servidor... "); fflush(stdout);
  	if ((udp_read_bytes = recvfrom(udp_client_socket, recv, sizeof(recv), 0, (struct sockaddr *) &udp_server_endpoint, &udp_server_endpoint_length)) == -1)
  		ERROR(25, "Can't recvfrom server: %s\n", strerror(errno));
  	printf("ok.  (%d bytes recebidos)\n", (int)udp_read_bytes);
    trata_recv(recv,send);
    if (close(udp_client_socket) == -1)
      ERROR(23, "Can't close udp_client_socket (IPv4): %s\n", strerror(errno));

    cmdline_parser_free(&args_info);

    return 0;
}
int16_t trata_send(char * request,int device){
  int16_t send=0;
  if(strcmp(request, "status")==0){
  return send;
}else if(strcmp(request, "on")==0){
  send=10+device;
  return send;
}else if(strcmp(request, "off")==0){
  send=20+device;
  return send;
}else{
  return send;
}
}
void trata_recv(int16_t recv,int16_t send){
  int d=1;
  show_legenda();
  if(send==0){
    for(int i=0;i<8;i++){
      if(((recv<<i)&1)==1){
        printf("%d está ligado\n",d);
      }
      else{
        printf("%d está desligado\n",d);
      }
      d++;
    }
  }else if(send>10&&send<20){
    if(recv==0){
      printf("%d já se encontra aberto/ligado\n",send-10);
    }else if(recv==1){
      printf("%d foi aberto/ligado com sucesso\n",send-10);
    }else if(recv==2){
      printf("Dispositivo inva'lido\n");
    }
  }else if(send>20){
    if(recv==0){
      printf("%d já se encontra fechado/desligado\n",send-20);
    }else if(recv==1){
      printf("%d fechado/desligado com sucesso\n",send-20);
    }else if(recv==2){
      printf("Dispositivo inva'lido\n");
    }
  }

}
void show_legenda(){
  printf("1- Portão da garagem\n");
  printf("2- Iluminacao hall\n");
  printf("3- Iluminacao sala\n");
  printf("4- Iluminacao jardim\n");
  printf("5- Persiana 1\n");
  printf("6- Persiana 2\n");
  printf("7- Persiana 3\n");
  printf("8- Piso\n");
}
