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
#include <common.h>

int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;

    char sendBuff[1025];
    time_t ticks;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listenfd, 10);
    while(1)
    {
        unsigned int c=1;

        std::ifstream ifile("priv/temp.csv");
//      std::ifstream ifile("/home/arunmathew/Downloads/sorted10000e.csv");
        std::string str;
        measurement m;

        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

        ticks = time(NULL);
        while (!ifile.eof()) {
            str.clear();
            getline(ifile, str);
            if (ifile.eof() && !str.length()) break ;
            size_t pos=0;
            while ((pos = str.find_first_of(',',pos))!= str.npos)
                str.replace(pos,1,1,' ');
            std::stringstream ss(str);
            ss >> m.id >> m.timestamp >> m.value >> m.property >> m.plug_id >> m.household_id >> m.house_id;
//          std::cout <<m.id <<std::endl;
//          snprintf(sendBuff, sizeof(sendBuff), "%u %.24s\r\n", c, ctime(&ticks));
//          memcpy(sendBuff, &m, sizeof(m));
            write(connfd, &m, sizeof(m));
//          sleep(1);
//          printf("wrote %u\n", c);
            c++;
    }
        close(connfd);
//        sleep(1);
     }
}

