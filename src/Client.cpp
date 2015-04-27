#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>


int main(int argc, char **argv){
	int sockfd;
	struct sockaddr_in serv_addr;
	if(argc != 3){
		fprintf(stderr, "usage: %s <IPaddress> <Port>", argv[0]);
		return -1;
	}
	int port = atoi(argv[2]);
	printf("%s %s %d\n", argv[0], argv[1], port);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);

	connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	size_t buf_len = 2048;
	char *buf = new char[2048];
	int get_size = 0;
	while((get_size = getline(&buf, &buf_len, stdin)) != -1){
		write(sockfd, (char*)&get_size, sizeof(int));
		write(sockfd, buf, get_size);
		
		read(sockfd, buf, 2048);
		printf("%s\n", buf);
	}

}
