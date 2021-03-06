#include <stdio.h>
#include <vector>
#include <queue>

#ifndef __COMMINTERFACE_H_INCLUDED__ 
#define __COMMINTERFACE_H_INCLUDED__
#include "../../include/CommInterface.h"
#endif
//#include "CommInterface.h"
#define MESSAGESIZE 128
#define MAXNODES 	10
#define MAXEVECOUNT	100
#define MAXVECSIZE	MAXNODES*MAXNODES*MAXEVECOUNT

struct Message_t
{
	//std::vector<std::vector<int>> timeMatrix;
	char* timeMatrix;
	char* messageBody;	
	int recvID;
	int srcID;
};

struct MessageCompare
{
	bool operator()(Message_t &m1, Message_t &m2)
	{
		std::vector<std::vector<int>> *m1Mat = reinterpret_cast<std::vector<std::vector<int>>*>(m1.timeMatrix);
		std::vector<std::vector<int>> *m2Mat = reinterpret_cast<std::vector<std::vector<int>>*>(m2.timeMatrix);
		bool precedes = false;
		for(int i=0; i< m1Mat->size(); i++){
			if((*m1Mat)[i][m1.recvID] < (*m2Mat)[i][m2.recvID]){
				precedes = true;
				break;
			}
		}
		return precedes;
	}
};

class Causal
{
private:
	int myID; 
	int numNeighbors;
	std::vector<std::vector<int>> timeMatrix;
	std::vector<char*> receivedMessages;
	std::vector<char*> deliveredMessages;
	std::priority_queue<Message_t, std::vector<Message_t>, MessageCompare> bufferQueue;
	CommInterface* comm;

	char* CreateMessage(char* message, int destID, size_t* sizePtr);
	bool isDelivered(Message_t* newMessage);
	void PerformDelivery(Message_t* newMessage);
	void Debuffer();


public:
	Causal(int numNeighbors, int myID, CommInterface* comm);
	//Causal(const Causal&) = delete;
	void SendCausal(char* message, int nodeNum);
	void SendCausal(char* message, int nodeNum, long unsigned int delay);
	std::vector<char*> GetReceived();
	std::vector<char*> GetDelivered();
	~Causal(){};

};