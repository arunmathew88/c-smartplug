#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <netdb.h>

int main() {
	int sockfd;
	int acceptfd;
	int forceRebind = 1;
	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t clientaddrlen = 0;
	const size_t bufflen = 4096*2;
	ssize_t bytesreceived = 0;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == sockfd) {
		perror("socket");
		return -1;
	}

	if(-1 == setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &forceRebind, sizeof(int))) {
		perror("setsockopt");
		return 1;
	}

	memset((void *) &server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(1234);
	server.sin_addr.s_addr = INADDR_ANY;

	if(-1 == bind(sockfd, (const struct sockaddr *) &server,
			sizeof(struct sockaddr_in))) {
		perror("bind");
		return 1;
	}

	if(-1 == listen(sockfd, 0)) {
		perror("listen");
		return 1;
	}

	while(true)
	{
		acceptfd = accept(sockfd, (struct sockaddr *) &client, &clientaddrlen);
		if(-1 == acceptfd) {
			perror("accept");
			return 1;
		}

		cout<<(string(acceptfd) + " 10"<<endl;
		cout<<execvp("./house", (string(acceptfd) + " 10").c_str())<<endl;

	}

	if(-1 == close(sockfd)) {
		perror("close(sockfd)");
		return 1;
	}
	return 0;
}
