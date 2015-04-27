#ifndef CONNECTION_H
#define CONNECTION_H

#include <sys/socket.h>
#include <stdlib.h>
#include "Epoll.h"
#include <stdio.h>

enum ConnectionState{
	CONNET,
	READ_HEAD,
	READ_DATA,
	READ_END,
	HANDLE,
	WRITE_HEAD,
	WRITE_DATA,
	WRITE_END,
	CLOSE,
	ERR
};


class Connection{
public:
	Connection(int accept_fd){
		fd = accept_fd;
		SetNoBlocking(fd);
		epl = Epoll::GetInstance();
	}

	void SetState(ConnectionState state){
		conn_stat = state;
	}

	int ConnectionStateMachine(){
		int done = 0;
		ConnectionState old_state;
		while(done == 0){
			old_state = conn_stat;
			switch(conn_stat){
			case CONNET:
				data_len_left_size = sizeof(int);
				SetState(READ_HEAD);
			case READ_HEAD:
				ReadHead();
				break;	
			case READ_DATA:
				ReadData();		
				break;
			case READ_END:
				SetState(HANDLE);
				break;
			case HANDLE:
				Handle();
				break;
			case WRITE_HEAD:
				WriteHead();
				break;
			case WRITE_DATA:
				WriteData();
				break;
			case WRITE_END:
				WriteEnd();
				break;
			case CLOSE:
				close(fd);
				break;
			case ERR:
				break;
			default:
				break;
			}
			if(old_state == conn_stat){
				done = 1;
			}
		}
	}

	void ReadHead(){
		int recv_size = read(fd, &data_len+sizeof(int)-data_len_left_size, data_len_left_size);
		if(recv_size > 0){
			data_len_left_size -= recv_size;
			if(data_len_left_size == 0){
				data_offset = 0;
				data_left_size = data_len;
				printf("data_len %d\n", data_len);
				SetState(READ_DATA);
			}else{
				SetState(READ_HEAD);
			}
		}else if(recv_size == 0){
			//客户端关闭连接
			SetState(READ_END);
		}else{
			if(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN){
				SetState(READ_HEAD);
			}else{
				SetState(ERR);
			}
		}
	}

	void ReadData(){
		int recv_size = read(fd, data_buf+data_offset, data_left_size);
		printf("recv %s\n", data_buf+data_offset);
		if(recv_size > 0){
			data_left_size -= recv_size;
			data_offset += recv_size;
			if(data_left_size == 0){

				printf("recv %s\n", data_buf+data_offset);
				SetState(READ_END);
			}else{
				SetState(READ_DATA);
			}
		}else if(recv_size == 0){
			SetState(READ_END);
		}else{
			if(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN){
				SetState(READ_DATA);
			}else{
				SetState(ERR);
			}
		}
	}

	void Handle(){
		printf("serve recive: %s\n", data_buf);
		memset(data_buf, 0, 4096);
		char tmp_buf[] = "serve send";
		memcpy(data_buf, tmp_buf, strlen(tmp_buf));
		data_len = strlen(tmp_buf);

		data_left_size = data_len;
		data_offset = 0;
		if(epl->EpollModWptr(fd, this) < 0){
			return;
		}else{
			SetState(WRITE_HEAD);
		}
	}

	void WriteHead(){
		int send_size = write(fd, &data_len+sizeof(int)-data_len_left_size, data_len_left_size);
		if(send_size >= 0){
			data_len_left_size -= send_size;
			if(data_len_left_size == 0){
				SetState(WRITE_DATA);
				return;
			}else{
				SetState(WRITE_HEAD);
			}
		}else{
			if(errno == EINTR || errno == EAGAIN){
				SetState(WRITE_HEAD);
			}else{
				SetState(ERR);
			}
		}
	}

	void WriteData(){
		int send_size = write(fd, data_buf+data_offset, data_left_size);
		if(send_size >= 0){
			data_left_size -= send_size;
			if(data_left_size == 0){
				SetState(WRITE_END);
			}else{
				SetState(WRITE_DATA);
			}
		}else{
			if(errno == EINTR || errno == EAGAIN){
				SetState(WRITE_DATA);
			}else{
				SetState(ERR);
			}
		}
	}

	void WriteEnd(){
		if(epl->EpollDel(fd) < 0){
			return;
		}else{
			SetState(WRITE_HEAD);
		}
		SetState(CLOSE);
	}
private:
	ConnectionState conn_stat;
	int data_len;
	int data_len_left_size;
	char data_buf[4096];
	int data_offset;
	int data_left_size;
	int fd;
	Epoll *epl;
};

#endif

