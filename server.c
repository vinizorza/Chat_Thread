/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

int newsockfd[10], n,qnt_cliente = 0;
pthread_t tr[10],t_accept;
int sockfd, portno, clilen;
struct sockaddr_in serv_addr, cli_addr;


void *thread_read(int cliente){	
     	
	while(1){
		char buffer[256];
		bzero(buffer,256);
		
		
		n = read(newsockfd[cliente],buffer,255);
			
		for(int i = 0; i < qnt_cliente; i++){			
			if(i != cliente){
				n = write(newsockfd[i],buffer,255);			
			}
		}
		
				
		if(strcmp(buffer,"bye")==0){
			pthread_cancel(t_accept);
			for(int i = 0; i < qnt_cliente; i++){
				pthread_cancel(tr[i]);
			}			
		}
		
		
     		if (n < 0) error("ERROR reading from socket");
     		printf("CLIENTE [%d]: %s\n",cliente, buffer);
	
	}
	pthread_exit((void *)NULL);
}

void *thread_accept(){	
	
	int i = 0;
     	
	while(1){
		
		newsockfd[i] = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd[i] < 0) error("ERROR on accept");
		
		pthread_create(&tr[i], NULL, thread_read, i);
		 
		qnt_cliente++;
		i++;
		
	}
	pthread_exit((void *)NULL);
}




void error(char *msg)
{
    perror(msg);
    exit(1);
}

void fechar_socket(){
	
		close(sockfd);
		
		for(int i = 0; i < qnt_cliente; i++){
			
				close(newsockfd[i]);
		
		}
	
}

int main(int argc, char *argv[])
{


     
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     

     pthread_create(&t_accept, NULL, thread_accept, NULL);         
     
     pthread_join(t_accept, NULL);
     
     fechar_socket();
     
	return 0;
	 
}
