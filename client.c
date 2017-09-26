#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

int sockfd, n;
pthread_t tr,tw;

void error(char *msg)
{
    perror(msg);
    exit(0);
}


void *thread_read(){
	
	char buffer[256];

	while(1){
		bzero(buffer,256);
     		n = read(sockfd,buffer,255);
			if(strcmp(buffer,"bye")==0){
				pthread_cancel(tr);
				pthread_cancel(tw);
			}
     		if (n < 0) error("ERROR reading from socket");
     		printf("%s\n",buffer);
	
	}
	pthread_exit((void *)NULL);
}

void *thread_write(){

	char buffer[256];

	while(1){
		gets(buffer);
     		n = write(sockfd,buffer,255);
		if(strcmp(buffer,"bye")==0){
			pthread_cancel(tr);
			pthread_cancel(tw);
		}
     		if (n < 0) error("ERROR writing to socket");
	
	}
	pthread_exit((void *)NULL);
}



int main(int argc, char *argv[])
{
    int portno, n;
    

    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
	
    pthread_create(&tw, NULL, thread_write, NULL);     
    pthread_create(&tr, NULL, thread_read, NULL);
    pthread_join(tw, NULL);
    pthread_join(tr, NULL);

    return 0;
        
}
