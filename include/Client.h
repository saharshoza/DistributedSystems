#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <queue> 
#include <map> 
#include <thread>

#ifndef __SHAREDQUEUE_H_INCLUDED__ 
#define __SHAREDQUEUE_H_INCLUDED__
#include "SharedQueue.h"
#endif

class Client
{

private:
    int myVal, numNodes;
    std::vector<int> neighbors;
    //void RecvData(int nodeNum, std::map<int, std::promise<char*>>& promiseMap);
    void RecvData(int nodeNum);

public:
    //std::map<int, std::queue<char*>> recvChannel;
    //std::map<int, SharedQueue<char*>> recvChannel;
    std::map<int, SharedQueue> recvChannel;
    std::map<int, int> socketMap;
    std::map<int, std::thread> threadMap;

	//Client(int numNodes, int myVal, std::map<int, std::promise<char*>>& promiseMap);
	Client(int numNodes, int myVal);
    Client(int numNodes, int myVal, std::vector<int> neighbors);
	void ListSockets();
	~Client(){};
	
};