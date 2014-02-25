#include <iostream>
#include <sys/socket.h>
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

// arg: house_id broker_ip port
int main(int argc, char *argv[])
{
    string house_id_str, broker_ip;
    int house_id, port;

    if(argc < 4)
    {
        cout<<"not enough arguments!"<<endl;
        exit(-1);
    } else
    {
        house_id_str  = string(argv[1]);
        house_id = atoi(house_id_str.c_str());
        broker_ip = string(argv[2]);
        port = atoi(argv[3]);
    }
    cout<<"house running with id :"<<house_id_str<<endl;

    int sockfd = 0, n = 0;
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

    if(inet_pton(AF_INET, argv[2], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    }

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    }

    // informing about the house id to the broker process
    cout<<write(sockfd, &house_id, sizeof(house_id))<<endl;

    measurement *m = new measurement;
    while((n = read(sockfd, m, sizeof(measurement))) > 0)
    {
        if(n == sizeof(measurement))
        {
            cout<<m->timestamp<<endl;
        } else if(n < sizeof(measurement))
        {
            char b[sizeof(measurement)];
            memcpy(b, &m, n);

            int nrest = read(sockfd, m, sizeof(measurement) - n);
            if(nrest == sizeof(measurement) - n)
            {
                memcpy(b+n, &m, nrest);
                cout<<m->timestamp<<endl;
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

