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
#include <errno.h>

#include "debug.h"
#include "common.h"
#include "server_opt.h"
#define C_MAX_PORT (1<<16)

int show_status(int16_t status);
int16_t trata_send(int16_t domus,int16_t recv);

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
    int status = args_info.status_arg;
    if( status < 0 || status >= (1<<16) ){
  		fprintf(stderr,"[ERROR] Invalid status '0x%x' (out of range)\n",
  				status);
  		exit(EXIT_FAILURE);
    }
    int16_t domus_status = (int16_t) status;
    show_status(domus_status);

    int udp_server_socket;
    if ((udp_server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
      ERROR(31, "Can't create udp_server_socket (IPv4): %s\n", strerror(errno));

    // UDP IPv4: bind a IPv4/porto
    struct sockaddr_in udp_server_endpoint;
    memset(&udp_server_endpoint, 0, sizeof(struct sockaddr_in));
    udp_server_endpoint.sin_family = AF_INET;
    udp_server_endpoint.sin_addr.s_addr = htonl(INADDR_ANY);  	// Todas as interfaces de rede
    udp_server_endpoint.sin_port = htons(port);	// Server port
    if (bind(udp_server_socket, (struct sockaddr *) &udp_server_endpoint, sizeof(struct sockaddr_in)) == -1)
      ERROR(32, "Can't bind @udp_server_endpoint info: %s\n", strerror(errno));

    socklen_t udp_client_endpoint_length = sizeof(struct sockaddr_in);
  	struct sockaddr_in udp_client_endpoint;
  	ssize_t udp_read_bytes, udp_sent_bytes;
  	//char buffer[];
  	//...
  	// UDP IPv4: "recvfrom" do cliente (bloqueante)
   int16_t recv=0;
  	printf("à espera de dados do cliente... "); fflush(stdout);
  	if ((udp_read_bytes = recvfrom(udp_server_socket, &recv, sizeof(recv), 0, (struct sockaddr *) &udp_client_endpoint, &udp_client_endpoint_length)) == -1)
  		ERROR(34, "Can't recvfrom client: %s\n", strerror(errno));
  	printf("ok.  (%d bytes recebidos)\n", (int)udp_read_bytes);
   int16_t send_tratado = trata_send(domus_status,recv);
   int16_t send=0;
   printf("send tratado = %d\n",send_tratado);
   if(send_tratado==0){
      send=2;//codigo de resposta
   }else if(send_tratado==99){
      printf("99\n");
      send=domus_status;
   }
   else if(send_tratado!=domus_status){
      send=1;
      domus_status=send_tratado;
   }
   else{
      send=0;
   }
   printf("send %d\n",send);
    // UDP IPv4: "sendto" para o cliente
    printf("a enviar dados para o cliente... "); fflush(stdout);
    if ((udp_sent_bytes = sendto(udp_server_socket, &send, sizeof(send), 0, (struct sockaddr *) &udp_client_endpoint, udp_client_endpoint_length)) == -1)
    	ERROR(35, "Can't sendto client: %s\n", strerror(errno));
    printf("ok.  (%d bytes enviados)\n", (int)udp_sent_bytes);
    show_status(domus_status);

    if (close(udp_server_socket) == -1)
  		ERROR(33, "Can't close udp_server_socket (IPv4): %s\n", strerror(errno));

    cmdline_parser_free(&args_info);

    return 0;
}
int16_t trata_send(int16_t domus,int16_t recv){
   int device=0;
   int16_t newDomus=domus;
   if(recv>20){
      device=(recv-20)-1;
      if(device>=0&&device<9){
         if(((domus>>device)&1)!=0){
            //verificações
            //codigo do set
            newDomus=newDomus&(~(1<<device));
            return newDomus;
         }
      }else{
         newDomus=0;
         return newDomus;
      }
   }else if(recv>10&&recv<20){
      device=(recv-10)-1;
      if(device>=0&&device<9){
         if(((domus>>device)|1)!=0){
            //verificações
            //codigo do set
            newDomus=(1<<device)|newDomus;
            return newDomus;
         }
      }else{
         newDomus=0;
         return newDomus;
      }

   }else if(recv==0){
      printf("99 recv\n");
      newDomus=99; //numero não plausivel para verificações
      return newDomus;
   }

}
int show_status(int16_t status){
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
