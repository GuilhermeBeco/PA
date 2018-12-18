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
#include <unistd.h>
#include <errno.h>
#include "debug.h"
#include "common.h"
#include "server_opt.h"

int show_status(uint16_t status);
void trata_send(int16_t recv,int16_t  domus);
void trata_liga(int16_t recv,int16_t  domus);
void trata_desliga(int16_t recv,int16_t  domus);
int show_status(uint16_t status);

#define C_MAX_PORT	(1<<16) // ==> common.h
int send_status_desligado=10;
int send_status_ligado=10;
int16_t send_status_all;

int check_port(int port){
	if( port <= 0 || port >= C_MAX_PORT ){
		fprintf(stderr,"ERROR: invalid port '%d'. Must be within"
				"[1,%d]\n", port, C_MAX_PORT-1);
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


	int status = args_info.status_arg;
	if( status < 0 || status >= (1<<16) ){
		fprintf(stderr,"[ERROR] Invalid status '0x%x' (out of range)\n",
				status);
		exit(EXIT_FAILURE);
	}
	int16_t domus_status = (int16_t) status;
	 //DEBUG
	show_status(domus_status);

	int remote_port = check_port(args_info.port_arg);

	int udp_server_socket;
	udp_server_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if( udp_server_socket == -1 ){
		ERROR(EXIT_FAILURE, "Can't create udp_server_socket (IPv4)");
	}

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET; //same as PF_INET
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // network byte address
	serv_addr.sin_port = htons(remote_port);
	printf("UDP server / port: %d\n", remote_port);
	int ret_bind = bind( udp_server_socket,
								(struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if( ret_bind == -1 ){
		fprintf(stderr,"ERROR: cannot bind at port %d: %s\n",
							remote_port, strerror(errno));
		exit(EXIT_FAILURE);
	}


	socklen_t udp_client_endpoint_length = sizeof(struct sockaddr_in);
	struct sockaddr_in udp_client_endpoint;
	ssize_t udp_read_bytes, udp_sent_bytes;
	//char buffer[];
	//...
	int16_t recv;
	// UDP IPv4: "recvfrom" do cliente (bloqueante)
	printf("à espera de dados do cliente... "); fflush(stdout);
	if ((udp_read_bytes = recvfrom(udp_server_socket, recv, sizeof(recv), 0, (struct sockaddr *) &udp_client_endpoint, &udp_client_endpoint_length)) == -1)
		ERROR(34, "Can't recvfrom client: %s\n", strerror(errno));
	printf("ok.  (%d bytes recebidos)\n", (int)udp_read_bytes);

	trata_send(recv,domus_status);

	// UDP IPv4: "sendto" para o cliente
	if(send_status_all==domus_status){
	printf("a enviar dados para o cliente... "); fflush(stdout);
	if ((udp_sent_bytes = sendto(udp_server_socket, send_status_all, sizeof(send_status_all), 0, (struct sockaddr *) &udp_client_endpoint, udp_client_endpoint_length)) == -1)
		ERROR(35, "Can't sendto client: %s\n", strerror(errno));
	printf("ok.  (%d bytes enviados)\n", (int)udp_sent_bytes);
}else if(send_status_desligado!=10){
	printf("a enviar dados para o cliente... "); fflush(stdout);
	if ((udp_sent_bytes = sendto(udp_server_socket, send_status_desligado, sizeof(send_status_desligado), 0, (struct sockaddr *) &udp_client_endpoint, udp_client_endpoint_length)) == -1)
		ERROR(35, "Can't sendto client: %s\n", strerror(errno));
	printf("ok.  (%d bytes enviados)\n", (int)udp_sent_bytes);
}else if(send_status_ligado!=10){
	printf("a enviar dados para o cliente... "); fflush(stdout);
	if ((udp_sent_bytes = sendto(udp_server_socket, send_status_ligado, sizeof(send_status_ligado), 0, (struct sockaddr *) &udp_client_endpoint, udp_client_endpoint_length)) == -1)
		ERROR(35, "Can't sendto client: %s\n", strerror(errno));
	printf("ok.  (%d bytes enviados)\n", (int)udp_sent_bytes);
}

	// liberta recurso: socket UDP IPv4
	if (close(udp_server_socket) == -1){
		ERROR(EXIT_FAILURE, "Can't close udp_server_socket (IPv4)");
	}

	cmdline_parser_free(&args_info);
	return 0;
}
void trata_desliga(int16_t recv,int16_t domus){
	int device=recv-20;
/*	int bitStatus = (domus >> device) & 1;
	int16_t mask=(1<<device);*/
	int mask = 1 << device;
	int bit = domus & mask;
	if(device>8&&device<1){
		send_status_desligado=2;
	}
	if(bit==0){
		send_status_desligado=0;
	}
	else{
		domus = domus & (~(1<<device));
		send_status_desligado=1;
	}

}
void trata_liga(int16_t recv,int16_t domus){
	int device=recv-20;
	int mask=(1<<device);
	if(device>8&&device<1){
		send_status_ligado=2;
	}
	if((domus&mask)==1){
		send_status_ligado=0;
	}
	else{
		domus = domus & (~(0<<device));
		send_status_ligado=1;
	}

}


void trata_send(int16_t recv,int16_t domus){
	if(recv==0){
		send_status_all=domus;
	}
	else if(recv >=20){
		trata_desliga(recv,domus);
	}
	else{
		trata_liga(recv,domus);
	}

}


int show_status(uint16_t status){
	char status_S[16][128];
	strcpy(status_S[0],"1 Portão da garagem (aberto / fechado)");
	strcpy(status_S[1],"2 Iluminação do hall de entrada");
	strcpy(status_S[2],"3 Iluminação sala");
	strcpy(status_S[3],"4 Iluminação jardim");
	strcpy(status_S[4],"5 Persiana 1");
	strcpy(status_S[5],"6 Persiana 2");
	strcpy(status_S[6],"7 Persiana 3");
	strcpy(status_S[7],"8 Piso radiante (ligado / desligado)");
	strcpy(status_S[8],"");
	strcpy(status_S[9],"");
	strcpy(status_S[10],"");
	strcpy(status_S[11],"");
	strcpy(status_S[12],"");
	strcpy(status_S[13],"");
	strcpy(status_S[14],"");
	strcpy(status_S[15],"");


	int num_bits = sizeof(status) * 8;
	for(int i=0; i<num_bits; i++){

		int mask = 1 << i;
		int bit = status & mask;
		if(strlen(status_S[i])>0) {
			printf("%s:%s\n",status_S[i],bit?"ON":"OFF");
		}
	}//for

	return 1;
}
