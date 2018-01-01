#ifndef __LEADERELECTION_H_INCLUDED__ 
#define __LEADERELECTION_H_INCLUDED__
#include "LeaderElection.h"
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
#include <map>

#define MESSAGESIZE 32
#define INVITE 1
#define ACCEPT 2
#define LEADER 3

char* LeaderElection::CreateMessage(int messageType, int myID){
	char *message = (char *)malloc(MESSAGESIZE);
	memset(message, '\0', MESSAGESIZE);
	sprintf(message, "%d %d", messageType, myID);
	return message;
}

void LeaderElection::InitiateElection(){
	char *message = LeaderElection::CreateMessage(INVITE, this->myID);
	this->comm->BroadcastData(message);
}	

int LeaderElection::ProcessMessage(int messageType, int ID){
	if(messageType == INVITE){
		if(ID < this->myID){
			char *message = LeaderElection::CreateMessage(ACCEPT, this->myID);
			this->comm->SendData(message, ID);
		}
		return 0;
	}
	if(messageType == ACCEPT){
		this->numChildren++;
		if(this->numChildren == this->numNeighbors - 1){
			char *message = LeaderElection::CreateMessage(LEADER, this->myID);
			this->comm->BroadcastData(message);
			printf("Process %d::LeaderElection::I am the leader with ID %d\n", (int) getpid(), this->myID );
			this->leaderID = this->myID;
			return 1;
		}
		return 0;
	}
	if(messageType == LEADER){
		printf("Process %d::LeaderElection::Leader has been chosen with ID %d\n", (int) getpid(), ID );
		this->leaderID = ID;
		return 1;
	}

}

void LeaderElection::ParseMessage(char* message, int* messageType, int* ID){
	std::string sep = " ";
	char* current = (char*) malloc(MESSAGESIZE);
	current = strtok(message, sep.c_str());
	*messageType = *current - '0';
	current = strtok(NULL,sep.c_str());
	*ID = *current - '0';
}

int LeaderElection::RunElection(){

	int electionDone = 0;
	std::map<int, char*> pollMap;
	int pollNumber = 0;
	unsigned int syncTime = 5000000;

	LeaderElection::InitiateElection();

	printf("Process %d::LeaderElection::Initiated\n", (int) getpid() );

	while(electionDone == 0){
		usleep(syncTime);
		// 1. Poll nodes
		pollMap = this->comm->PollData();
		for (auto& kv : pollMap) {
			if(kv.second != NULL){
		    	int messageType, ID;
		    	printf("Process %d::LeaderElection::Before ParseMessage\n", (int) getpid() );
		    	this->ParseMessage(kv.second, &messageType, &ID);
		    	printf("Process %d::LeaderElection::ID %d and messageType %d\n", (int) getpid(), ID, messageType );
		    	// 2. a) On receiving invite: Accept if ID less than myID, else ignore
				//	  b) On receiving accept: Increment numChildren
				//							  if numChildren == numNeighbors, send leader message, Done
				//	  c) On receiving leader: Done
		    	electionDone = this->ProcessMessage(messageType, ID);
		    	if(electionDone == 1)
		    		break;
			}
		} 
	}

	return this->leaderID;
}

LeaderElection::LeaderElection(int numNeighbors, int myID, CommInterface* comm){
	
	this->numNeighbors = numNeighbors;
	this->myID = myID;
	this->comm = comm;
	this->leaderID = -1;
	this->numChildren = 0;

	printf("Process %d::LeaderElection::Constructed\n", (int) getpid() );
}