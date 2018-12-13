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
#include "client_opt.h"

int main(int argc, char *argv[]){
    /* Estrutura gerada pelo utilitario gengetopt */
    struct gengetopt_args_info args_info;

    /* Processa os parametros da linha de comando */
    if (cmdline_parser (argc, argv, &args_info) != 0){
        exit(ERR_ARGS);
    }

    int port=args_info.port_arg;


    int udp_client_socket;
    if ((udp_client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        ERROR(EXIT_FAILURE, "Can't create udp_client_socket (IPv4): %s\n", strerror(errno));
    }
    struct timeval timeout;
    timeout.tv_sec;
    timeout.tv_usec;
    int ret =setsockopt(udp_client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    if(ret==-1){
        ERROR(EXIT_FAILURE,"Cannot set socket");
    }

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

    int16_t request;
    request=0;

    ssize_t udp_read_bytes, udp_sent_bytes;
	//char buffer[];
	// UDP IPv4: "sendto" para o servidor
	printf("a enviar dados para o servidor... ");
    fflush(stdout);
    udp_sent_bytes = sendto(udp_client_socket, &request, sizeof(request), 0, (struct sockaddr *) &udp_server_endpoint, udp_server_endpoint_length);
	if (udp_sent_bytes == -1){
		ERROR(EXIT_FAILURE, "Can't sendto server: %s\n", strerror(errno));
    }
	printf("ok.  (%d bytes enviados)\n", (int)udp_sent_bytes);

    printf("à espera de dados do servidor... ");
    fflush(stdout);
    int16_t response;
    udp_read_bytes = recvfrom(udp_client_socket, &response, sizeof(response), 0, (struct sockaddr *) &udp_server_endpoint, &udp_server_endpoint_length);
	if (udp_read_bytes == -1){
		ERROR(EXIT_FAILURE, "Can't recvfrom server: %s\n", strerror(errno));
    }
	printf("ok.  (%d bytes recebidos)\n", (int)udp_read_bytes);


    if (close(udp_client_socket) == -1){
        ERROR(23, "Can't close udp_client_socket (IPv4): %s\n", strerror(errno));
    }
    cmdline_parser_free(&args_info);

    return 0;
}
