#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "common.h"
#include <cstdio>
using namespace std;

#define SYNC_PORT 5556

// arg: #houses datafile freq
int main(int argc, char *argv[])
{
    int subscribers_expected;
    string data_file;

    if(argc < 4)
    {
        cout<<"not enough arguments!"<<endl;
        exit(-1);
    } else
    {
        subscribers_expected  = atoi(argv[1]);
        data_file = string(argv[2]);
    }

    int listenfd=0, connfd=0, n=0;
    struct sockaddr_in serv_addr;
    char send_buffer[1025];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(send_buffer, '0', sizeof(send_buffer));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SYNC_PORT);

    // listening on the socket
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(listenfd, subscribers_expected);

    int *con_map;
    con_map = new int[subscribers_expected];

//    unsigned id;
    int house_id;

	# define BUFFERLENGTH (128*1024)
	char *buffer[40];
	size_t offset[40] = {0};

	// get synchronization from subscribers
	int subscribers = 0;
	while(subscribers < subscribers_expected)
	{
		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
		if((n = read(connfd, &house_id, sizeof(int))) > 0)
		{
			if(house_id < subscribers_expected && house_id >= 0)
			{
				con_map[house_id] = connfd;
				buffer[house_id] = new char[BUFFERLENGTH];
			} else
			{
				cout<<"error occurred: wrong house id!"<<endl;
				delete[] con_map;
				close(listenfd);
				exit(-1);
			}
		}

		subscribers++;
	}

	FILE * ifile = fopen(data_file.c_str(), "r");
//	measurement m;

	unsigned long count=0, stat=atol(argv[3]);
    timeval ctime = {0}, ptime;
    gettimeofday(&ptime, NULL);

    unsigned int pts = 0;
    size_t bytesWritten = 0;

# define READBUFFERLENGTH (96*1024*1024)
    char *readbuffer = new char[READBUFFERLENGTH];
    size_t readoffset = 0, remaining_bytes = 0;

    struct wrapper {
    	unsigned long id;
    	measurement m;
    	unsigned int house_id;
    }event;

    while(!feof(ifile))
    {
    	remaining_bytes += fread(readbuffer + readoffset, 1, READBUFFERLENGTH - readoffset, ifile);
    	readoffset = 0;
		while (readoffset < remaining_bytes) {
			memcpy(&event, readbuffer+readoffset,sizeof(event));
			readoffset += sizeof(event);

			// send the message
			if ( event.house_id < subscribers )
			{
				memcpy((buffer[event.house_id]+offset[event.house_id]), &event.m, sizeof(measurement));
				offset[event.house_id] += sizeof(event.m);
				if (BUFFERLENGTH - offset[event.house_id] < sizeof(event.m)) {
					do {
						bytesWritten += write(con_map[event.house_id], buffer[event.house_id] + bytesWritten, offset[house_id] - bytesWritten);
					} while (bytesWritten != offset[event.house_id]);
					bytesWritten = 0;
					offset[event.house_id] = 0;
				}
				count++;
			}
			if (event.m.timestamp - pts > 30) {
				for(int i=0; i<subscribers_expected; i++)
					if (offset[i] != 0) {
						do {
							bytesWritten += write(con_map[i], buffer[i] + bytesWritten, offset[i] - bytesWritten);
						} while (bytesWritten != offset[i]);
						bytesWritten = 0;
						offset[i] = 0;
					}
				pts = event.m.timestamp - event.m.timestamp % 30;
			}
			if(count == stat)
			{
				gettimeofday(&ctime, NULL);
				cerr<<"Throughput = "<<(unsigned int)(count*1000000.0/((ctime.tv_sec - ptime.tv_sec)*1000000 + ctime.tv_usec - ptime.tv_usec))<<endl;
				ptime = ctime;
				count = 0;
			}

    	};
		remaining_bytes = 0;
    }
    delete readbuffer;
/*
    while(!feof(ifile))
	{
		if(fscanf(ifile, "%u,%u,%f,%c,%u,%u,%u", &id, &m.timestamp, &m.value, &m.property, &m.plug_id, &m.household_id, &house_id) < 7)
			continue;

		// send the message
		if ( house_id < subscribers )
		{
			memcpy(buffer[house_id]+offset[house_id], &m, sizeof(m));
			offset[house_id] += sizeof(m);
			if (BUFFERLENGTH - offset[house_id] < sizeof(m)) {
				do {
					bytesWritten += write(con_map[house_id], buffer[house_id] + bytesWritten, offset[house_id] - bytesWritten);
				} while (bytesWritten != offset[house_id]);
				bytesWritten = 0;
				offset[house_id] = 0;
			}
			count++;
		}
		if (m.timestamp - pts > 30) {
			for(int i=0; i<subscribers_expected; i++)
				if (offset[i] != 0) {
					do {
						bytesWritten += write(con_map[i], buffer[i] + bytesWritten, offset[i] - bytesWritten);
					} while (bytesWritten != offset[i]);
					bytesWritten = 0;
					offset[i] = 0;
				}
			pts = m.timestamp - m.timestamp % 30;
		}
		if(count == stat)
		{
			gettimeofday(&ctime, NULL);
			cerr<<"Throughput = "<<(unsigned int)(count*1000000.0/((ctime.tv_sec - ptime.tv_sec)*1000000 + ctime.tv_usec - ptime.tv_usec))<<endl;
			ptime = ctime;
			count = 0;
		}
	}
*/

	for(int i=0; i<subscribers_expected; i++) {
		if (offset[i] != 0) {
			do {
				bytesWritten += write(con_map[i], buffer[i] + bytesWritten, offset[i] - bytesWritten);
			} while (bytesWritten != offset[i]);
			bytesWritten = 0;
		}
		delete buffer[i];
		close(con_map[i]);
	}

    delete[] con_map;
    close(listenfd);
}

