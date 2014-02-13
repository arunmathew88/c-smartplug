#include <iostream>
#include <fstream>
#include "zhelpers.hpp"
using namespace std;

#define SUBSCRIBERS_EXPECTED 40

int main()
{
    zmq::context_t context(1);

    //  socket to talk to clients
    zmq::socket_t broker(context, ZMQ_PUB);
    broker.bind("tcp://*:5555");

    //  socket to receive signals
    zmq::socket_t syncservice(context, ZMQ_REP);
    syncservice.bind("tcp://*:5556");

    //  get synchronization from subscribers
    int subscribers = 0;
    while(subscribers < SUBSCRIBERS_EXPECTED) {
        // wait for synchronization request
        s_recv(syncservice);
        s_send(syncservice, "ok");
        subscribers++;
    }

    // read the data stream from file
    ifstream file("priv/temp.csv");
    string buffer, house_id, message;

    while(getline(file, buffer))
    {
        // need to remove sample id
        unsigned pos_first = buffer.find_first_of(",");

        // need to get the starting point of the house id
        unsigned pos_last = buffer.find_last_of(",");

        // THE message
        message = buffer.substr(pos_first+1, pos_last-pos_first);

        //house id string to int
        house_id = buffer.substr(pos_last + 1);
        if(house_id.length() == 1)
            house_id = string("0") + house_id;

        // send the message
        s_sendmore(broker, house_id);
        s_send(broker, message);
    }

    // end of communication
    s_sendmore(broker, "");
    s_send(broker, "END");

    sleep(1);
    return 0;
}
