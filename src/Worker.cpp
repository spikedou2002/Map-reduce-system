//Implementation of Worker

#include "../include/Worker.h"

using namespace std;
Worker::Worker(struct sockaddr_in address):server_address(address){}

void Worker::TCP_Connect(){
    this->sock_num = socket(AF_INET,SOCK_STREAM,0);
    if(sock_num==-1){
        cerr << "Error creating socket." << endl;
        return ;
    }
    int server_socket = connect(sock_num, (struct sockaddr*)&server_address, sizeof(server_address));
    if (server_socket < 0) {
        cerr << "Error connecting to the server." <<endl;
        return ;
    }
    int n = recv(sock_num, buffer,sizeof(buffer)-1 , 0);
    if (n < 0) {
        cerr<<"Error receiveing maeesge"<<endl;
        exit(1);
    }
    cout<<"Message is: "<<buffer<<endl;
    //while(1){
      //  request_task();
    //}
}

void Worker::request_task(){
    char* message = "request";
    int ret = send(sock_num, message, strlen(message), 0);
    if(ret<0){
        perror("Error sending message");
        //exit(1);
    }
    //clear the buffer
    for(int i=0;i<255;i++){
        buffer[i] = 0;
    }
    ret = recv(sock_num, buffer,sizeof(buffer)-1 , 0);
    if (ret < 0) {
        cerr<<"Error receiveing maeesge"<<endl;
        exit(1);
    }
    //cout<<"Message is: "<<buffer<<endl;
    
    string temp(buffer);
    if(temp=="done"){
        task.task_type = 3;
        return;
    }
    nlohmann::json j = nlohmann::json::parse(temp);
    from_json(j,task);
    //cout<<"task is: "<<(task=="done")<<endl;
}
void Worker::perform(){
    if(task.task_type==3){
        cout<<"No task currently"<<endl;
        sleep(2);
        return;
    }
    if(task.task_type==0){
        perform_map(task);
        finish_task();
    }
    else if(task.task_type==1){
        perform_reduce(task);
        finish_task();
    }
    else{
        cerr<<"Task type error"<<endl;
    }
    //finish_task();
}
void Worker::perform_map(Task &t){
    cout<<"Performing map function"<<endl;
    //cout<<t.NMap<<endl;
    int id = t.task_id%t.NMap;
    cout<<"Task is: "<<t.task_id<<" "<<t.Finished<<endl;
    sleep(5);
    
}

void Worker::perform_reduce(Task &t){
    cout<<"Performing reduce function"<<endl;
    int id = t.task_id%t.Nreduce;
    cout<<"Task is: "<<t.task_id<<" "<<endl;
    sleep(5);
}
void Worker::finish_task(){
    string message = "finish"+to_string(task.task_id);
    int ret = send(sock_num, message.c_str(), message.length(), 0);
    if(ret<0){
        perror("Error sending message");
        exit(1);
    }
}
