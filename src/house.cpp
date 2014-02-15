#include "house.h"

House::House(string ip, string port, int id, int socketfd)
{
	ip_addr = ip;
	port_number = port;
	house_id = id;
	sockfd = socketfd;
	setConnection();
}

int House::setConnection()
{
	struct sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(atoi(port_number.c_str()));
	dest_addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());

	if(-1 == connect(sockfd, (const struct sockaddr *) &dest_addr, sizeof(struct sockaddr_in)))
	{
        perror("connect");
        return -1;
    }
    cout << "connected to "<< ip_addr << ":" << port_number  << endl;
    return 1;
}

int House::sendMessage(string message)
{
	if (-1 == send(sockfd, message.c_str(), (size_t) strlen(message.c_str()) + 1, 0))
	{
        perror("send");
        return -1;
    }
    return 1;
}
