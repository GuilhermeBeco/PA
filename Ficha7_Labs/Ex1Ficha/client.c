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

#include "memory.h"
#include "debug.h"
#include "common.h"
#include "client_opt.h"
int16_t send=0;
int check_port(int port){
	if( port <= 0 || port >= C_MAX_PORT ){
		fprintf(stderr,"ERROR: invalid port '%d'. Must be within"
				"[1,%d]\n", port, C_MAX_PORT-1);
		exit(EXIT_FAILURE);
	}
	return port;
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
void trata_send(char request[], int device){
    if(strcmp(request,"status")==0){
      send=0;
    }
    else if(strcmp(request,"on")==0){
      send+=10;
      send+=device;
    }
    else if(strcmp(request,"off")==0){
      send+=20;
      send+=device;
    }
}
void trata_recv(int recv_spec,int16_t recv_all,int flag,int device){
int c=1;
show_legenda();
  if(flag==0){
    for(int i=0;i<8;i++){
      if(((recv_all<<i)&1)==1){
        printf("%d está ligado\n",c);
      }
      else{
        printf("%d está desligado\n",c);
      }
      c++;
    }
  }else{
      if(recv_spec==0){
        printf("%d já se encontra ligado\n",device);
      }
      else if(recv_spec==1){
        printf("%d foi ligado com sucesso\n",device);
      }
      else{
        printf("Disposito invalido\n");
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
    int remote_port = check_port(args_info.port_arg);
    int device=args_info.device_arg;
    char request [] = args_info.request_arg;
    int flag=0;
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
    udp_server_endpoint.sin_port = htons(remote_port);

    ssize_t udp_read_bytes, udp_sent_bytes;
    //char buffer[];
    //...
    trata_send(request, device);
    // UDP IPv4: "sendto" para o servidor
    printf("a enviar dados para o servidor... "); fflush(stdout);
    if ((udp_sent_bytes = sendto(udp_client_socket, send, sizeof(send), 0, (struct sockaddr *) &udp_server_endpoint, udp_server_endpoint_length)) == -1)
      ERROR(24, "Can't sendto server: %s\n", strerror(errno));
    printf("ok.  (%d bytes enviados)\n", (int)udp_sent_bytes);
    int16_t recv_all=0;
    int recv_spec=0;
    // UDP IPv4: "recvfrom" do servidor (bloqueante)
    if(strcmp(request,"status")!=0){
    printf("à espera de dados do servidor... "); fflush(stdout);
    if ((udp_read_bytes = recvfrom(udp_client_socket, recv_all, sizeof(recv_all), 0, (struct sockaddr *) &udp_server_endpoint, &udp_server_endpoint_length)) == -1)
      ERROR(25, "Can't recvfrom server: %s\n", strerror(errno));
    printf("ok.  (%d bytes recebidos)\n", (int)udp_read_bytes);
    flag=1;
  }else{
    if ((udp_read_bytes = recvfrom(udp_client_socket, recv_spec, sizeof(recv_spec), 0, (struct sockaddr *) &udp_server_endpoint, &udp_server_endpoint_length)) == -1)
      ERROR(25, "Can't recvfrom server: %s\n", strerror(errno));
    printf("ok.  (%d bytes recebidos)\n", (int)udp_read_bytes);
    flag=0;
  }
  trata_recv(recv_spec,recv_all,flag);


    if (close(udp_client_socket) == -1)
  		ERROR(23, "Can't close udp_client_socket (IPv4): %s\n", strerror(errno));

    cmdline_parser_free(&args_info);

    return 0;
}
