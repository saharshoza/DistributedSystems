#ifndef __SERVER_H_INCLUDED__ 
#define __SERVER_H_INCLUDED__
#include "Server.h"
#endif

#ifndef __CLIENT_H_INCLUDED__ 
#define __CLIENT_H_INCLUDED__
#include "Client.h"
#endif

#include <future>
#include <fstream>

class CommInterface
{
public:
	Server* s;
	Client* c;
	std::ofstream loggerFile;
	std::map<int, std::queue<int>> delayMap;
	std::vector<int> neighbors;
	int numNodes, myVal;

	CommInterface(int numNodes, int myVal);
	CommInterface(int numNodes, int myVal, std::vector<int> neighbors);
	CommInterface();
	CommInterface& operator=(CommInterface other);
	//CommInterface(const CommInterface&) = delete;
	void ClientThread(int numNodes, int myVal, std::promise<int>& clientPromise);
	void ClientThreadVector(int numNodes, int myVal, std::vector<int> neighbors, std::promise<int>& clientPromise);
	void ServerThread(int numNodes, int myVal, std::promise<int>& serverPromise);
	void SendData(char* data, int nodeNum);
	char* ReceiveData(int nodeNum);
	void BroadcastData(char* data);
	void Logger(int nodeNum, char* data);
	std::map<int, char*> PollData();
	~CommInterface(){};
	
};