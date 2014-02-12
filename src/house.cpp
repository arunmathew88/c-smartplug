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

// arg: house_id, port
int main(int argc, char const *argv[])
{
	int house_id, port;

	if(argc < 2)
	{
		cout<<"not enough arguments!"<<endl;
		exit(-1);
	} else
	{
		house_id = atoi(argv[1]);
		port = atoi(argv[2]);
	}

	int sockfd;
	int acceptfd;
	int forceRebind = 1;
	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t clientaddrlen = 0;
	const size_t bufflen = 4096;
	ssize_t bytesreceived = 0;
	char buff[bufflen];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd) {
		perror("socket");
		return 1;
	}

	if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &forceRebind, sizeof(int))) {
		perror("setsockopt");
		return 1;
	}

	memset((void *) &server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;

	if (-1 == bind(sockfd, (const struct sockaddr *) &server,
			sizeof(struct sockaddr_in))) {
		perror("bind");
		return 1;
	}

	if (-1 == listen(sockfd, 0)) {
		perror("listen");
		return 1;
	}

	acceptfd = accept(sockfd, (struct sockaddr *) &client, &clientaddrlen);
	if (-1 == acceptfd) {
		perror("accept");
		return 1;
	}

	while(true)
	{
		bytesreceived = recv(acceptfd, buff, bufflen, 0);
		if (-1 == bytesreceived) {
			perror("recv");
			return 1;
		}

		printf("%s\n", buff);

		// if end message, end connection
	}

	if (-1 == close(acceptfd)) {
		perror("close(acceptfd)");
		return 1;
	}
	if (-1 == close(sockfd)) {
		perror("close(sockfd)");
		return 1;
	}
	return 0;
}
