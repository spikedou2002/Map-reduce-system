#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include<string>
#include<iostream>
#include "../src/Worker.cpp"
#include <nlohmann/json.hpp>
using namespace std;
int main(){
    int nRet=0;
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd==-1){
        cerr << "Error creating socket." << std::endl;
        return 1;
    }
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(45433); // Use a specific port
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    int server_socket = connect(sockfd, (struct sockaddr*)&server_address, sizeof(server_address));
    if (server_socket < 0) {
        cerr << "Error connecting to the server." << std::endl;
        return 1;
    }
    //cout<<"Successfully connect"<<endl;
    char buffer[255] = {0,};
    int n = recv(sockfd, buffer,sizeof(buffer)-1 , 0);
    if (n < 0) {
        cerr<<"Error receiveing maeesge"<<endl;
        exit(1);
    }
    string received(buffer);
    nlohmann::json j = nlohmann::json::parse(received);
    Task t;
    from_json(j,t);
    cout<<"Task is: "<<t.task_type<<" "<<t.file_id<<" "<<t.Nreduce<<" "<<t.task_id<<" "<<t.Finished<<endl;
    while(1){
        string message;
        cin>>message;
        int s = send(sockfd,message.c_str(),message.length(), 0);
        if(s<0){
            cerr<<"error sending message"<<endl;
        }
        int ret = recv(sockfd, buffer,sizeof(buffer)-1 , 0);
        if (ret < 0) {
            cerr<<"Error receiveing maeesge"<<endl;
            exit(1);
        }
        cout<<"Message is: "<<buffer<<endl;
    }
    
    
}