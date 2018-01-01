#ifndef __MST_H_INCLUDED__ 
#define __MST_H_INCLUDED__
#include "MST.h"
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
#define REJECT 3
#define CONVERGE 4

MST::MST(int numNeighbors, int myID, CommInterface* comm){

	this->numNeighbors = numNeighbors;
	this->myID = myID;
	this->comm = comm;
	this->parentID = -1;
	this->numReplies = 0;
	this->phase1Done = 0;
	this->numCast = 0;

	printf("Process %d::MST::Constructed\n", (int) getpid() );
}

int MST::GetParent(){
	return this->parentID;
}

std::vector<int> MST::GetChildren(){
	return this->childrenID;
}

void MST::InitiateMST(){
	char *message = MST::CreateMessage(INVITE, this->myID);
	this->comm->BroadcastData(message);
}	

char* MST::CreateMessage(int messageType, int myID){
	char *message = (char *)malloc(MESSAGESIZE);
	memset(message, '\0', MESSAGESIZE);
	sprintf(message, "%d %d", messageType, myID);
	return message;
}

void MST::ParseMessage(char* message, int* messageType, int* ID){
	std::string sep = " ";
	char* current = (char*) malloc(MESSAGESIZE);
	current = strtok(message, sep.c_str());
	*messageType = *current - '0';
	current = strtok(NULL,sep.c_str());
	*ID = *current - '0';
}

int MST::ProcessMessage(int messageType, int ID){
	if(messageType == INVITE){
		if(this->parentID == -1 && this->myID != 0){
			this->parentID = ID;
			char *message = MST::CreateMessage(ACCEPT, this->myID);
			this->comm->SendData(message, ID);
			this->InitiateMST();
		}
		else{
			char *message = MST::CreateMessage(REJECT, this->myID);
			this->comm->SendData(message, ID);	
		}
		return 0;
	}
	else if(messageType == ACCEPT || messageType == REJECT){
		this->numReplies++;
		if(messageType == ACCEPT){
			this->childrenID.push_back(ID);
		}
		if(this->numReplies == this->numNeighbors){
			this->phase1Done = 1;
			if(this->childrenID.size() == 0){
				char *message = MST::CreateMessage(CONVERGE, this->myID);
				this->comm->SendData(message, this->parentID);	
				return 1;			
			}
		}
		return 0;
	}
	else if(messageType == CONVERGE){
		this->numCast++;
		if(this->phase1Done == 1){
			if(this->numCast == this->childrenID.size()){
				if(this->parentID != -1){
					char *message = MST::CreateMessage(CONVERGE, this->myID);
					this->comm->SendData(message, this->parentID);	
				}			
				return 1;
			}
		}
		return 0;
	}
	else{
		printf("Process %d::MST::Garbage message\n", (int) getpid());
	}

}

void MST::RunMST(){

	std::map<int, char*> pollMap;
	int pollNumber = 0;
	unsigned int syncTime = 5000000;
	int MSTDone = 0;

	if(this->myID == 0)
		MST::InitiateMST();

	printf("Process %d::MST::Initiated\n", (int) getpid() );

	while(MSTDone == 0){
		usleep(syncTime);
		// 1. Poll nodes
		pollMap = this->comm->PollData();
		for (auto& kv : pollMap) {
			if(kv.second != NULL){
		    	int messageType, ID;
		    	printf("Process %d::MST::Before ParseMessage\n", (int) getpid() );
		    	this->ParseMessage(kv.second, &messageType, &ID);
		    	printf("Process %d::LeaderElection::ID %d and messageType %d\n", (int) getpid(), ID, messageType );
		    	// 2. a) On receiving invite: Accept if parentID = -1, set parentID and send INVITE to all neighbors, else send reject
				//	  b) i) On receiving accept: Increment numReplies and append ID to childrenID
				//		 ii) On receiving reject: Increment numReplies
				//			 if numReplies == numNeighbors, return
		    	MSTDone = this->ProcessMessage(messageType, ID);
		    	if(MSTDone == 1)
		    		break;
			}
		} 
	}
	return;
}