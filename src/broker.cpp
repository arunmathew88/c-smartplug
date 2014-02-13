#include <iostream>
#include <fstream>
#include <ctime>
#include "zhelpers.hpp"
using namespace std;

#define SUBSCRIBERS_EXPECTED 1
#define MESSAGE_SIZE (sizeof(int) + sizeof(float) + sizeof(bool) + sizeof(int) + sizeof(int) + 1)

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
    int ts, plug_id, hh_id;
    float val;
    char prop;
    char *dup_buffer;

    while(getline(file, buffer))
    {
        // timestamp-4 value-4, property-1(char), plugid-4, hid-4, \0
        char *message = new char[100];

        // process the data and convert into a message
/* check ofr NULL */
        dup_buffer = strdup(buffer.c_str());
        strtok(dup_buffer, ",");
        sscanf(strtok(NULL, ","), "%d", &ts);
        sscanf(strtok(NULL, ","), "%f", &val);
        sscanf(strtok(NULL, ","), "%c", &prop);
        sscanf(strtok(NULL, ","), "%d", &plug_id);
        sscanf(strtok(NULL, ","), "%d", &hh_id);
        house_id = strtok(NULL, "\n");
        sprintf(message, "%d%f%c%d%d", ts, val, prop, plug_id, hh_id);


        // send the message
        s_sendmore(broker, house_id);
        s_send(broker, message);
    }

    s_send(broker, "END");
    sleep(1);
    return 0;
}
