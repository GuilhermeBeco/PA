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
#define C_MAX_PORT ((1<<16)-1)

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
    int port = check_port(args_info.port_arg);
    uint8_t chave=0;
    if(args_info.key_given){
      chave=args_info.key_arg;
    }
    char *mesg = malloc(1024);
    mesg=args_info.message_arg;
    int tcp_client_socket;
  	if ((tcp_client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  		ERROR(41, "Can't create tcp_client_socket (IPv4): %s\n", strerror(errno));

  	// TCP IPv4: connect ao IP/porto do servidor
  	struct sockaddr_in tcp_server_endpoint;
  	memset(&tcp_server_endpoint, 0, sizeof(struct sockaddr_in));
  	tcp_server_endpoint.sin_family = AF_INET;
      if (inet_pton(AF_INET, args_info.ip_arg, &tcp_server_endpoint.sin_addr) <= 0)	// Server IP
  		ERROR(42, "Can't convert IP address: %s\n", strerror(errno));
  	tcp_server_endpoint.sin_port = htons(port);						// Server port

  	printf("a ligar ao servidor... "); fflush(stdout);
  	if (connect(tcp_client_socket, (struct sockaddr *) &tcp_server_endpoint, sizeof(struct sockaddr_in)) == -1)
  		ERROR(43, "Can't connect @tcp_server_endpoint: %s\n", strerror(errno));
  	printf("ok. \n");

  	// TCP IPv4: informação sobre IP/porto do cliente (porto atribuído automaticamente)
  	struct sockaddr_in tcp_client_endpoint;
  	socklen_t tcp_client_endpoint_length = sizeof(struct sockaddr_in);
  	char tcp_client_string_ip[20];
  	if (getsockname(tcp_client_socket, (struct sockaddr *)&tcp_client_endpoint, &tcp_client_endpoint_length) == -1)
  		ERROR(44, "Can't connect @tcp_server_endpoint: %s\n", strerror(errno));
  	printf("cliente: %s@%d\n", inet_ntop(AF_INET, &tcp_client_endpoint.sin_addr, tcp_client_string_ip, sizeof(tcp_client_string_ip)), htons(tcp_client_endpoint.sin_port));

    ssize_t tcp_read_bytes, tcp_sent_bytes;
    //char buffer[???];
    //...
    char * newMess=malloc(strlen(mesg));
    int pedido=0;
    if(chave==0){
    // TCP IPv4: "send" para o servidor
    printf("a enviar dados para o servidor... "); fflush(stdout);
    if ((tcp_sent_bytes = send(tcp_client_socket, &pedido, sizeof(pedido), 0)) == -1)
      ERROR(46, "Can't send to server: %s\n", strerror(errno));
    printf("ok.  (%d bytes enviados)\n", (int)tcp_sent_bytes);

    printf("a enviar dados para o servidor... "); fflush(stdout);
    if ((tcp_sent_bytes = send(tcp_client_socket, mesg, strlen(mesg), 0)) == -1)
      ERROR(46, "Can't send to server: %s\n", strerror(errno));
    printf("ok.  (%d bytes enviados)\n", (int)tcp_sent_bytes);

    uint8_t chave_recv=0;

    // TCP IPv4: "recv" do servidor (bloqueante)
    printf("à espera de dados do servidor... "); fflush(stdout);
    if ((tcp_read_bytes = recv(tcp_client_socket, &chave_recv, sizeof(chave_recv), 0)) == -1)
      ERROR(47, "Can't recv from server: %s\n", strerror(errno));
    printf("ok.  (%d bytes recebidos)\n", (int)tcp_read_bytes);

    printf("à espera de dados do servidor... "); fflush(stdout);
    if ((tcp_read_bytes = recv(tcp_client_socket, newMess, strlen(mesg), 0)) == -1) //strlen(mesg) pois o sizeof da newMess estava menor que o devido
      ERROR(47, "Can't recv from server: %s\n", strerror(errno));
    printf("ok.  (%d bytes recebidos)\n", (int)tcp_read_bytes);
    printf(">> KEY: %d\n",chave_recv );
    printf(">> MESSAGE: %s\n",newMess );
  }else{
    pedido=1;

    printf("a enviar dados para o servidor... "); fflush(stdout);
    if ((tcp_sent_bytes = send(tcp_client_socket, &pedido, sizeof(pedido), 0)) == -1)
      ERROR(46, "Can't send to server: %s\n", strerror(errno));
    printf("ok.  (%d bytes enviados)\n", (int)tcp_sent_bytes);

    printf("a enviar dados para o servidor... "); fflush(stdout);
    if ((tcp_sent_bytes = send(tcp_client_socket, &chave, sizeof(chave), 0)) == -1)
      ERROR(46, "Can't send to server: %s\n", strerror(errno));
    printf("ok.  (%d bytes enviados)\n", (int)tcp_sent_bytes);

    printf("a enviar dados para o servidor... "); fflush(stdout);
    if ((tcp_sent_bytes = send(tcp_client_socket, mesg, strlen(mesg), 0)) == -1)
      ERROR(46, "Can't send to server: %s\n", strerror(errno));
    printf("ok.  (%d bytes enviados)\n", (int)tcp_sent_bytes);

    printf("à espera de dados do servidor... "); fflush(stdout);
    if ((tcp_read_bytes = recv(tcp_client_socket, newMess, strlen(mesg), 0)) == -1)//strlen(mesg) pois o sizeof da newMess estava menor que o devido
      ERROR(47, "Can't recv from server: %s\n", strerror(errno));
    printf("ok.  (%d bytes recebidos)\n", (int)tcp_read_bytes);

    printf(">> MESSAGE: %s\n",newMess );
  }

    // TCP IPv4: fecha socket (client)
    if (close(tcp_client_socket) == -1)
      ERROR(45, "Can't close tcp_client_socket (IPv4): %s\n", strerror(errno));
    printf("ligação fechada. ok. \n");

    /*
     * Inserir codigo do cliente
     */
     free(mesg);
    cmdline_parser_free(&args_info);

    return 0;
}
