#ifndef __COMMINTERFACE_H_INCLUDED__ 
#define __COMMINTERFACE_H_INCLUDED__
#include "../../include/CommInterface.h"
#endif

#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <thread>
#include <string>
#include <string.h>
#include <future>
#include <fstream>


void CommInterface::ServerThread(int numNodes, int myVal, std::promise<int>& serverPromise){
    Server *s = new Server(numNodes, myVal);
    this->s = s;
    s->CreateSocket();
    s->SetConnections();
    printf("Process %d::CommInterface::Server Connections Complete\n", (int) getpid());
    serverPromise.set_value(1);
    while(1){};
}

void CommInterface::ClientThread(int numNodes, int myVal, std::promise<int>& clientPromise){
    unsigned int syncTime = 10000000;
    usleep(syncTime);
    Client *c = new Client(numNodes, myVal);
    this->c = c;
    printf("Process %d::CommInterface::Client Connections Complete\n", (int) getpid());
    clientPromise.set_value(1);
    while(1){};
}

void CommInterface::ClientThreadVector(int numNodes, int myVal, std::vector<int> neighbors, std::promise<int>& clientPromise){
    unsigned int syncTime = 10000000;
    usleep(syncTime);
    Client *c = new Client(numNodes, myVal, neighbors);
    this->c = c;
    printf("Process %d::CommInterface Overloaded::Client Connections Complete\n", (int) getpid());
    clientPromise.set_value(1);
    while(1){};
}

void CommInterface::CreateLogger(const char* testBin){
    char loggerFileName[256];
    memset(loggerFileName, '\0', sizeof(loggerFileName));
    char myValStr[2];
    sprintf(myValStr, "%d", myVal);
    char *dirPath(getenv("DistributedSystems"));
    strcat(loggerFileName, dirPath);
    strcat(loggerFileName, "demo");
    strcat(loggerFileName, "/");
    strcat(loggerFileName, testBin);
    strcat(loggerFileName, "/");
    strcat(loggerFileName, "log");
    // strcat(loggerFileName, "/home/saharsh/UTAustin/December/DS/logcomm");
    strcat(loggerFileName, myValStr);
    strcat(loggerFileName, ".out");
    strcpy(this->logName, loggerFileName);    
    this->loggerFile.open(loggerFileName, std::fstream::in | std::fstream::out | std::fstream::app); 
}

CommInterface::CommInterface(int numNodes, int myVal){

    this->numNodes = numNodes;
    this->myVal = myVal;
    std::vector<int> neighbors;
    this->neighbors = neighbors;

    std::promise<int> serverPromise;                      
    std::future<int> serverFuture = serverPromise.get_future();

    std::promise<int> clientPromise;                      
    std::future<int> clientFuture = clientPromise.get_future();

    std::thread t1(&CommInterface::ServerThread, this, numNodes, myVal, std::ref(serverPromise));
    std::thread t2(&CommInterface::ClientThread, this, numNodes, myVal, std::ref(clientPromise));

    t1.detach();
    t2.detach();

    while(serverFuture.get() != 1 || clientFuture.get() != 1){};

    printf("Process %d::CommInterface::Connection Established\n", (int) getpid());
}

CommInterface::CommInterface(int numNodes, int myVal, std::vector<int> neighbors){

    this->numNodes = neighbors.size()+1;
    this->myVal = myVal;
    this->neighbors = neighbors;

    std::promise<int> serverPromise;                      
    std::future<int> serverFuture = serverPromise.get_future();

    std::promise<int> clientPromise;                      
    std::future<int> clientFuture = clientPromise.get_future();

    std::thread t1(&CommInterface::ServerThread, this, this->numNodes, myVal, std::ref(serverPromise));
    std::thread t2(&CommInterface::ClientThreadVector, this, this->numNodes, myVal, neighbors, std::ref(clientPromise));

    t1.detach();
    t2.detach();

    while(serverFuture.get() != 1 || clientFuture.get() != 1){};

    printf("Process %d::CommInterface::Connection Established\n", (int) getpid());
}

void CommInterface::SendData(char* data, int nodeNum){
    char dataCopy[32];
    strcpy(dataCopy, data);
    this->s->sendChannel[nodeNum].push(dataCopy);
    this->s->SendData(nodeNum);
}

void CommInterface::Logger(int nodeNum, char* data){
    char logString[256];
    char infoStr[16];
    memset(infoStr, 0, sizeof(infoStr));

    sprintf(infoStr, "Time::%lu\t", std::time(0));
    strcat(logString, infoStr);
    sprintf(infoStr, "Node::%d\t", nodeNum);
    strcat(logString, infoStr);
    sprintf(infoStr, "Data::%s\n", data);
    strcat(logString, infoStr);

    printf("Process %d::CommInterface::logString is %s\n", (int) getpid(), logString);
    if(this->loggerFile.is_open()){
        this->loggerFile << logString;
        this->loggerFile.flush();
    }
    else{
        printf("Process %d::CommInterface::loggerFile not open\n", (int) getpid());
    }
}

char* CommInterface::ReceiveData(int nodeNum){
    char* buffer = this->c->recvChannel[nodeNum].front();
    this->c->recvChannel[nodeNum].pop();
    return buffer;
}

void CommInterface::BroadcastData(char* data){
    for(auto &node_iter: this->neighbors){
        this->SendData(data, node_iter);
    }
}

std::map<int, char*> CommInterface::PollData(){
    std::map<int, char*> retMap;
    for(auto &node_iter: this->neighbors){
        retMap[node_iter] = this->ReceiveData(node_iter);
        this->Logger(node_iter, retMap[node_iter]);
    }  
    return retMap;  
}