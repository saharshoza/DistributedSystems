#ifndef __CAUSAL_H_INCLUDED__ 
#define __CAUSAL_H_INCLUDED__
#include "Causal.h"
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
#include <vector>

Causal::Causal(int numNeighbors, int myID, CommInterface* comm){

	this->comm = comm;
	this->numNeighbors = numNeighbors;
	this->myID = myID;

	std::vector<std::vector<int> > matrix(numNeighbors);
	for ( int i = 0 ; i < numNeighbors ; i++ )
	   matrix[i].resize(numNeighbors);
	this->timeMatrix = matrix;

}

char* Causal::CreateMessage(char* message, int destID){
	Message_t* m;
	m->messageBody = message;
	m->timeMatrix = this->timeMatrix;
	m->recvID = destID;
	m->srcID = this->myID;
	return reinterpret_cast<char*>(m);
// https://ideone.com/7H8dy
}

void Causal::SendCausal(char* message, int destID){
	char* causalMessage = this->CreateMessage(message, destID);
	this->comm->SendData(causalMessage, destID);
	timeMatrix[this->myID][destID]++;
}

bool Causal::isDelivered(Message_t* newMessage){

	std::vector<std::vector<int>> messageMatrix = newMessage->timeMatrix;
	bool isDelivered = true;

	for(int k = 0; k < messageMatrix.size(); k++){
		if(messageMatrix[k][this->myID] > this->timeMatrix[k][this->myID]){
			isDelivered = false;
			break;
		}
	}

	return isDelivered;
}

void Causal::PerformDelivery(Message_t* newMessage){
	
	std::vector<std::vector<int>> messageMatrix = newMessage->timeMatrix;

	for(int sender=0; sender < this->timeMatrix.size(); sender++){
		for(int receiver = 0; receiver < this->timeMatrix.size(); receiver++){
			int maxTime = std::max(this->timeMatrix[sender][receiver], messageMatrix[sender][receiver]);
			this->timeMatrix[sender][receiver] = maxTime;
		}
	}
	this->timeMatrix[newMessage->srcID][this->myID]++;
	this->deliveredMessages.push_back(newMessage->messageBody);
}

void Causal::Debuffer(){
	while(1){
		Message_t candidate = this->bufferQueue.top();
		if(this->isDelivered(&candidate)){
			this->PerformDelivery(&candidate);
			this->bufferQueue.pop();
		}
		else{
			break;
		}
	}
}

std::vector<char*> Causal::GetReceived(){
	std::map<int, char*> pollMap;
	int pollNumber = 0;
	unsigned int syncTime = 5000000;
	usleep(syncTime);
	// 1. Poll nodes
	pollMap = this->comm->PollData();
	for(auto& kv: pollMap){
		if(kv.second != NULL){
			// 2. Add to received vector
			Message_t *newMessage = reinterpret_cast<Message_t*>(kv.second);
			this->receivedMessages.push_back(newMessage->messageBody);
			// 3. a) Check if delivered (all timestamps received are <= my timestamps)
			// 	  b) If delivered, update my timeMatrix with max of both,  increment the matrix[sender, receiver]++ and add to delivered vector
			//	  c) Check if buffered data can be delivered due to this delivery in a loop

			if(this->isDelivered(newMessage)){
				this->PerformDelivery(newMessage);
				this->Debuffer();
			}

			// 4. If not delivered, add to buffer
			else{
				this->bufferQueue.push(*newMessage);
			}
		}
	}
	return this->receivedMessages;
};

std::vector<char*> Causal::GetDelivered(){
	return this->deliveredMessages;
};