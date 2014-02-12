#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <netdb.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>


int server();
int client(std::string message);

int main() {
	std::ifstream file("temp.csv");
    std::string buffer;
    int house_id;

    /*unordered_map <int, pid_t> m;
    for(int i = 0; i < SIZE; ++i)
    {
        m[std::to_string(i)] = i;
    }
    for(int i = 0; i < (SIZE-1); ++i)
    {
        m[std::to_string(i)] = m[std::to_string(i+1)];
    }*/

    while (std::getline(file, buffer))
    {
    	std::string message;
    	unsigned pos_first = buffer.find_first_of(","); //need to remove sample id
    	unsigned pos_last = buffer.find_last_of(","); //need to get the starting point of the house id
    	message = buffer.substr(pos_first + 1, pos_last - pos_first - 1);
    	house_id = atof(buffer.substr(pos_last + 1).c_str());	//house id string to int
    	std::cout << "house_id = " << house_id << std::endl;
        std::cout << "message = " << message << std::endl;
        pid_t childpid = fork();

		if (childpid == -1) {
			perror("fork");
			return 1;
		} else if (childpid != 0) {
			int status;
			server();
			if (childpid != waitpid(childpid, &status, 0)) {
				perror("waitpid");
				return 1;
			}
			return 0;
		} else {
			client(message);
			return 0;
		}
    	//send message to the house with house id

    }


}

int server() {
	int sockfd;
	int acceptfd;
	int forceRebind = 1;
	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t clientaddrlen = 0;
	const size_t bufflen = 4096;
	ssize_t bytesreceived = 0;
	char buff[bufflen];
	const char * reply = "I'm fine thanks!";

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
	server.sin_port = htons(1234);
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

	//in while loop till max
	acceptfd = accept(sockfd, (struct sockaddr *) &client, &clientaddrlen);
	if (-1 == acceptfd) {
		perror("accept");
		return 1;
	}

	//create a house process
	bytesreceived = recv(acceptfd, buff, bufflen, 0);
	if (-1 == bytesreceived) {
		perror("recv");
		return 1;
	}

	printf("%s\n", buff);

	if (-1 == send(acceptfd, reply, (size_t) strlen(reply) + 1, 0)) {
		perror("send parent");
		return 1;
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

int client(std::string message) {
	int sockfd;
	struct sockaddr_in server;
	struct addrinfo *serverinfo;
	const size_t bufflen = 4096;
	ssize_t bytesreceived = 0;
	char buff[bufflen];
	const char * question = message.c_str();

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd) {
		perror("socket");
		return 1;
	}

	if (0 != getaddrinfo("localhost", NULL, NULL, &serverinfo)) {
		perror("getaddrinfo");
		return 1;
	}

	/*Copy size of sockaddr_in b/c res->ai_addr to big for this example*/
	memcpy(&server, serverinfo->ai_addr, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(1234);
	freeaddrinfo(serverinfo);

	if (-1 == connect(sockfd, (const struct sockaddr *) &server,
			sizeof(struct sockaddr_in))) {
		perror("connect");
		return 1;
	}

	if (-1 == send(sockfd, question, (size_t) strlen(question) + 1, 0)) {
		perror("send");
		return 1;
	}

	bytesreceived = recv(sockfd, buff, bufflen, 0);
	if (-1 == bytesreceived) {
		perror("recv");
		return 1;
	}
	printf("%s\n", buff);

	close(sockfd);
	return 0;
}