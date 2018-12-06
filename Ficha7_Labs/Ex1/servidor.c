#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>


#include "debug.h"
#include "comum.h"
#include "servidor_opt.h"

#define C_FIM "fim"
void processaCliente(int fd);
int check_port(int port){
  if (port<=0||port>C_MAX_PORT) {
    fprintf(stderr, "ERROR: invalid port\n");
    exit(EXIT_FAILURE);
  }
  return port;
}
int main(int argc, char *argv[])
{
  /* Processa os parâmetros da linha de comando */
  struct gengetopt_args_info args_info;
  if (cmdline_parser(argc, argv, &args_info) != 0)
    ERROR(C_ERRO_CMDLINE, "cmdline_parser");

  int my_port=check_port(args_info.porto_arg);

  int tcp_server_socket;
  if ((tcp_server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    ERROR(51, "Can't create tcp_server_socket (IPv4): %s\n", strerror(errno));

  struct sockaddr_in tcp_server_endpoint;
  memset(&tcp_server_endpoint, 0, sizeof(struct sockaddr_in));
  tcp_server_endpoint.sin_family = AF_INET;
  tcp_server_endpoint.sin_addr.s_addr = htonl(INADDR_ANY); 		// Todas as interfaces de rede
  tcp_server_endpoint.sin_port = htons(my_port);		// Server port
  if (bind(tcp_server_socket, (struct sockaddr *) &tcp_server_endpoint, sizeof(struct sockaddr_in)) == -1)
    ERROR(52, "Can't bind @tcp_server_endpoint: %s\n", strerror(errno));

  int tcp_max_simultaneous_clients = 1;
  if (listen(tcp_server_socket, tcp_max_simultaneous_clients)  == -1)
    ERROR(53, "Can't listen for %d clients: %s\n", tcp_max_simultaneous_clients, strerror(errno));

  char tcp_client_string_ip[20];
  struct sockaddr_in tcp_client_endpoint;
  while (1) {
  socklen_t tcp_client_endpoint_length = sizeof(struct sockaddr_in);
  int tcp_client_socket;
  printf("à espera da ligação do cliente... ");
  fflush(stdout);
  if ((tcp_client_socket = accept(tcp_server_socket, (struct sockaddr *) &tcp_client_endpoint, &tcp_client_endpoint_length)) == -1){
    if(errno == EINTR){
      fprintf(stderr, "[Server] EINTR detected. continuing ..\n");
      }
     else {
        ERROR(EXIT_FAILURE,"Server cannot accept");
      }
    }
  printf("ok. \n");
  inet_ntop(AF_INET, &tcp_client_endpoint.sin_addr,
  tcp_client_string_ip, sizeof(tcp_client_string_ip)), htons(tcp_client_endpoint.sin_port);
  printf("cliente: %s@%d\n",tcp_client_string_ip);
  processaCliente(tcp_client_socket);
  close(tcp_client_socket);
  }
  close(tcp_server_socket);
  cmdline_parser_free(&args_info);
  return 0;
}



void processaCliente(int fd){
  ssize_t tcp_read_bytes,tcp_sent_bytes;
  char echo_buff[1024];
  do {
    /* recebe dados do cliente - chamada bloqueante */
    tcp_read_bytes = recv(fd, echo_buff, sizeof(echo_buff)-1, 0);
    if( tcp_read_bytes== -1 ){
      if( errno == EINTR ){
        printf("recv: got EINTR - continuing\n");
        continue;
      }else{
        ERROR(C_ERRO_RECV, "recv");
        return ;
      }
    }
    echo_buff[tcp_read_bytes-1]='\0';
    if( tcp_read_bytes == 0 ){
      fprintf(stderr,"Got 0 bytes - connection close by peer\n");
      return ;
    }
    printf("echo_buff = %s,C_FIM=%s\n",echo_buff,C_FIM);

    /* envia resposta ao cliente */
    if (send(fd, echo_buff, strlen(echo_buff)+1, 0) == -1)
    ERROR(C_ERRO_SEND, "send");

  } while (strcasecmp(echo_buff, C_FIM)!=0);
}
