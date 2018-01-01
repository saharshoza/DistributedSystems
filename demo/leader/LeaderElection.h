#include <stdio.h>

#ifndef __COMMINTERFACE_H_INCLUDED__ 
#define __COMMINTERFACE_H_INCLUDED__
#include "../../include/CommInterface.h"
#endif
//#include "CommInterface.h"

#define INVITE 1
#define ACCEPT 2
#define LEADER 3

class LeaderElection
{
private:
	int numNeighbors;	// Number of neighboring nodes
	int numChildren;	// Number of neighbors that have sent accept
	int myID; 
	int leaderID;		// LeaderID
	CommInterface* comm;

	void InitiateElection();
	int ProcessMessage(int messageType, int ID);
	void ParseMessage(char* message, int* messageType, int* ID);
	char* CreateMessage(int messageType, int myID);

public:
	LeaderElection(int numNeighbors, int myID, CommInterface* comm);
	//LeaderElection(const LeaderElection&) = delete;
	int RunElection();
	~LeaderElection(){};

};