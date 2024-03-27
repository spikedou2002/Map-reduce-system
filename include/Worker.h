#include"Basic.h"
#include<vector>
#include<string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<sys/types.h>
#include<cstring>
#include <unistd.h>
#include <stdio.h>
#include <nlohmann/json.hpp>
class Worker{
    int sock_num;
    struct sockaddr_in server_address;
    char buffer[255]={0,};
    Task task;
    public:
    Worker(){};
    Worker(struct sockaddr_in address);
    void TCP_Connect();
    void request_task();
    void perform();
    void perform_map(Task &t);
    void perform_reduce(Task &t);
    void finish_task();
    ~Worker(){
        close(sock_num);
    }
};