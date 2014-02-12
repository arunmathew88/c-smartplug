#include <iostream>
#include <fstream>
#include <cstdlib>
#include <queue>
#include <unordered_map>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "house.h"

using namespace std;

int readConfigFile(unordered_map <unsigned long, House*> * house_map, int sockfd)
{
    //Reading config file
    ifstream file("priv/nodes.config");
    string buffer;
    string ip_addr;
    string port_number;
    unsigned long house_id;

    while (getline(file, buffer))
    {
        unsigned pos_comma = buffer.find_first_of(",");
        house_id = stoul(buffer.substr(0, pos_comma));
        unsigned pos_colon = buffer.find_first_of(":");
        ip_addr = buffer.substr(pos_comma + 2, pos_colon - pos_comma - 2);
        port_number = buffer.substr(pos_colon+1);

        House *new_house = new House(ip_addr, port_number, house_id, sockfd);
        house_map->insert({house_id, new_house});
    }
    return 1;
}

int main()
{
    //create socket
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd) {
        perror("socket");
        return 1;
    }
    cout << "socket created " << endl;

    //Read config file and create connections to the houses
    unordered_map <unsigned long, House*> *house_map =  new unordered_map <unsigned long, House*>();
    readConfigFile(house_map, sockfd);

    //Read the data stream
    ifstream file("priv/temp.csv");
    string buffer;
    unsigned long house_id;


    while (getline(file, buffer))
    {
    	string message;
    	unsigned pos_first = buffer.find_first_of(","); //need to remove sample id
    	unsigned pos_last = buffer.find_last_of(","); //need to get the starting point of the house id
    	message = buffer.substr(pos_first + 1);
        house_id = stoul(buffer.substr(pos_last + 1).c_str());	//house id string to int
        cout << "house_id = " << house_id << endl;

        unordered_map<unsigned long, House*>::const_iterator got = house_map->find(house_id);
        if(got != house_map->end())
        {
            //send message to the house process
            got->second->sendMessage(message);
        }
        else
        {
            //erroneous house id
        }

    }
    close(sockfd);
	return 0;
}