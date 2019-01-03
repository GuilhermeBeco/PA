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
#include <time.h>
#include <errno.h>
#include "debug.h"
#include "common.h"
#include "server_opt.h"
#define C_MAX_PORT ((1<<16)-1)
char *cifrar(uint8_t chave,char buffer[]);

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

  // TCP IPv4: bind a IPv4/porto
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
  // TCP IPv4: "accept" 1 cliente (bloqueante)
  char tcp_client_string_ip[20];
  struct sockaddr_in tcp_client_endpoint;
  socklen_t tcp_client_endpoint_length = sizeof(struct sockaddr_in);
  int tcp_client_socket;
  printf("à espera da ligação do cliente... "); fflush(stdout);
  if ((tcp_client_socket = accept(tcp_server_socket, (struct sockaddr *) &tcp_client_endpoint, &tcp_client_endpoint_length)) == -1){
    ERROR(54, "Can't accept client: %s\n", strerror(errno));
  }
  printf("ok. \n");
  printf("cliente: %s@%d\n", inet_ntop(AF_INET, &tcp_client_endpoint.sin_addr, tcp_client_string_ip, sizeof(tcp_client_string_ip)), htons(tcp_client_endpoint.sin_port));


  ssize_t tcp_read_bytes, tcp_sent_bytes;
  char buffer[1024];
  //...
  int pedido=0;//0-cifrar  1-decifrar
  // TCP IPv4: "recv" do cliente (bloqueante)
  printf("à espera de dados do cliente... "); fflush(stdout);
  if ((tcp_read_bytes = recv(tcp_client_socket, &pedido, sizeof(pedido), 0)) == -1){
    ERROR(57, "Can't recv from client: %s\n", strerror(errno));
  }
  printf("ok.  (%d bytes recebidos)\n", (int)tcp_read_bytes);
  printf("%d\n",pedido);
  if(pedido==0){
    printf("à espera de dados do cliente... "); fflush(stdout);
    if ((tcp_read_bytes = recv(tcp_client_socket, buffer, sizeof(buffer)-1, 0)) == -1){
      ERROR(57, "Can't recv from client: %s\n", strerror(errno));
    }
  //  buffer[tcp_read_bytes]='\0';
    printf("ok.  (%d bytes recebidos)\n", (int)tcp_read_bytes);

    srand(time(NULL));   // Initialization, should only be called once.
    uint8_t r = (rand() % (127 - 1 )) + 1;
    char *mesg = malloc(strlen(buffer)-1);
    mesg = cifrar(r,buffer);

    printf("a enviar dados para o cliente... "); fflush(stdout);
    if ((tcp_sent_bytes = send(tcp_client_socket, &r, sizeof(r), 0)) == -1){
      ERROR(58, "Can't send to client: %s\n", strerror(errno));
    }
    printf("ok.  (%d bytes enviados)\n", (int)tcp_sent_bytes);

    printf("a enviar dados para o cliente... "); fflush(stdout);
    if ((tcp_sent_bytes = send(tcp_client_socket, mesg, strlen(mesg), 0)) == -1){
      ERROR(58, "Can't send to client: %s\n", strerror(errno));
    }
    printf("ok.  (%d bytes enviados)\n", (int)tcp_sent_bytes);
  }
  else{
    uint8_t chave=0;
    printf("à espera de dados do cliente... "); fflush(stdout);
    if ((tcp_read_bytes = recv(tcp_client_socket, &chave, sizeof(chave), 0)) == -1){
      ERROR(57, "Can't recv from client: %s\n", strerror(errno));
    }
    printf("ok.  (%d bytes recebidos)\n", (int)tcp_read_bytes);

    printf("à espera de dados do cliente... "); fflush(stdout);
    if ((tcp_read_bytes = recv(tcp_client_socket, buffer, sizeof(buffer)-1, 0)) == -1){
      ERROR(57, "Can't recv from client: %s\n", strerror(errno));
    }
    buffer[tcp_read_bytes]='\0';
  printf("%s\n",buffer);
    char *send_buf=malloc(strlen(buffer)-1);
    send_buf=cifrar(chave,buffer);
    printf("to send %s\n",send_buf );
    printf("a enviar dados para o cliente... "); fflush(stdout);
    if ((tcp_sent_bytes = send(tcp_client_socket, send_buf, strlen(send_buf), 0)) == -1){
      ERROR(58, "Can't send to client: %s\n", strerror(errno));
    }
    printf("ok.  (%d bytes enviados)\n", (int)tcp_sent_bytes);

  }

  // TCP IPv4: fecha socket (client)
  if (close(tcp_client_socket) == -1){
    ERROR(55, "Can't close tcp_client_socket (IPv4): %s\n", strerror(errno));
  }
  // TCP IPv4: fecha socket (server)
  if (close(tcp_server_socket) == -1){
    ERROR(56, "Can't close tcp_server_socket (IPv4): %s\n", strerror(errno));
    printf("ligação fechada. ok. \n");
  }

  cmdline_parser_free(&args_info);

  return 0;
}
char *cifrar(uint8_t chave,char buffer[]){
  //int len=strlen(buffer)-1;
  int len=0;
  int j=0;
  while(buffer[j]!='\0'){
    j++;
  }
  len=j;
  printf("len= %d\n",len);
  uint8_t aux_buff=0;
  uint8_t aux_convert=0;
  char *mesg=malloc(len+1);
  for(int i=0;i<len;i++){
    aux_buff=buffer[i];
    aux_convert=aux_buff;
    if(aux_buff>=32){
      aux_convert=aux_convert^chave;
    }
    mesg[i]=aux_convert;
  }
  return mesg;
}
