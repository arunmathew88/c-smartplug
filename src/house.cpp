#include <iostream>
#include <cstring>
#include <cstdio>
#include "zhelpers.hpp"
#include "mc.h"
using namespace std;

#define NUM_THREADS 10

// arg: house_id broker_ip sync_port self_ip data_port
int main(int argc, char const *argv[])
{
	string house_id, broker_ip, sync_port, self_ip, data_port;

	if(argc < 6)
	{
		cout<<"not enough arguments!"<<endl;
		exit(-1);
	} else
	{
		house_id  = string(argv[1]);
		broker_ip = string(argv[2]);
		sync_port = string(argv[3]);
		self_ip   = string(argv[4]);
		data_port = string(argv[5]);
	}
	cout<<"house running with id :"<<house_id<<endl;

	// creating a zmq context
	zmq::context_t context(NUM_THREADS);

	// listen on a pull socket
    zmq::socket_t subscriber(context, ZMQ_PULL);
    subscriber.bind((string("tcp://*:") + data_port).c_str());

    // synchronize with publisher
    zmq::socket_t syncclient(context, ZMQ_REQ);
    syncclient.connect((string("tcp://") + broker_ip + string(":") + sync_port).c_str());

    // send a synchronization request
    s_sendmore(syncclient, house_id);
    s_send(syncclient, self_ip+string(":")+data_port);

    // wait for synchronization reply
    s_recv(syncclient);

    while(true) {
        // read message contents
        string contents = s_recv(subscriber);
        cout << contents << endl;
    }

    return 0;
}
