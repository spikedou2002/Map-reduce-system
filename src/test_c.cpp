#include "../include/Worker.h"
#include<iostream>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
using namespace std;

int main(){
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(45433); // Use a specific port
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    Worker w(server_address);
    w.TCP_Connect();
    while(1){
        w.request_task();
        w.perform();
    }
}