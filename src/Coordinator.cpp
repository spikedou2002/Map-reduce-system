//Implementation of Coordinator
#include "../include/Coordinator.h"


using namespace std;

//const char *message = "welcome";
Coordinator::Coordinator(){
    stage = 0 ;
    
    for(int i=0;i<client_accept;i++){
        client_sock[i]=0;
    }
    memset(buffer, 0, sizeof(buffer));
}
Coordinator::Coordinator(struct sockaddr_in address, int map_num,int reduce_num,vector<Task>&map_task)
: stage(0), num_of_mapper(map_num),num_of_reducer(reduce_num),
  address(address), 
  addrlen(sizeof(address)),
  map_tasks(map_task),
  map_assign(map_task.begin(),map_task.end()),
  com_time(0){
    for(int i = 0; i < client_accept; i++) {
        client_sock[i] = 0;
    }
    memset(buffer, 0, sizeof(buffer));
    task_remain = map_task.size();
}
void Coordinator::TCP_Connect(){
    
    master_sock = socket(AF_INET, SOCK_STREAM, 0);
    
    if(master_sock<=0){
        cerr << "Error creating TCP listening socket." << std::endl;
        exit(EXIT_FAILURE);  
    }
    int i = bind(master_sock, (struct sockaddr*)&address, sizeof(address));
    if(i < 0){
        cerr << "Error binding socket." <<i<<endl;
        close(master_sock);
        exit(1);
    }
    if(listen(master_sock,client_accept)<0){
        cerr << "Error listening." <<endl;
        close(master_sock);
        return;
    }
   
}
void Coordinator::TCP_Receive(const char*message){
    while(stage!=2){
        FD_ZERO(&readfds);
        FD_SET(master_sock,&readfds);
        max_sd = master_sock;
        for(int i=0;i<client_accept;i++){
            if(client_sock[i]>0){
                FD_SET(client_sock[i],&readfds);
            }
            if(client_sock[i]>max_sd){
                max_sd = client_sock[i];
            }
        }
        int activity = select(max_sd+1,&readfds,NULL,NULL,NULL);
        if(activity<0){
            perror("Select call failed");
            exit(1);
        }
        if(FD_ISSET(master_sock, &readfds)){
            auto start = std::chrono::high_resolution_clock::now();
            int new_sock = accept(master_sock,(struct sockaddr *)&address, (socklen_t*)&addrlen);
            if (new_sock<0){   
                cerr<<"Error accept";   
                exit(EXIT_FAILURE);   
            }   
            if( send(new_sock, message, strlen(message), 0) != strlen(message) ){   
                cerr<<"Error sending message"<<endl;   
            }  
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
            com_time+=duration;
            for (int i = 0; i <client_accept; i++){   
                //if position is empty  
                if( client_sock[i] == 0 ){   
                    client_sock[i] = new_sock;   
                    cout<<"Adding to list of sockets as "<<i<<endl;;   
                    break;}   
            }
        }//recieve a new client
        
        TCP_Handle(); 
        
    }
}
void Coordinator::TCP_Handle(){
    //number of reduce

    for(int i=0;i<client_accept;i++){
        
        int sd = client_sock[i];
        if(FD_ISSET( sd , &readfds)){
            memset(buffer,0,sizeof(buffer)-1);
            auto start = std::chrono::high_resolution_clock::now();
            int val = read( sd , buffer,1024);
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
            com_time+=duration;
            if(val==0){
                cout<<"client"<<sd<<"is disconnected"<<endl;
                close( sd );   
                client_sock[i] = 0;
            }

            else{
                string s(buffer);
                if(s=="request"){
                    assign_task(sd);
                }
                else if(s.substr(0,6)=="finish"){
                    auto id = stoi(s.substr(6));
                    cout<<"task"<<id<<"is finished"<<endl;
                    task_finished(id);
                }
            }
        }

    }
}

