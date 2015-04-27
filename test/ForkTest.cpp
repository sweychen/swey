/*
 *==========================================================================
 *       Filename:  ForkTest.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2014年08月31日 22时50分20秒
 *       Revision:  none
 *       Compiler:  g++
 *         Author:  chzwei, chzwei3@gmail.com
 *   Organization:  sysu
 *==========================================================================
 */
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
using namespace std;

static void sig_chld(int signo){
	if(signo == SIGCHLD){
		printf("child signal\n");
	}
}

int main(){
	pid_t pid;
	if(signal(SIGCHLD, sig_chld) == SIG_ERR){
		perror("signal\n");
		return -1;
	}

	if((pid = fork()) < 0){
		perror("fork\n");
		return -1;
	}else if(pid == 0){
	//	sleep(2);
		printf("child\n");
		exit(0);
	}

	if((pid = fork()) < 0){
		perror("fork\n");
		return -1;
	}else if(pid == 0){
	//	sleep(2);
		printf("child\n");
		exit(0);
	}
	
	sleep(10);
	int res;
	while(wait(&res) >0){
		printf("wait\n");
	}
}

