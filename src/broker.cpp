#include <iostream>
#include <fstream>
#include "zhelpers.hpp"
using namespace std;

#define SUBSCRIBERS_EXPECTED 40
#define NUM_THREADS 10
#define SYNC_PORT 5556
#define SLEEP_TIME 1

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
    string buffer, house_id, message;

    // @todo what if something is wrong with the message read from file
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

        // send the message
        s_send(*broker[atoi(house_id.c_str())], message);
    }

    // @todo end of communication

    // clean up
    sleep(SLEEP_TIME);
    for(int i=0; i<SUBSCRIBERS_EXPECTED; i++)
        delete broker[i];
    delete broker;
    return 0;
}
