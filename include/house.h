#ifndef HOUSE_H
#define HOUSE_H

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

class House
{
	string ip_addr;
	string port_number;
	int house_id;
	int sockfd;
	void setConnection();

  public:
  	House(string ip, string port, int id, int socketfd);
    void sendMessage(string message);
};

#endif
