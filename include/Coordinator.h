#include"Basic.h"
#include<deque>
#include <nlohmann/json.hpp>
#include<vector>
#include<string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include<stdio.h>
#include<cstring>
#include<time.h>
#include<chrono>
#include <unordered_map>
//#define MAX_CLIENT_NUM 30
using namespace std;
class Coordinator{
vector<Task> map_tasks;
deque<Task> map_assign;
vector<Task> reduce_tasks;
deque<Task> reduce_assign;
map<int, chrono::time_point<chrono::high_resolution_clock>> unfinished;
int stage;
int num_of_mapper;
int num_of_reducer;
int task_remain;
//variables for internet connections
int master_sock;
int client_sock[client_accept];
char buffer[1024];
int max_sd;
struct sockaddr_in address;
fd_set readfds;
int addrlen = sizeof(address); 
std::chrono::nanoseconds com_time;
public:
    Coordinator();
    Coordinator(struct sockaddr_in address,int map_num,int reduce_num,vector<Task>&map_tasks);
    void TCP_Connect();
    void TCP_Receive(const char*message);
    void TCP_Handle();
    void assign_task(int sock);//called when a worker request a task
    //void state_check();//check whether to switch stages
    void task_finished(int id);//called when a worker finishes task
    bool error_check();//check for errors
    void reassign_task(int sock);//task reassignment if errors occur
    ~Coordinator(){
        close(master_sock);
    }
    //functions for TCP connections
};