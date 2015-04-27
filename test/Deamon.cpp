#include <unistd.h> 
#include <signal.h> 
#include <sys/param.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <sys/resource.h>
#include <syslog.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

void InitDaemon() 
{
	int i, fd0, fd1, fd2, fd3;
	pid_t pid;
	struct rlimit r1;
	struct sigaction sa;

	printf("1\n");
	//clear file creation mask
	umask(0);
	
	printf("2\n");
	
	printf("3\n");
	if((pid = fork()) < 0) {
		perror("fork\n");
		exit(1);
	}else if(pid != 0) {
		exit(0);
	}
	
	printf("4\n");
	//become a session leader to lose controlling tty
	setsid();

	//ensure future opens won't allocate controlling ttys
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if(sigaction(SIGHUP, &sa, NULL) < 0){
		perror("sigaction\n");
		exit(1);
	}

	printf("5\n");
	if((pid = fork()) < 0){ 
		perror("fork\n");
		exit(1);
	}else if(pid != 0){
		exit(0);
	} 

	//second child process, no session leader
	
	//change the current working directory to the root 
	//so we won't prevent file system from unmounted
	printf("6\n");
	if(chdir("/") < 0){
		perror("chdir\n");
		exit(1);
	}
	
	printf("7\n");
	//get maximum number of file descriptors
	if(getrlimit(RLIMIT_NOFILE, &r1) < 0){
		perror("getrlimit\n");
		exit(1);
	}

	//close all open file descriptors
	if(r1.rlim_cur == RLIM_INFINITY)
		r1.rlim_cur = 1024;
	for(i = 0; i < r1.rlim_cur; ++i)
		close(i); 
	
	//attach file descriptors 0, 1, 2, to /dev/null
	fd0 = open("/dev/null", O_RDWR);
	if(fd0 < 0){
		perror("open\n");
		exit(1);
	}
	fd1 = dup2(fd0, STDIN_FILENO);
	fd2 = dup2(fd0, STDOUT_FILENO);
	fd3 = dup2(fd0, STDERR_FILENO);
	if(fd1 != STDIN_FILENO || fd2 != STDOUT_FILENO || fd3 != STDERR_FILENO){
		perror("dup2\n");
		exit(1);
	}

	//initialize the log file
	
	printf("8\n");
} 

int main()
{
    FILE *fp;
    time_t t;
	printf("hello1\n");
    InitDaemon();
	printf("hello2\n");
    while(1)//每隔一分钟向test.log报告运行状态
    {
        sleep(2);//睡眠一分钟
        if((fp=fopen("/home/chzwei/tmp/Lightcp/test/test.log","a")) >=0){
            t=time(0);
            fprintf(fp,"Im here at %sn",asctime(localtime(&t)) );
            fclose(fp);
        }
    }
}
