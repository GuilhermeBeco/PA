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

#define C_MAX_PORT (1<<16)

int tiny_http(int sock);
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

  int remote_port=check_port(args_info.port_arg);


  int tcp_client_socket;
  if ((tcp_client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    ERROR(41, "Can't create tcp_client_socket (IPv4): %s\n", strerror(errno));
  }
  // TCP IPv4: connect ao IP/porto do servidor
  struct sockaddr_in tcp_server_endpoint;
  memset(&tcp_server_endpoint, 0, sizeof(struct sockaddr_in));
  tcp_server_endpoint.sin_family = AF_INET;
  int ret_inet=inet_pton(AF_INET, args_info.ip_arg, &tcp_server_endpoint.sin_addr);
  if (ret_inet == -1){	// Server IP
    ERROR(42, "invalid family: %s\n", strerror(errno));
  }
  if(ret_inet==0){
    fprintf(stderr, "[ERROR] Invalid ipv4 %s\n", args_info.ip_arg);
    exit(EXIT_FAILURE);
  }
  tcp_server_endpoint.sin_port = htons(remote_port);						// Server port

  printf("a ligar ao servidor... "); fflush(stdout);
  int ret_connect=connect(tcp_client_socket, (struct sockaddr *) &tcp_server_endpoint, sizeof(struct sockaddr_in));
  if (ret_connect == -1){
    ERROR(EXIT_FAILURE, "Can't connect @tcp_server_endpoint: %s\n", strerror(errno));
  }
  printf("ok. \n");

  // TCP IPv4: informação sobre IP/porto do cliente (porto atribuído automaticamente)
  /*  struct sockaddr_in tcp_client_endpoint;
  socklen_t tcp_client_endpoint_length = sizeof(struct sockaddr_in);
  char tcp_client_string_ip[20];
  if (getsockname(tcp_client_socket, (struct sockaddr *)&tcp_client_endpoint, &tcp_client_endpoint_length) == -1){
  ERROR(44, "Can't connect @tcp_server_endpoint: %s\n", strerror(errno));
}
printf("cliente: %s@%d\n", inet_ntop(AF_INET, &tcp_client_endpoint.sin_addr, tcp_client_string_ip, sizeof(tcp_client_string_ip)), htons(tcp_client_endpoint.sin_port));
*/

tiny_http(tcp_client_socket);

close(tcp_client_socket);
cmdline_parser_free(&args_info);

return 0;
}

int tiny_http(int sock){
  char buff[2048];
  snprintf(buff, sizeof(buff), "GET / HTTP/1.1\r\nHost:www.google.pt\r\nUser-agent:ProgA\r\nConnection:close\r\n\r\n");
  ssize_t tcp_read_bytes, tcp_sent_bytes;
  tcp_sent_bytes=(tcp_sent_bytes = send(sock, buff, strlen(buff), 0));
  //char buffer[???];
  // TCP IPv4: "send" para o servidor
  printf("a enviar dados para o servidor... "); fflush(stdout);
  if (tcp_sent_bytes == -1){
    ERROR(EXIT_FAILURE, "Can't send to server: %s\n", strerror(errno));
  }
  printf("ok.  (%zu bytes enviados)\n", tcp_sent_bytes);



  printf("à espera de dados do servidor... ");
  fflush(stdout);
  while(1){

    tcp_read_bytes=recv(sock, buff, sizeof(buff)-1, 0);
    if ( tcp_read_bytes== -1){
      ERROR(47, "Can't recv from server: %s\n", strerror(errno));
    }else if(tcp_read_bytes==0)  {
      fprintf(stderr, "[CLIENT] server has closed connection\n");
      return 0;
    }else{
      printf("ok.  (%zu bytes recebidos)\n", tcp_read_bytes);
      buff[tcp_read_bytes]='\0';
      printf("%s\n",buff);
    }
  }

}
