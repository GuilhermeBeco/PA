/**
 * @file: client.c
 * @date: 2018-12-13 11h23:20 
 * @author: Patricio R. Domingues
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

#define C_MAX_PORT	(1<<16) // ==> common.h
int check_port(int port){
	if( port <= 0 || port >= C_MAX_PORT ){
		fprintf(stderr,"ERROR: invalid port '%d'. Must be within"
				"[1,%d]\n", port, C_MAX_PORT-1);              
		exit(EXIT_FAILURE);
	}
	return port;
}
int tiny_http(int sock);

int main(int argc, char *argv[]){
	struct gengetopt_args_info args;

	/* Processa os parametros da linha de comando */
	if (cmdline_parser (argc, argv, &args) != 0){
		exit(ERR_ARGS);
	}
	int remote_port = check_port(args.port_arg);
	// TCP IPv4: cria socket
	int tcp_client_socket;
	tcp_client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if( tcp_client_socket == -1 ){
		ERROR(EXIT_FAILURE, "Can't create tcp_client_socket (IPv4)");
	}

	// TCP IPv4: connect ao IP/porto do servidor
	struct sockaddr_in tcp_server_endpoint;
	memset(&tcp_server_endpoint, 0, sizeof(struct sockaddr_in));
	tcp_server_endpoint.sin_family = AF_INET;
	int inet_ret = inet_pton(AF_INET,args.ip_arg,
			&tcp_server_endpoint.sin_addr);
	if( inet_ret == -1 ){
		ERROR(EXIT_FAILURE,"Invalid family '%s'", args.ip_arg);
	}else if (inet_ret == 0){
		fprintf(stderr,"[ERROR] Invalid IPv4 '%s'\n", args.ip_arg);
		exit(EXIT_FAILURE);
	}
	tcp_server_endpoint.sin_port = htons(remote_port);
	printf("[CLIENT] Attempting to connect to server\n");

	int connect_ret = connect(tcp_client_socket, 
	  (struct sockaddr *) &tcp_server_endpoint, sizeof(struct sockaddr_in));
	if( connect_ret == -1){
		ERROR(EXIT_FAILURE, "Can't connect @tcp_server_endpoint");
	}

	tiny_http(tcp_client_socket);

	close(tcp_client_socket);
	cmdline_parser_free(&args);

	return 0;
}

int tiny_http(int sock){
	char buff[256];
	snprintf(buff,sizeof(buff),
		"GET / HTTP/1.1\r\n"
	 	"Host:www.google.com\r\nUser-agent:ProgA\r\n"
	 	"Connection:close\r\n\r\n");
//	 	"Host:www.ipleiria.pt\r\nUser-agent:ProgA\r\n"
	ssize_t tcp_read_bytes, tcp_sent_bytes;
	tcp_sent_bytes = send(sock, buff, strlen(buff), 0);
	if( tcp_sent_bytes == -1 ) {
		ERROR(EXIT_FAILURE,"Cannot send to server '%s'", buff);
	}
	printf("[CLIENT] %zu bytes sent to server\n", tcp_sent_bytes);
	printf("[CLIENT] Waiting for server response\n");


	while(1){
		tcp_read_bytes = recv(sock, buff, sizeof(buff)-1, 0);
		if( tcp_read_bytes == -1 ){
			ERROR(EXIT_FAILURE,"Cannot recv");
		}else if( tcp_read_bytes == 0 ){
			fprintf(stderr,
			       "[CLIENT] Server has closed the connection\n");
			return 1;
		}else{
			printf("[CLIENT] recv. %zu bytes from server\n",
								tcp_read_bytes);
			buff[tcp_read_bytes] = '\0';
			printf("[CLIENT]: '%s'\n", buff);
		}
	}
	return 1;
}//tiny_http
