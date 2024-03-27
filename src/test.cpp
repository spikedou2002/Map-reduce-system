#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include<iostream>
#include<time.h>
#include<chrono>
//#include<stdio.h>
#include "../include/Coordinator.h"
using namespace std;
// A server that can handle more than one client at a time
#define port_number 45433
#define num_of_reducer 10

int main(){
    auto start = std::chrono::high_resolution_clock::now();
    vector<Task>map_task;
    for(int i=0;i<10;i++){
        Task new_task(0,"file",10,num_of_reducer,i);
        //cout<<new_task.NMap<<endl;
        map_task.push_back(new_task);
    }
    const char*message = "Welcome";
    int i=0;
    struct sockaddr_in address;
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons(port_number); 
    Coordinator c(address,10,num_of_reducer,map_task);
    
    c.TCP_Connect();//create a listening socket
    c.TCP_Receive(message);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Total run time: " << duration.count() << " milliseconds" << std::endl;
}