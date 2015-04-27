#ifndef MINHEAP_H
#define MINHEAP_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

class HeapObj{
public:
	int alrm_time;
	void *ptr;

public:
	HeapObj(){
		alrm_time = 0;
		ptr = NULL;
	}

	HeapObj(int t, void *p){
		alrm_time = time(NULL)+t;
		ptr = p;
	}

	HeapObj(const HeapObj &obj){
		alrm_time = obj.alrm_time;
		ptr = obj.ptr;
	}

	~HeapObj(){}

	int GetAlrmTime(){ return alrm_time; }
	void* GetPtr(){ return ptr; }

	HeapObj& operator=(const HeapObj &obj){
		alrm_time = obj.alrm_time;
		ptr = obj.ptr;
		return *this;
	}

	bool operator<(const HeapObj &obj){
		return alrm_time < obj.alrm_time;
	}

	bool operator>(const HeapObj &obj){
		return alrm_time > obj.alrm_time;
	}

};

class MinHeap
{
public:
	MinHeap():size(0), capacity(10){
		data = new HeapObj[capacity];
	}

	MinHeap(int num):size(0), capacity(num){
		data = new HeapObj[capacity];
	}

	~MinHeap(){
		if(data)
			delete [] data;
	}

	void push(HeapObj obj){
		int index = size;
		++size;
		if(size == capacity){
			ResizeData();
		}
		while(index > 0){
			if(obj < data[(index-1)/2]){
				data[index] = data[(index-1)/2];
				index = (index-1)/2;
			}else{
				data[index] = obj;
				return ;
			}
		}
		data[0] = obj;
	}

	int Pop(HeapObj &obj){
		if(size < 1){
			return -1;
		}else if(size == 1){
			obj = data[0];
			--size;
			return 0;
		}
		obj = data[0];
		--size;

		data[0] = data[size];
		int index = 1;
		HeapObj tmp = data[0];
		while(index < size){
			if(index+1 < size && data[index+1] < data[index])
				++index;

			if( data[index] < tmp){
				data[(index-1)/2] = data[index];
				data[index] = tmp;
				index = index*2+1;
			}else{
				return 0;
			}
		}
		return 0;
	}

	int Front(HeapObj &obj){
		if(size > 0){
			obj = data[0];
			return 0;
		}else{
			return -1;
		}
	}

	int Size(){
		return size;
	}
private:
	int size;
	HeapObj *data;
	int capacity;
	void ResizeData(){
		int old_capacity = capacity*2;
		HeapObj *tmp_data = new HeapObj[capacity];
		for(int i = 0; i < capacity; ++i){
			tmp_data[i] = data[i];	
		}
		delete [] data;
		data = tmp_data;
	}
};

static MinHeap time_heap;
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

class TimeHeap(){
public:
	static TimeHeap* GetInstance(){
		if(instance == NULL){
			instance = new TimeHeap;
			instance->Init();
		}
	    return instance;
	}
	MinHeap heap;
private:
	TimeHeap(){}
	static TimeHeap *instance;
	class CG
	{
	public:
		~CG(){
			if(TimeHeap::instance){
				delete TimeHeap::instance;
			}
		}
	};
	static CG cg;
}

TimeHeap* TimeHeap::instance = NULL;

#endif
