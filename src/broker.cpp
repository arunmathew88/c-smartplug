#include <iostream>
#include <fstream>
#include <cstdlib>
#include <queue>
#include <unordered_map>

#include "house.h"

using namespace std;

struct Node
{
    string ip_addr;
    string port_number;
    int max_house_processes;
    int remaining_house_processes;
    Node(string ip, string port, int max): ip_addr(ip), port_number(port), max_house_processes(max), remaining_house_processes(max)
    {
    }
};

class CompareNode
{
    bool reverse;

    public:
    CompareNode(const bool& rev=false){ reverse=rev;}
    bool operator()(const Node * lhs, const Node * rhs) const
    {
        if(reverse) return (lhs->remaining_house_processes > rhs->remaining_house_processes);
        else return (lhs->remaining_house_processes < rhs->remaining_house_processes);
    }
};

int readConfigFile(priority_queue<Node*, vector<Node* >, CompareNode> *node_info)
{
    //Reading config file
    ifstream file("priv/nodes.config");
    string buffer;
    string ip_addr;
    string port_number;
    int max_house_processes;

    while (getline(file, buffer))
    {
        unsigned pos_colon = buffer.find_first_of(":");
        ip_addr = buffer.substr(0, pos_colon - 1);
        unsigned pos_comma = buffer.find_first_of(",");
        port_number = buffer.substr(pos_colon + 1, pos_comma - pos_colon - 1);
        max_house_processes = atoi(buffer.substr(pos_comma + 1).c_str());
        Node *newNode = new Node(ip_addr, port_number,max_house_processes);
        node_info->push(newNode);
    }
    return 1;
}

int main()
{
    //Read config file
    priority_queue<Node*, vector<Node* >, CompareNode> *node_info = new priority_queue<Node*, vector<Node* >, CompareNode>();
    readConfigFile(node_info);

    while(!node_info->empty())
    {
        Node * n = node_info->top();
        node_info->pop();
        cout << n->ip_addr << " " << n->port_number << " "<< n->max_house_processes << endl;
    }

    //Read the data stream
    ifstream file("priv/temp.csv");
    string buffer;
    unsigned long house_id;

    unordered_map <unsigned long, House*> house_map;
    /*
    for(int i = 0; i < SIZE; ++i)
    {
        m[to_string(i)] = i;
    }
    for(int i = 0; i < (SIZE-1); ++i)
    {
        m[to_string(i)] = m[to_string(i+1)];
    }*/

    while (getline(file, buffer))
    {
    	string message;
    	unsigned pos_first = buffer.find_first_of(","); //need to remove sample id
    	unsigned pos_last = buffer.find_last_of(","); //need to get the starting point of the house id
    	message = buffer.substr(pos_first + 1);
        house_id = stoul(buffer.substr(pos_last + 1).c_str());	//house id string to int
        cout << "house_id = " << house_id << endl;

        unordered_map<unsigned long, House*>::const_iterator got = house_map.find(house_id);
        if(got != house_map.end())
        {
            //house id found in the map
            got->second->send(message);
        }
        else
        {
            //house id not found in the map
            //get the daemon on which the house process is to be run
            Node * n = node_info->top();
            node_info->pop();
            n->remaining_house_processes = n->remaining_house_processes - 1;
            node_info->push(n);

            //create an house process
            //send message to the house
        }
        //cout << "message = " << message << endl;
    	//send message to the house with house id

    }
	return 0;
}