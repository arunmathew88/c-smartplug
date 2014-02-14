#include <iostream>
#include <cstdlib>
#include <fstream>
#include "zhelpers.hpp"
using namespace std;

#define NUM_HOUSE 2
#define DEFAULT_PORT 4000

int main()
{
    zmq::context_t **context = new zmq::context_t*[NUM_HOUSE];

    // socket to talk to clients
    zmq::socket_t **broker = new zmq::socket_t*[NUM_HOUSE];
    for(int i=0; i<NUM_HOUSE; i++)
    {
        context[i] = new zmq::context_t(1);
        broker[i] = new zmq::socket_t(*context[i], ZMQ_PAIR);
        broker[i]->bind((string("tcp://*:") + to_string(DEFAULT_PORT + i)).c_str());
    }

    //  socket to receive signals
    zmq::context_t sync_context(1);
    zmq::socket_t syncservice(sync_context, ZMQ_REP);
    syncservice.bind("tcp://*:5556");

    //  get synchronization from subscribers
    int subscribers = 0;
    while(subscribers < NUM_HOUSE) {
        // wait for synchronization request
        s_recv(syncservice);
        s_send(syncservice, "ok");
        subscribers++;
    }

    // read the data stream from file
    ifstream file("priv/temp.csv");
    string buffer, message;
    int house_id;

    while(getline(file, buffer))
    {
        // need to remove sample id
        unsigned pos_first = buffer.find_first_of(",");

        // need to get the starting point of the house id
        unsigned pos_last = buffer.find_last_of(",");

        // THE message
        message = buffer.substr(pos_first+1, pos_last-pos_first);

        //house id string to int
        house_id = atoi(buffer.substr(pos_last + 1).c_str());

        // send the message
        if(house_id < NUM_HOUSE)
            s_send(*broker[house_id], message);
    }

    // end of communication
    for(int i=0; i<NUM_HOUSE; i++)
        delete broker[i];
    delete broker;

    sleep(1);
    return 0;
}
