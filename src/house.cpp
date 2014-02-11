#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <netdb.h>
#include <pthread.h>
#include "mc.h"
using namespace std;

// listening function
// create a new household thread, if doesn't exist
// else store a message after parsing
void *listenServer(void *threadid)
{
	while(true)
	{
		bytesreceived = recv(acceptfd, buff, bufflen, 0);
		if(-1 == bytesreceived) {
			perror("recv");
			return 1;
		}
	}

	if(-1 == close(acceptfd)) {
		perror("close(acceptfd)");
		return 1;
	}

	pthread_exit(NULL);
}

// function
// hosuhold thread
// process the message, synchronize

// arg: acceptfd, house_id
int main(int argc, char const *argv[])
{
	int acceptfd, house_id;

	if(argc < 2)
	{
		cout<<"not enough arguments!"<<endl;
		exit(-1);
	} else
	{
		acceptfd = atoi(argv[1]);
		house_id = atoi(argv[2]);
	}

	cout<<acceptfd<<"\t"<<house_id<<endl;

	// create a thread to listen on the socket (virtual)
	pthread_t listen_thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	int rc = pthread_create(&listen_thread, &attr, listenServer, NULL);
    if(rc){
        printf("ERROR: return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

    pthread_attr_destroy(&attr);
    void *status;
    rc = pthread_join(listen_thread, &status);
    if (rc) {
		printf("ERROR; return code from pthread_join() is %d\n", rc);
		exit(-1);
	}

	printf("Main: program completed. Exiting.\n");
	pthread_exit(NULL);

	// handle house processing

}
