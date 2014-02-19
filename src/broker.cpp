#include <iostream>
#include <fstream>
#include "common.h"
#include "zhelpers.hpp"
using namespace std;

#define SUBSCRIBERS_EXPECTED 40
#define NUM_THREADS 10
#define SYNC_PORT 5556
#define SLEEP_TIME 1000

int main()
{
    zmq::context_t context(NUM_THREADS);

    // socket to talk to clients
    zmq::socket_t **broker = new zmq::socket_t*[SUBSCRIBERS_EXPECTED];
    for(int i=0; i<SUBSCRIBERS_EXPECTED; i++)
        broker[i] = new zmq::socket_t(context, ZMQ_PUSH);

    // socket to receive address and port to connect to & synchronization
    zmq::socket_t syncservice(context, ZMQ_REP);
    syncservice.bind((string("tcp://*:") + to_string(SYNC_PORT)).c_str());

    // get synchronization from subscribers
    int subscribers = 0;
    while(subscribers < SUBSCRIBERS_EXPECTED)
    {
        // wait for synchronization request, first [house_id] then [ip:port]
        int house_id = atoi(s_recv(syncservice).c_str());
        string client = s_recv(syncservice);

        if(house_id < SUBSCRIBERS_EXPECTED && house_id >= 0)
            broker[house_id]->connect((string("tcp://") + client).c_str());
        else
        {
            cout<<"error occurred: wrong house id!"<<endl;
            for(int i=0; i<SUBSCRIBERS_EXPECTED; i++)
                delete broker[i];
            delete broker;
            exit(-1);
        }

        s_send(syncservice, "ok");
        subscribers++;
    }

    // read the data stream from file
    ifstream file("priv/temp.csv");
    string buffer;
    unsigned long id;
    unsigned int ts, plug_id, hh_id, house_id;
    float val;
    char prop;

    // @todo what if something is wrong with the message read from file
    while(getline(file, buffer))
    {
        if(sscanf(buffer.c_str(), "%lu,%u,%f,%c,%u,%u,%u", &id, &ts, &val, &prop, &plug_id, &hh_id, &house_id) < 7)
            continue;
        measurement message(ts, val, prop, plug_id, hh_id);

        // send the message
        zmq_send(*broker[house_id], &message, sizeof(measurement), 0);
    }

    // @todo end of communication

    // clean up
    sleep(SLEEP_TIME);
    for(int i=0; i<SUBSCRIBERS_EXPECTED; i++)
        delete broker[i];
    delete broker;
    return 0;
}
