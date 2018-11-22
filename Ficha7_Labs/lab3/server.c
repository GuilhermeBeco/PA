#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../debug/debug.h"
#include "../cmdline/server_cmdline.h"

#define MAX_PORT ((1<<16)-1)


int main(int argc, char *argv[])
{
	struct gengetopt_args_info args;

	// cmdline_parser: deve ser a primeira linha de código no main
	if( cmdline_parser(argc, argv, &args) )
	fprintf(stderr, "cannot cmdline_parser\n");

	int port = args.porto_arg;
	if(port<=0 || port>MAX_PORT)
	fprintf(stderr, "post %d out of range [1-%d]\n",port,MAX_PORT );

	int tcp_server_socket;
	if ((tcp_server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	ERROR(51, "Can't create tcp_server_socket (IPv4): %s\n", strerror(errno));


	struct sockaddr_in tcp_server_endpoint;
	memset(&tcp_server_endpoint, 0, sizeof(struct sockaddr_in));
	tcp_server_endpoint.sin_family = AF_INET;
	tcp_server_endpoint.sin_addr.s_addr = htonl(INADDR_ANY); 		// Todas as interfaces de rede
	tcp_server_endpoint.sin_port = htons(port);		// Server port
	if (bind(tcp_server_socket, (struct sockaddr *) &tcp_server_endpoint, sizeof(struct sockaddr_in)) == -1)
	ERROR(52, "Can't bind @tcp_server_endpoint: %s\n", strerror(errno));

	int backlog=2;
	if (listen(tcp_server_socket, backlog)  == -1)
	ERROR(53, "Can't listen for %d clients: %s\n", backlog, strerror(errno));


	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_len;
	int clnt_sock;
	const char * ret_ptr;
	char clnt_ip_s [128];
	while (1) {

		clnt_addr_len=sizeof(clnt_addr);
		clnt_sock=accept(tcp_server_socket,(struct sockaddr*)&clnt_addr.sin_addr.s_addr,&clnt_addr_len);
		if(clnt_sock==-1){
			ERROR(54,"Cannot accept");
		}


	ret_ptr=inet_ntop(AF_INET,&clnt_addr,clnt_ip_s,clnt_addr_len);
	if(ret_ptr==NULL){
		fprintf(stderr, "Cannot inet_ntop%s\n",strerror(errno));
	}

	printf("Client = %s port=%u\n",clnt_ip_s,ntohs(clnt_addr.sin_port) );

char S[128];
ssize_t ret_recv;
ret_recv=recv(clnt_sock,S,sizeof(S),0);
if(ret_recv==-1){
	fprintf(stderr, "Cannot receive %s\n", strerror(errno));
	continue;
}
if(ret_recv==0){
	fprintf(stderr, "Remote peer has closed\n");
	close(clnt_sock);
	continue;
}
printf("Num to process: %s\n",S);
uint16_t number =atoi(S);
number=htons(number);
ssize_t ret_send =send(clnt_sock,&number,sizeof(number),0);
if(ret_send==-1){
	fprintf(stderr, "Cannot send: %s\n",strerror(errno));
	close(clnt_sock);
	continue;
}
}
	close(tcp_server_socket);
	// libertar recurso (cmdline_parser)
	cmdline_parser_free(&args);

	exit(0);
}
