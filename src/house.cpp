#include <iostream>
#include <cstring>
#include <cstdio>
#include "zhelpers.hpp"
#include "mc.h"
using namespace std;

// arg: house_id ip sync_port data_port
int main(int argc, char const *argv[])
{
	string house_id, broker_ip, sync_port, data_port;

	if(argc < 5)
	{
		cout<<"not enough arguments!"<<endl;
		exit(-1);
	} else
	{
		house_id  = string(argv[1]);
		broker_ip = string(argv[2]);
		sync_port = string(argv[3]);
		data_port = string(argv[4]);
	}
	cout<<"house running with id :"<<house_id<<endl;

	// creating a zmq context
	zmq::context_t context(1);

	//  First, connect our subscriber socket
    zmq::socket_t subscriber(context, ZMQ_SUB);
    subscriber.connect((string("tcp://") + broker_ip + string(":") + data_port).c_str());
    if(house_id.length() == 1)
    	house_id = string("0") + house_id;
    subscriber.setsockopt(ZMQ_SUBSCRIBE, house_id.c_str(), 2);

    // second, synchronize with publisher
    zmq::socket_t syncclient(context, ZMQ_REQ);
    syncclient.connect((string("tcp://") + broker_ip + string(":") + sync_port).c_str());

    // send a synchronization request
    s_send(syncclient, "ready");

    // wait for synchronization reply
    s_recv(syncclient);

    while(true) {
        // read envelope with address
        string address = s_recv(subscriber);

        // read message contents
        string contents = s_recv(subscriber);
        cout << "[" << address << "] " << contents << endl;
    }

    return 0;
}
