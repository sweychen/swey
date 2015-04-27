#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

const char* IP = "127.0.0.1";
const int PORT = 12345;
const int BACKLOG = 5;
const int BUF_SIZE = 1024;

int SocketServer(){
	int sockfd;
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket\n");
        return -1;
    }

	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(PORT);

    if( bind(sockfd, (struct sockaddr*)&address, sizeof(address)) < 0){
        perror("bind\n");
        return -1;
    }

	if(listen(sockfd, BACKLOG) < 0){
        perror("listen\n");
        return -1;
    }
	return sockfd;
}

void ReadBuf(int fd){
	char buf[2048];
	int buf_len = 0;
	while( (buf_len = read(fd, buf, 2048)) > 0){
		printf("%s\n", buf);
	}
}

void WriteBuf(int fd){
	char buf[] = "server write\n";
	int buf_len = strlen(buf)+1;
	while(true){
		write(fd, buf, buf_len);
	}
}

int main(){
	int sockfd = SocketServer();
	struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
	int acceptfd;
    if((acceptfd = accept(sockfd, (struct sockaddr*)&client, &client_addrlength)) < 0){
		perror("accept\n");
		return -1;
	}

	switch(fork()){
		case -1:
			return -1;
		case 0:
			ReadBuf(acceptfd);
			break;
		default:
			WriteBuf(acceptfd);
			break;
	}
}
