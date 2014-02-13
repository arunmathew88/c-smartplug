#include <iostream>
#include <cstring>
#include <cstdio>
#include "zhelpers.hpp"
#include "mc.h"
using namespace std;

// arg: house_id ip sync_port default_data_port
int main(int argc, char const *argv[])
{
	int house_id, default_data_port;
	string broker_ip, sync_port;

	if(argc < 5)
	{
		cout<<"not enough arguments!"<<endl;
		exit(-1);
	} else
	{
		house_id  = atoi(argv[1]);
		broker_ip = string(argv[2]);
		sync_port = string(argv[3]);
		default_data_port = atoi(argv[4]);
	}
	cout<<"house running with id :"<<house_id<<endl;

	// creating a zmq context
	zmq::context_t context(1);

	//  First, connect our subscriber socket
    zmq::socket_t subscriber(context, ZMQ_PAIR);
    subscriber.connect((string("tcp://") + broker_ip + string(":") + to_string(default_data_port+house_id)).c_str());

    // second, synchronize with publisher
    zmq::socket_t syncclient(context, ZMQ_REQ);
    syncclient.connect((string("tcp://") + broker_ip + string(":") + sync_port).c_str());

    // send a synchronization request
    s_send(syncclient, "ready");

    // wait for synchronization reply
    s_recv(syncclient);

    while(true) {
    	// read message contents
        string contents = s_recv(subscriber);
        cout  << contents << endl;
    }

    return 0;
}
