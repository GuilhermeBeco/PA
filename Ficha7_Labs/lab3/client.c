#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "../debug/debug.h"
#include "../cmdline/client_cmdline.h"

#define MAX_PORT ((1<<16)-1)
int main(int argc, char *argv[])
{
	struct gengetopt_args_info args;

	// cmdline_parser: deve ser a primeira linha de código no main
	if( cmdline_parser(argc, argv, &args) ){
		fprintf(stderr, "Erro na exec do cmdline_parser\n");
		exit(1)	;
	}

	int tcp_client_socket;
	if ((tcp_client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	ERROR(41, "Can't create tcp_client_socket (IPv4): %s\n", strerror(errno));

	int port = args.porto_arg;
	if(port<=0 || port>MAX_PORT)
	fprintf(stderr, "post %d out of range [1-%d]\n",port,MAX_PORT );
	char * ip_ptr=args.ip_arg;


	// TCP IPv4: connect ao IP/porto do servidor
	struct sockaddr_in tcp_server_endpoint;
	memset(&tcp_server_endpoint, 0, sizeof(struct sockaddr_in));
	tcp_server_endpoint.sin_family = AF_INET;
	tcp_server_endpoint.sin_port = htons(port);
	int ret_inet=inet_pton(AF_INET, ip_ptr, &tcp_server_endpoint.sin_addr.s_addr);
	if (ret_inet < 0){	// Server IP
		ERROR(42, "Can't convert IP address: %s\n", strerror(errno));
	}
	else if (ret_inet==0){
		fprintf(stderr, "Cannot convert ip %s\n",ip_ptr);
		exit(32);
	}
	int ret_connect = connect(tcp_client_socket,(struct sockaddr*)&tcp_server_endpoint,sizeof(tcp_server_endpoint));
	if(ret_connect==-1){
		ERROR(54,"Cannot connect to server");
	}







	// Server port
	/*
	printf("a ligar ao servidor... "); fflush(stdout);
	if (connect(tcp_client_socket, (struct sockaddr *) &tcp_server_endpoint, sizeof(struct sockaddr_in)) == -1)
	ERROR(43, "Can't connect @tcp_server_endpoint: %s\n", strerror(errno));
	printf("ok. \n");

	*/

	// libertar recurso (cmdline_parser)
	cmdline_parser_free(&args);

	exit(0);
}
