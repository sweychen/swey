#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <cstring>
#include <unistd.h>

using namespace std;

const char* IP = "127.0.0.1";
const int PORT = 12345;
const int BACKLOG = 5;
const int BUF_SIZE = 1024;

void ReadBuf(int fd){
	char buf[2048];
	int buf_len = 0;
	while( (buf_len = read(fd, buf, 2048)) > 0){
		printf("%s\n", buf);
	}
}

void WriteBuf(int fd){
	char buf[] = "client write\n";
	int buf_len = strlen(buf)+1;
	while(true){
		write(fd, buf, buf_len);
	}
}

int main(){
	int sockfd;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket\n");
        return -1;
    }

	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, IP, &address.sin_addr);
	address.sin_port = htons(PORT);

	if(connect(sockfd, (struct sockaddr*) &address, sizeof(address)) < 0){
		perror("connect");
		return -1;
	}

	switch(fork()){
		case -1:
			return -1;
		case 0:
			ReadBuf(sockfd);
			break;
		default:
			WriteBuf(sockfd);
			break;
	}

}
