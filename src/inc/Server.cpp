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
#include <ctime>

#ifndef __SERVER_H_INCLUDED__ 
#define __SERVER_H_INCLUDED__
#include "../../include/Server.h"
#endif

Server::Server(int numNodes, int myVal){
	this->numNodes = numNodes;
	this->myVal = myVal;
}

//Server::Server(int numNodes, int myVal){
void Server::CreateSocket(){
	int opt = 1;
	int basePort = 8080;

	// Server::myVal = myVal;
	// Server::numNodes = numNodes;
	myVal = this->myVal;
	numNodes = this->numNodes;

	// Set Node Port
	int port = basePort + myVal;
	//printf("Server Port:: %d\n", port);
	// Create Socket
	if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
    	printf("Socket Failed on node %d", myVal);
    	exit(EXIT_FAILURE);
	}

	//printf("Server:: Socket FD is %d with pid %d\n", serverFd, (int) getpid()	);
     
    // Set Socket param
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        printf("Setsockopt failed on node %d", myVal);
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );

      
    // Bind socket to node port
    if ((bind(serverFd, (struct sockaddr *)&address, 
                                 sizeof(address))) < 0)
    {
        printf("Bind Failed on node %d", myVal);
        exit(EXIT_FAILURE);
	}

	// Listen on port
    if ((listen(serverFd, numNodes)) < 0)
    {
        printf("Listen Failed on node %d", myVal);
        exit(EXIT_FAILURE);
    }

    //printf("Server::Init Complete on Node %d with pid %d\n", myVal, (int) getpid());
};

int Server::SetConnections(){

	int addrlen = sizeof(address);
	int numConnections = 0;
	while(numConnections < numNodes - 1){
		char buffer[1024] = {0};
		int acceptedSocket, nodeNum;
		
		printf("Server::Waiting on accept at time %lu\n", std::time(0));
    	if ((acceptedSocket = accept(serverFd, (struct sockaddr *)&address, 
                   (socklen_t*)&addrlen))<0)
    	{
        	printf("Accept failed on node %d", myVal);
        	exit(EXIT_FAILURE);
    	}

    	printf("Server::Waiting on receive at time %lu\n", std::time(0));
    	
    	int n = recv(acceptedSocket , buffer, 1024, 0);

    	if(n == 0){
    		printf("n = 0 on receive\n");
    	}

    	std::istringstream ss(buffer);
    	if (!(ss >> nodeNum)){
    		std::cerr << "Invalid number " << buffer << '\n';
		}

		printf("Server::Accepted connection from node %d at time %lu\n", nodeNum, std::time(0));
		printf("Server::numConnections %d out of numNodes %d done at time %lu\n", numConnections, numNodes, std::time(0));
		numConnections++;

		this->socketMap[nodeNum] = acceptedSocket;
		this->sendChannel[nodeNum] = std::queue<char*>();
	}

	//printf("Left while loop at time %lu\n", std::time(0));
}


void Server::SendData(int nodeNum){
	char* buffer = this->sendChannel[nodeNum].front();
	strcat(buffer, "END");
	printf("Process %d::Server::Buffer value before send to node %d is %s at time %lu\n", (int) getpid(), nodeNum, buffer, std::time(0) );
	send(this->socketMap[nodeNum], buffer, strlen(buffer), 0);
	this->sendChannel[nodeNum].pop();
}

void Server::ListSockets(){
	for (auto& kv : this->socketMap) {
	    std::cout << kv.first << " has value " << kv.second << std::endl;
	}
}
