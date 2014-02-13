#include <iostream>
#include <fstream>
#include <ctime>
#include "zhelpers.hpp"
#include "common.h"
using namespace std;

#define SUBSCRIBERS_EXPECTED 1

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
    string buffer, house_id;
    char *dup_buffer;
    char* message = new char[sizeof(DP)+1];

    // timestamp-4 value-4, property-1(char), plugid-4, hid-4, \0
    DP dp;

    while(getline(file, buffer))
    {
        // process the data and convert into a message @todo what if the line is not proper?
        dup_buffer = strdup(buffer.c_str());
        strtok(dup_buffer, ",");
        sscanf(strtok(NULL, ","), "%d", &(dp.ts));
        sscanf(strtok(NULL, ","), "%f", &(dp.val));
        sscanf(strtok(NULL, ","), "%c", &(dp.prop));
        sscanf(strtok(NULL, ","), "%d", &(dp.plug_id));
        sscanf(strtok(NULL, ","), "%d", &(dp.hh_id));
        house_id = strtok(NULL, "\n");

        // send the message
        s_sendmore(broker, house_id);
        memcpy(message, &dp, sizeof(DP));
        message[sizeof(DP)] = '\0';
        s_send(broker, message);
    }

    // end of communication
    s_sendmore(broker, "");
    s_send(broker, "END");

    sleep(1);
    delete message;
    free(dup_buffer);
    return 0;
}
