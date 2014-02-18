#include <iostream>
#include <fstream>
#include "zhelpers.hpp"
using namespace std;

#define SUBSCRIBERS_EXPECTED 40

int main()
{
    zmq::context_t context(10);

    //  socket to talk to clients
    zmq::socket_t **broker = new zmq::socket_t*[SUBSCRIBERS_EXPECTED];
    for(int i=0; i<SUBSCRIBERS_EXPECTED; i++)
    {
        broker[i] = new zmq::socket_t(context, ZMQ_PUSH);
        broker[i]->bind((string("tcp://*:") + to_string(4000 + i)).c_str());
    }

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
        // if(house_id.length() == 1)
        //     house_id = string("0") + house_id;

        // send the message
        //s_sendmore(broker, house_id);
        s_send(*broker[atoi(house_id.c_str())], message);
        cout<<"."; cout.flush();
    }

    // end of communication
    // s_sendmore(broker, "");
    // s_send(broker, "END");

    sleep(1);
    for(int i=0; i<SUBSCRIBERS_EXPECTED; i++)
    {
        delete broker[i];
    }
    delete broker;
    return 0;
}
