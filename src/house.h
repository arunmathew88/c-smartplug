#ifndef HOUSE_H
#define HOUSE_H

#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

class House
{
   	private:
   		string ip_addr;
		string port_number;
		int house_id;
		int sockfd;
		int setConnection();

    public:
    	int sendMessage(string message);
        House();
        House(string ip, string port, int id, int socketfd);

};

#endif