void Coordinator::assign_task(int sock){ 
    if(error_check()){
            reassign_task(sock);
            return;
    }
    if(stage==0){// stage=0 map status 1 reduce status
       
        if(map_assign.empty()){
            char* c = "done";
            auto start = std::chrono::high_resolution_clock::now();
            send(sock,c,strlen(c),0);
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
            com_time+=duration;
            return;
        }
        Task assigned = map_assign.front();
        map_assign.pop_front();
        nlohmann::json j;
        to_json(j,assigned);
        string c = j.dump();
        auto start = std::chrono::high_resolution_clock::now();
        send(sock,c.c_str(),c.length(),0);
        auto stop = std::chrono::high_resolution_clock::now();
        unfinished.insert(make_pair(assigned.task_id,stop));
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
        com_time+=duration;
        //std::cout << "Total communication time: " << duration.count() << " nanoseconds" << std::endl;
    }
    else if(stage==1){
        if(reduce_assign.empty()){
            char* c = "done";
            auto start = std::chrono::high_resolution_clock::now();
            send(sock,c,strlen(c),0);
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
            com_time+=duration;
            return;
        }
        Task assigned = reduce_assign.front();
        reduce_assign.pop_front();
        nlohmann::json j;
        to_json(j,assigned);
        string c = j.dump();
        auto start = std::chrono::high_resolution_clock::now();
        send(sock,c.c_str(),c.length(),0);
        auto stop = std::chrono::high_resolution_clock::now();
        unfinished.insert(make_pair(assigned.task_id,stop));
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
        com_time+=duration;
        //std::cout << "Total communication time: " << duration.count() << " nanoseconds" << std::endl;
    }
}

void Coordinator::task_finished(int id){
    task_remain--;
    unfinished.erase(id);
    if(task_remain<=0){
        if(stage==0){
            stage=1;
            task_remain = num_of_reducer;
            for(int i=0;i<num_of_reducer;i++){
                string filename = "file";
                Task temp(1,filename,num_of_mapper,10,i);
                reduce_assign.push_back(temp);
                reduce_tasks.push_back(temp);  
            }
        }
        else if(stage==1){
            cout<<"All the tasks are finished"<<endl;
            //exit(0);
            std::cout << "Total communication time: " << com_time.count() << " nanoseconds" << std::endl;
            stage=2;
        }
    }
}
bool Coordinator::error_check(){
    if(unfinished.empty())return false;
    auto end = std::chrono::high_resolution_clock::now();
    auto early = unfinished.begin();
    auto duration = chrono::duration_cast<chrono::seconds>(end - early->second);
    if(duration>chrono::seconds(20)){
        cout<<"errors occured in the worker"<<endl;
        return true;
    }
    else return false;
}
void Coordinator::reassign_task(int sock){
    auto early = unfinished.begin();
    auto end = std::chrono::high_resolution_clock::now();
    if(stage==0){
        int id = (early->first)%num_of_mapper;
        Task r = map_tasks[id];
        r.task_id = early->first + num_of_mapper; //update the id of the tasks
        nlohmann::json j;
        to_json(j,r);
        string c = j.dump();
        send(sock,c.c_str(),c.length(),0);//reassign the tasks
        unfinished.erase(early);
        unfinished.insert(make_pair(r.task_id,end));//update the unfinished set 
        cout<<"Map task " <<r.task_id<<" reassigned"<<endl;
    }
    else if(stage==1){
        int id = (early->first)%num_of_reducer;
        Task r = reduce_tasks[id];
        r.task_id = early->first + num_of_reducer; //update the id of the tasks
        nlohmann::json j;
        to_json(j,r);
        string c = j.dump();
        send(sock,c.c_str(),c.length(),0);
        unfinished.erase(early);
        unfinished.insert(make_pair(r.task_id,end));//update the unfinished set 
        cout<<"Reduce task " <<r.task_id<<" reassigned"<<endl;
    }
}
