#ifndef API_H
#define API_H

class Api{
	int Read(const char* buf, char* res){
		printf("serve recive: %s\n", buf);
		res = "serve send: ";
	}
};

#endif
