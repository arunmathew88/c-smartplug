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
    float global_median;
    bool should_exit;

    Node(measurement m = measurement(), Node* n=NULL, float gm=-1, bool se=false)
    : mt(m), next(n), global_median(gm), should_exit(se) {}
};
typedef struct Node Node;

struct ThreadData
{
    Node *mt;
    int house_id;

    ThreadData(Node *n, int h)
    : mt(n), house_id(h) {}
};

void* solveHouse(void *threadarg)
{
    struct ThreadData *my_data = (struct ThreadData*) threadarg;

    // unordered_map<unsigned, unordered_map<unsigned, SlidingMc> > mc[NUM_WINDOWS];
    // int num_percentage_more[NUM_WINDOWS] = {0};
    // SCont msc[NUM_WINDOWS];
    // Node* hr_begin_node[NUM_WINDOWS];

    Node* current_node = my_data->mt;
    while(true)
    {
        if(current_node->should_exit == true)
        {
            delete current_node;
            break;
        } else if(current_node->next)
        {
            cout<<my_data->house_id<<","<<current_node->mt.value<<endl;
            Node *node = current_node;
            current_node = current_node->next;
            delete node;
        }
    }

    pthread_exit(NULL);
}

void solveQuery2(measurement *m, Node** current_node)
{
    current_node[m->house_id]->mt = *m;
    current_node[m->house_id]->next = new Node();
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
    Node **current_node;
    current_node = new Node*[NUM_HOUSE];
    for(int i=0; i<NUM_HOUSE; i++)
        current_node[i] = new Node();

    // creating threads
    pthread_t threads[NUM_HOUSE];
    struct ThreadData* td[NUM_HOUSE];
    pthread_attr_t attr;
    void *status;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for(int h=0; h<NUM_HOUSE; h++)
    {
        td[h] = new struct ThreadData(current_node[h], h);
        int rc = pthread_create(&threads[h], NULL, solveHouse, (void *)td[h]);
        if(rc)
        {
            cout << "Error:unable to create thread," << rc << endl;
            exit(-1);
        }
    }

    measurement *m = new measurement;
    while((n = read(sockfd, m, sizeof(measurement))) > 0)
    {
        if(n == sizeof(measurement))
        {
            solveQuery2(m, current_node);
        } else if(n < sizeof(measurement))
        {
            char b[sizeof(measurement)];
            memcpy(b, &m, n);

            unsigned nrest = read(sockfd, m, sizeof(measurement) - n);
            if(nrest + n == sizeof(measurement))
            {
                memcpy(b+n, &m, nrest);cout<<m->timestamp<<endl;
                solveQuery2(m, current_node);
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

    // send exit signal to threads
    for(int h=0; h<NUM_HOUSE; h++)
    {
        current_node[h]->should_exit = true;
        current_node[h]->next = new Node();
    }

    pthread_attr_destroy(&attr);
    for(int h=0; h<NUM_HOUSE; h++)
    {
        int rc = pthread_join(threads[h], &status);
        if(rc)
        {
            cout << "Error:unable to join," << rc << endl;
            exit(-1);
        }
        delete td[h];
    }

    delete[] current_node;
    delete m;
    close(sockfd);
    return 0;
}
