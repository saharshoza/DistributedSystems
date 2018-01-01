#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <queue> 
#include <map>
#include <thread>
#include <ctime>

#ifndef __CLIENT_H_INCLUDED__ 
#define __CLIENT_H_INCLUDED__
#include "../../include/Client.h"
#endif

#define STOPSEQ "END"
#define MESSAGESIZE 64

void Client::RecvData(int nodeNum){
    char* residualBuffer = (char*) malloc(5*MESSAGESIZE);
    memset(residualBuffer, '\0', sizeof(residualBuffer));
    int messageNum = 0;
	while(1){
		int n;
        int append = 0;
		char* buffer = (char*) malloc(MESSAGESIZE);
		memset(buffer, '\0', sizeof(buffer));
        //printf("Process %d::Client::Client Channel holds (front) %s from node %d at time %lu\n", (int) getpid(), this->recvChannel[nodeNum].front(), nodeNum, std::time(0) );
		n = recv(this->socketMap[nodeNum], buffer, sizeof(buffer), 0);
		if(n > 0){
            // If last receive was incomplete, append this to that
            residualBuffer = strcat(residualBuffer, buffer);
            //printf("Process %d::Client::Client appended message %s from node %d at time %lu\n", (int) getpid(), residualBuffer, nodeNum, std::time(0) );

            // Check if the concatenated message is complete. If not, receive again
            std::string checkStr(residualBuffer);
            int pos = checkStr.find_last_of(STOPSEQ);
            if(pos+1 != strlen(residualBuffer)){
                //append = 1;
                printf("Process %d::Client::Client had to redo recv as buffer is %s from node %d at time %lu\n", (int) getpid(), residualBuffer, nodeNum, std::time(0) );
                //printf("Process %d::Client::Client redo since pos:%d, stopsize:%lu, ressize:%lu from node %d at time %lu\n", (int) getpid(), pos, strlen(STOPSEQ), strlen(residualBuffer), nodeNum, std::time(0) );
                continue;
            }

            // This message was complete. Parse to identify individual messages
            //printf("Process %d::Client::Client Channel holds %s from node %d at time %lu\n", (int) getpid(), buffer, nodeNum, std::time(0) );
            printf("Process %d::Client::Client Channel holds %s from node %d at time %lu\n", (int) getpid(), residualBuffer, nodeNum, std::time(0) );
			std::string sep = "END";
			
	    	char* current = strtok(residualBuffer, sep.c_str());
            char* currentCopy = (char*) malloc(sizeof(current));
            memset(currentCopy, '\0', sizeof(currentCopy));
            strcpy(currentCopy, current);
	    	while(current!=NULL){
                char* currentCopy = (char*) malloc(sizeof(current));
                memset(currentCopy, '\0', sizeof(currentCopy));
                strcpy(currentCopy, current);
	       		this->recvChannel[nodeNum].push(currentCopy);
                messageNum++;
	       		printf("Process %d::Client::Client Channel holds %s from node %d. messageNum %d at time %lu\n", (int) getpid(), currentCopy, nodeNum, messageNum, std::time(0) );
	        	current = strtok(NULL,sep.c_str());
    		}

            // Clear residual Buffer and reset append to start fresh
            memset(residualBuffer, '\0', sizeof(residualBuffer));
            //append = 0;
		}
		else if(n < 0){
			printf("Error on thread for node %d with error code %d\n", nodeNum, n);
			printf("%s\n", strerror(errno));
			break;
		}
		else{
			printf("n = 0 on thread for node %d at time %lu\n", nodeNum, std::time(0));
			break;
		}
	}
}

void Client::ListSockets(){
	for (auto& kv : this->socketMap) {
	    std::cout << kv.first << " has value " << kv.second << std::endl;
	}
}

Client::Client(int numNodes, int myVal){
	struct sockaddr_in address;
	int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    int basePort = 8080;
	this->numNodes = numNodes;
	this->myVal = myVal;

	char* portStr = (char*) malloc(sizeof(int));
	sprintf(portStr, "%d", this->myVal);

	for(int server_iter=0; server_iter < numNodes; server_iter++){
    	if(server_iter != myVal){

    		// Set Node Port
			int port = basePort + server_iter;
			//printf("Client Port:: %d\n", port);
            char buffer[1024] = {0};
            int clientFd;

            if ((clientFd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
            {
                printf("\n Socket creation error \n");
           		//exit(EXIT_FAILURE);
            }
            //printf("Client:: Client FD is %d\n", clientFd);
          
        	memset(&address, '0', sizeof(address));
      
            address.sin_family = AF_INET;
            address.sin_port = htons(port);
          
            // Convert IPv4 and IPv6 addresses from text to binary form
            if((inet_pton(AF_INET, "127.0.0.1", &address.sin_addr)) <= 0) 
            {
                printf("\nInvalid address/ Address not supported \n");
                //exit(EXIT_FAILURE);
            }

            if ((connect(clientFd, (struct sockaddr *)&address, sizeof(address))) < 0)
            {
            	printf("%s\n", strerror(errno));
                printf("\nConnection Failed \n");
                //exit(EXIT_FAILURE);
            }
            else{
	            send(clientFd , portStr , strlen(portStr) , 0 );

	            //printf("Client::Connected with node %d at time %lu\n", server_iter, std::time(0));

	            this->socketMap[server_iter] = clientFd;
	            //this->recvChannel[server_iter] = std::queue<char*>();
	            //this->recvChannel[server_iter] = SharedQueue<char*>();
	            this->threadMap[server_iter] = std::thread(&Client::RecvData, this, server_iter);
	            //this->threadMap[server_iter] = std::thread(&Client::RecvData, this, server_iter, promiseMap);
            }
    	}
	}
};

Client::Client(int numNodes, int myVal, std::vector<int> neighbors){
	struct sockaddr_in address;
	int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    int basePort = 8080;
	this->numNodes = numNodes;
	this->myVal = myVal;
	this->neighbors = neighbors;

	char* portStr = (char*) malloc(sizeof(int));
	sprintf(portStr, "%d", this->myVal);

	for(int &server_iter: neighbors){
		// Set Node Port
		int port = basePort + server_iter;
		//printf("Client Port:: %d\n", port);
        char buffer[1024] = {0};
        int clientFd;

        if ((clientFd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("\n Socket creation error \n");
       		//exit(EXIT_FAILURE);
        }
        //printf("Client:: Client FD is %d\n", clientFd);
      
    	memset(&address, '0', sizeof(address));
  
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
      
        // Convert IPv4 and IPv6 addresses from text to binary form
        if((inet_pton(AF_INET, "127.0.0.1", &address.sin_addr)) <= 0) 
        {
            printf("\nInvalid address/ Address not supported \n");
            //exit(EXIT_FAILURE);
        }

        if ((connect(clientFd, (struct sockaddr *)&address, sizeof(address))) < 0)
        {
        	printf("%s\n", strerror(errno));
            printf("\nConnection Failed \n");
            //exit(EXIT_FAILURE);
        }
        else{
            send(clientFd , portStr , strlen(portStr) , 0 );

            //printf("Client::Connected with node %d at time %lu\n", server_iter, std::time(0));

            this->socketMap[server_iter] = clientFd;
            //this->recvChannel[server_iter] = std::queue<char*>();
            //this->recvChannel[server_iter] = SharedQueue<char*>();
            this->threadMap[server_iter] = std::thread(&Client::RecvData, this, server_iter);
            //this->threadMap[server_iter] = std::thread(&Client::RecvData, this, server_iter, promiseMap);
		}
	}
};
