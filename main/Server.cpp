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
#include"../include/Basic.h"
#include <nlohmann/json.hpp>
using namespace std;
// A server that can handle more than one client at a time
#define port_number 45433
#define num_of_reducer 5


int main(){
    //basic socket settings
    
    nlohmann::json j;
    Task new_task(0,"file",10,0);
    to_json(j,new_task);
    string message = j.dump();
    int listen_sock, addrlen,new_sock,activity,valread;
    int client_sock[5];
    int max_sd;
    struct sockaddr_in address;
    char buffer[1025];  //data buffer of 1K
    fd_set readfds;
    for (int i = 0; i < num_of_reducer; i++)   
    {   
        client_sock[i] = 0;   
    }   
    listen_sock = socket(AF_INET, SOCK_STREAM, 0);//the listening socket
    if(listen_sock<=0){
        cerr << "Error creating TCP listening socket." << std::endl;
         exit(EXIT_FAILURE);  
    }
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons( port_number ); 
    if(bind(listen_sock,(struct sockaddr*)&address,sizeof(address))<0){
        cerr << "Error binding socket." <<endl;
        close(listen_sock);
        return 1;
    }
    if(listen(listen_sock,num_of_reducer)<0){
        cerr << "Error listening." <<endl;
        return 1;
    }
    addrlen = sizeof(address); 
    while(1){
        FD_ZERO(&readfds);
        FD_SET(listen_sock,&readfds);
        max_sd = listen_sock;
        for(int i=0;i<num_of_reducer;i++){
            if(client_sock[i]>0){
                FD_SET(client_sock[i],&readfds);
                cout<<"client"<<i<<"is"<<client_sock[i]<<endl;
            }
            if(client_sock[i]>max_sd){
                max_sd = client_sock[i];
            }
        }
        activity = select(max_sd+1,&readfds,NULL,NULL,NULL);
        if(activity<0){
            cerr<<"select api call failed"<<endl;
            return(EXIT_FAILURE);
        }
        if(FD_ISSET(listen_sock, &readfds)){
            new_sock = accept(listen_sock,(struct sockaddr *)&address, (socklen_t*)&addrlen);
            if (new_sock<0){   
                cerr<<"Error accept";   
                exit(EXIT_FAILURE);   
            }   
            if( send(new_sock, message.c_str(), message.length(), 0) != message.length() ){   
                cerr<<"Error sending message"<<endl;   
            }   
            for (int i = 0; i <num_of_reducer; i++){   
                //if position is empty  
                if( client_sock[i] == 0 )   
                {   
                    client_sock[i] = new_sock;   
                    cout<<"Adding to list of sockets as "<<i<<endl;;   
                         
                    break;   
                }   
            }   
        }
        for (int i = 0; i < num_of_reducer; i++){   
            int sd = client_sock[i];   
            if (FD_ISSET( sd , &readfds)){   
                //Check if it was for closing , and also read the  
                //incoming message  
                if ((valread = read( sd , buffer, 1024)) == 0){   
                    //Somebody disconnected , get his details and print  
                    getpeername(sd , (struct sockaddr*)&address , \ 
                        (socklen_t*)&addrlen);   
                    printf("Host disconnected , ip %s , port %d \n" ,  
                          inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   
                    //Close the socket and mark as 0 in list for reuse  
                    close( sd );   
                    client_sock[i] = 0;   
                }       
                //Echo back the message that came in  
                else 
                {   
                    //set the string terminating NULL byte on the end  
                    //of the data read  
                    buffer[valread] = '\0';
                    cout<<"message is: "<<buffer<<endl;  
                    send(sd , buffer , strlen(buffer) , 0 );   
                }   
            }   
        }   

    }
}


