#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include "Lightlib.h"
#include "Connection.h"
#include "Epoll.h"
#include "TimeHeap.h"
using namespace std;

static bool shut_down = false;
static TimeHeap time_heap = TimeHeap::GetInstance()->heap;
static void SigAlrm(int signo){
	printf("in SigAlrm\n");
	int now_time = time(NULL);
	HeapObj obj;
	while(time_heap.Front(obj1) == -1){
		if(obj.alrm_time > now_time){
			alarm(obj.alrm_time - now_time);
			break;
		}else{
			int *fd = (int*)obj1.GetPtr();
			write(*fd, " ", 1);
			time_heap.Pop(obj);
		}
	}
}


int main(){
	int child_num = 0;
	bool child = false;

	if(signal(SIGALRM, SigAlrm) == SIG_ERR){
		perror("signal\n");
		return 1;
	}

	
	int sockfd; 
	if( (sockfd = SocketServer()) < -2 )
		return -1;

    epoll_event events[MAX_EVENT_NUMBER];
    Epoll *epl = Epoll::GetInstance();
    if(epl->EpollAddRfd(sockfd) < 0){
        perror("addfd");
        return -1;
    }
	
	int pid;
	if(child_num > 0){
		while(!child && !shut_down){
			if(child_num > 0){
				switch(fork()){
				case -1:
					return -1;
				case 0:
					child = 1;
					break;
				default:
					-- child_num;
					break;
				}
			}else{
				int status;
				if( -1 != wait(&status) ){
					++ child_num;
				}else{
					switch(errno) {
					case EINTR:
						break;
					default:
						break;
					}
				}
			}
		}	

		if(!child){
			return 0;
		}
	}
	

	printf("sockfd %d\n", sockfd);

	int ret = 0;
	while(!shut_down){
		if((ret = epl->EpollWait(events, MAX_EVENT_NUMBER, -1)) < 0){
			perror("epoll wait\n");
			continue;
		}else{
			for(int i = 0; i < ret; ++i){
				int tmpfd = events[i].data.fd;
				Connection *tmp_conn = (Connection*)events[i].data.ptr;
				if(tmpfd == sockfd){
					struct sockaddr_in client;
				    socklen_t client_addrlength = sizeof(client);
					int acceptfd;
				    if((acceptfd = accept(sockfd, (struct sockaddr*)&client, &client_addrlength)) < 0){
						perror("accept\n");
					}else{
						Connection *accept_conn = new Connection(acceptfd); 
						if(epl->EpollAddRptr(acceptfd, accept_conn) < 0){
					        perror("addfd");
					        delete accept_conn;
					    }
					}
				}else{
					tmp_conn->ConnectionStateMachine();
				}				
			}
		}
	}
	return 0;
}
