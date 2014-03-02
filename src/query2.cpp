#include <iostream>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "common.h"
#include "slidingmc.h"
#include "scont.h"
using namespace std;

enum Window
{
    WINDOW_1HR = 0,
    WINDOW_24HR,
    NUM_WINDOWS
};

unsigned getWindowSize(Window ws)
{
    switch(ws)
    {
        case WINDOW_1HR:
            return 60*60;
        case WINDOW_24HR:
            return 24*3600;
        default:
            cout<<"error occured!"<<endl;
            exit(-1);
    }
}

struct Node
{
    measurement mt;
    Node* next;

    Node(measurement m = measurement(), Node* n=NULL)
    : mt(m), next(n) {}
};
typedef struct Node Node;

unordered_map<unsigned, unordered_map<unsigned, SlidingMc> > mc[NUM_WINDOWS][NUM_HOUSE];
SlidingMc global_median[NUM_WINDOWS];
int num_percentage_more[NUM_WINDOWS] = {0};
SCont msc[NUM_WINDOWS];
Node* hr_begin_node[NUM_WINDOWS];

Node* data;
Node* current_node;

void solveQuery2(measurement *m)
{
    current_node->mt = *m;
    current_node->next = new Node();

    for(int i=0; i<NUM_WINDOWS; i++)
    {
        Window ws = (Window)i;
        while(true)
        {
            int old_percentage = num_percentage_more[i];

            unsigned house_id = hr_begin_node[i]->mt.house_id;
            unsigned household_id =  hr_begin_node[i]->mt.household_id;
            unsigned plug_id =  hr_begin_node[i]->mt.plug_id;

            // initializing
            mc[i][house_id][household_id][plug_id];
            mc[i][m->house_id][m->household_id][m->plug_id];

            float old_median = global_median[i].getMedian();
            float old_plug_median = (mc[i][house_id][household_id][plug_id]).getMedian();

            if(hr_begin_node[i]->mt.timestamp + getWindowSize(ws) <= current_node->mt.timestamp)
            {
                global_median[i].del(hr_begin_node[i]->mt.value);
                mc[i][house_id][household_id][plug_id].del(hr_begin_node[i]->mt.value);
            }

            if(hr_begin_node[i]->mt.timestamp + getWindowSize(ws) >= current_node->mt.timestamp)
            {
                global_median[i].insert(m->value);
                mc[i][m->house_id][m->household_id][m->plug_id].insert(m->value);
            }

            float new_median = global_median[i].getMedian();
            float new_plug_median = mc[i][house_id][household_id][plug_id].getMedian();

            if(old_median == new_median && old_plug_median == new_plug_median) {}
            else if(old_median == new_median && old_plug_median != new_plug_median)
            {
                msc[i].insert(house_id, household_id, plug_id, new_plug_median, old_plug_median);

                if((new_plug_median - new_median >= 0) == (old_plug_median - old_median >= 0)) {}
                else if(new_plug_median - new_median >= 0)
                    num_percentage_more[i]--;
                else
                    num_percentage_more[i]++;
            } else
            {
                if(old_plug_median != new_plug_median)
                {
                    msc[i].insert(house_id, household_id, plug_id, new_plug_median, old_plug_median);
                }

                num_percentage_more[i] = msc[i].getNumOfLargeNum(new_median);
            }

            Node* old_hr_begin_node = hr_begin_node[i];
            unsigned ts = hr_begin_node[i]->mt.timestamp;
            hr_begin_node[i] = hr_begin_node[i]->next;
            if(i == NUM_WINDOWS)
                delete old_hr_begin_node;

            if(old_percentage != num_percentage_more[i])
                cout<<num_percentage_more[i]<<endl;

            if(ts + getWindowSize(ws) > current_node->mt.timestamp)
                break;
        }
    }

    current_node = current_node->next;
}

// arg: broker_ip port
int main(int argc, char *argv[])
{
    int port;

    if(argc < 3)
    {
        cout<<"not enough arguments!"<<endl;
        exit(-1);
    } else
        port = atoi(argv[2]);

    int sockfd = 0;
    unsigned n = 0;
    char recv_buff[1024];
    struct sockaddr_in serv_addr;

    memset(recv_buff, '0',sizeof(recv_buff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    }

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    }

    // init
    data = new Node();
    current_node = data;
    for(int i=0; i<NUM_WINDOWS; i++)
        hr_begin_node[i] = data;

    measurement *m = new measurement;
    while((n = read(sockfd, m, sizeof(measurement))) > 0)
    {
        if(n == sizeof(measurement))
        {
            solveQuery2(m);
        } else if(n < sizeof(measurement))
        {
            char b[sizeof(measurement)];
            memcpy(b, &m, n);

            unsigned nrest = read(sockfd, m, sizeof(measurement) - n);
            if(nrest + n == sizeof(measurement))
            {
                memcpy(b+n, &m, nrest);cout<<m->timestamp<<endl;
                solveQuery2(m);
            } else
            {
                cout<<"Error: should not reah here lineno:"<<__LINE__<<"!"<<endl;
                delete m;
                close(sockfd);
                exit(-1);
            }
        } else
        {
            cout<<"Error: should not reah here lineno:"<<__LINE__<<"!"<<endl;
            delete m;
            close(sockfd);
            exit(-1);
        }
    }

    delete m;
    close(sockfd);
    return 0;
}
