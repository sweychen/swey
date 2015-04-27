/*
 *==========================================================================
 *       Filename:  FileClockTest.cpp
 *    Description:  
 *        Version:  1.0
 *        Created:  2014年08月22日 19时15分37秒
 *       Revision:  none
 *       Compiler:  g++
 *         Author:  chzwei, chzwei3@gmail.com
 *   Organization:  sysu
 *==========================================================================
 */
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <fcntl.h>
using namespace std;

#define ReadLock(fd, offset, whence, len) \
	RegLock((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))
#define ReadwLock(fd, offset, whence, len) \
	RegLock((fd), F_SETLKW, F_RDLCK, (offset), (whence), (len))
#define WriteLock(fd, offset, whence, len) \
	RegLock((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))
#define WritewLock(fd, offset, whence, len) \
	RegLock((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))
#define UnLock(fd, offset, whence, len) \
	RegLock((fd), F_SETLK, F_UNLCK, (offset), (whence), (len))

int RegLock(int fd, int cmd, int type, off_t offset, int whence, off_t len){
	struct flock lock;
	lock.l_type = type;
	lock.l_start = offset;
	lock.l_whence = whence;
	lock.l_len = len;
	return(fcntl(fd, cmd, &lock));
}

int main(){
	int fd;
	char str[] = "/home/chzwei/cpp/Lightcp/test/tmp";
	if((fd = open(str, O_RDWR|O_CREAT)) == -1){
		perror("open\n");
		return -1;
	}
	if(ReadwLock(fd, SEEK_SET, 0, 0) == -1){
		perror("ReadwLock\n");
		return -1;
	}
	int pid;
	if((pid = fork()) == -1){
		perror("fork\n");
		return -1;
	}else if(pid == 0){
		if(WritewLock(fd, SEEK_SET, 0, 0) == -1){
			perror("child ReadwLock\n");
			return -1;
		}
		printf("child\n");
		return 0;
	}else{
		printf("father\n");
		sleep(2);
		if(UnLock(fd, SEEK_SET, 0, 0) == -1){
			perror("UnLock\n");
			return -1;
		}
	}
	return 0;
}

