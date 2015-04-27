#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

const char* IP = "127.0.0.1";
const int PORT = 12345;
const int BACKLOG = 5;
const int BUF_SIZE = 1024;
const int MAX_EVENT_NUMBER = 5;

int SetNoBlocking(int fd){
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

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

int main(){
	int sockfd = SocketServer();

	struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
	int acceptfd;
    if((acceptfd = accept(sockfd, (struct sockaddr*)&client, &client_addrlength)) < 0){
		perror("accept\n");
		return -1;
	}
    SetNoBlocking(acceptfd);

	int epollfd;
    epoll_event events[MAX_EVENT_NUMBER];
	if((epollfd= epoll_create(MAX_EVENT_NUMBER)) < 0){
        perror("epoll_create");
        return -1;    
    }

	epoll_event event1;
    event1.data.fd = 0;
    event1.events = EPOLLIN;
    int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, acceptfd, &event1);
	

	epoll_event event2;
    event2.data.fd = 1;
    event2.events = EPOLLOUT|EPOLLIN;
    ret = epoll_ctl(epollfd, EPOLL_CTL_MOD, acceptfd, &event2);

	while(true){
		if((ret = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1)) < 0){
			perror("epoll wait\n");
			continue;
		}else{
			for(int i = 0; i < ret; ++i){
				int tmpfd = events[i].data.fd;
				printf("%d %d %d\n", tmpfd, events[i].events&EPOLLIN, events[i].events&EPOLLOUT);
			}	
		}
	}
}
