#include <stdio.h>

#ifndef __COMMINTERFACE_H_INCLUDED__ 
#define __COMMINTERFACE_H_INCLUDED__
#include "../../include/CommInterface.h"
#endif
//#include "CommInterface.h"

#define INVITE 1
#define ACCEPT 2
#define REJECT 3

class MST
{
private:
	int parentID;
	std::vector<int> childrenID;
	int numNeighbors;	// Number of neighboring nodes
	int myID; 
	CommInterface* comm;
	int numReplies, numCast, phase1Done;

	void InitiateMST();
	int ProcessMessage(int messageType, int ID);
	void ParseMessage(char* message, int* messageType, int* ID);
	char* CreateMessage(int messageType, int myID);


public:
	MST(int numNeighbors, int myID, CommInterface* comm);
	int GetParent();
	std::vector<int> GetChildren();
	void MSTCovergecast();
	void RunMST();
	~MST(){};
};