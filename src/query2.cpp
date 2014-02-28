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
#include "mc.h"
using namespace std;

#define NUM_HOUSE 40
#define NUM_PLUGS 2125
#define MAX_SIZE (2125*24*3600)

#define WINDOW_1HR (3600)
#define WINDOW_24HR (24*3600)

measurement *data = new measurement[MAX_SIZE];
unordered_map<unsigned, unordered_map<unsigned, Mc> > mc[NUM_HOUSE];
Mc global_median;
int num_percentage_more = 0;
int current_index = 0;
int hr_begin = 0;
Mc msc;

void solveQuery2(measurement *m)
{
    data[current_index] = *m;

    while(data[hr_begin].timestamp + WINDOW_1HR < data[current_index].timestamp)
    {
        float old_median = global_median.getMedian();
        global_median.del(data[hr_begin].value);
        float new_median = global_median.getMedian();

        float old_plug_median = mc[m->house_id][m->household_id][m->plug_id].getMedian();
        mc[data[hr_begin].house_id][data[hr_begin].household_id][data[hr_begin].plug_id].del(data[hr_begin].value);
        float new_plug_median = mc[data[hr_begin].house_id][data[hr_begin].household_id][data[hr_begin].plug_id].getMedian();

        hr_begin++;

        if(old_median == new_median && old_plug_median == new_plug_median)
            continue;
        else if(old_median == new_median && old_plug_median != new_plug_median)
        {
            msc.del(old_plug_median);
            msc.insert(new_plug_median);

            if((new_plug_median - new_median >= 0) == (old_plug_median - old_median >= 0))
                continue;
            else if(new_plug_median - new_median >= 0)
                num_percentage_more--;
            else
                num_percentage_more++;
        } else
        {
            if(old_plug_median != new_plug_median)
            {
                msc.del(old_plug_median);
                msc.insert(new_plug_median);
            }

            num_percentage_more = msc.calNumber(new_median);
        }

        // OUTPUT
    }

    // last prediction
    float old_median = global_median.getMedian();
    float old_plug_median = mc[m->house_id][m->household_id][m->plug_id].getMedian();
    if(data[hr_begin].timestamp + WINDOW_1HR == data[current_index].timestamp)
    {
        global_median.del(data[hr_begin].value);
        mc[m->house_id][m->household_id][m->plug_id].del(data[hr_begin].value);
        hr_begin++;
    }

    global_median.insert(m->value);
    mc[m->house_id][m->household_id][m->plug_id].insert(m->value);

    float new_median = global_median.getMedian();
    float new_plug_median = mc[m->house_id][m->household_id][m->plug_id].getMedian();

    if(!(old_median == new_median && old_plug_median == new_plug_median))
    {
        if(old_median == new_median && old_plug_median != new_plug_median)
        {
            msc.del(old_plug_median);
            msc.insert(new_plug_median);

            if(!((new_plug_median - new_median >= 0) == (old_plug_median - old_median >= 0)))
            {
                if(new_plug_median - new_median >= 0)
                    num_percentage_more--;
                else
                    num_percentage_more++;
            }
        } else
        {
            if(old_plug_median != new_plug_median)
            {
                msc.del(old_plug_median);
                msc.insert(new_plug_median);
            }

            num_percentage_more = msc.calNumber(new_median);
        }
    }

    // OUTPUT

    current_index++;
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
            if(nrest == sizeof(measurement) - n)
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
    delete data;
    close(sockfd);
    return 0;
}
