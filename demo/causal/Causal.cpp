#ifndef __CAUSAL_H_INCLUDED__ 
#define __CAUSAL_H_INCLUDED__
#include "Causal.h"
#endif

#include "message.pb.h"

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
#include <iostream>
#include <fstream>

Causal::Causal(int numNeighbors, int myID, CommInterface* comm){

	this->comm = comm;
	this->numNeighbors = numNeighbors;
	this->myID = myID;

	std::vector<std::vector<int>> matrix(numNeighbors);
	// for ( int i = 0 ; i < numNeighbors ; i++ )
	//    matrix[i].resize(numNeighbors);

	for ( int i = 0 ; i < numNeighbors ; i++ ){
	   matrix[i].resize(numNeighbors);
	   for (int j=0; j < numNeighbors; j++){
	   	matrix[i][j] = 5;
	   }
	}
	this->timeMatrix = matrix;

	printf("Causal contructed\n");
}

void print_bytes_causal(const void *object, size_t size)
{
  // This is for C++; in C just drop the static_cast<>() and assign.
  const unsigned char * const bytes = static_cast<const unsigned char *>(object);
  size_t i;

  printf("[ ");
  for(i = 0; i < size; i++)
  {
    printf("%02x ", bytes[i]);
  }
  printf("]\n");
}

// Parses and prints serialized protobuffer
void ReadProtoBuf(char* buffer, size_t size){
	printf("Reading buffer\n");
	message::CausalMessage* causalMessage = new message::CausalMessage();
	//std::fstream input(buffer);
	print_bytes_causal(buffer, size);
	causalMessage->ParseFromArray(buffer, size);
	print_bytes_causal(causalMessage, size);
	std::cout << "Message is " << causalMessage->messagebdy() << "\n";
	std::cout << "srcID is " << causalMessage->srcid() << "\n";
	std::cout << "DestID is " << causalMessage->destid() << "\n";
	message::CausalMessage_Matrix causalMatrix = causalMessage->matrix();
	std::cout << "Size is " << causalMatrix.vectors_size() << "\n";
	for(int i = 0; i< causalMatrix.vectors_size(); i++){
		//std::cout << "Vector " << i << "\n";
		for(int j = 0; j < causalMatrix.vectors(i).event_size(); j++){
			std::cout << causalMatrix.vectors(i).event(j) << "\t";
		}
		printf("\n");
	}
}

// Message_t* Causal::ParseMessage(char* buffer, size_t size){
// 	printf("Reading buffer\n");
// 	message::CausalMessage* causalMessage = new message::CausalMessage();
// 	//std::fstream input(buffer);
// 	print_bytes_causal(buffer, size);
// 	causalMessage->ParseFromArray(buffer, size);
// 	print_bytes_causal(causalMessage, size);
// 	std::cout << "Message is " << causalMessage->messagebdy() << "\n";
// 	std::cout << "srcID is " << causalMessage->srcid() << "\n";
// 	std::cout << "DestID is " << causalMessage->destid() << "\n";
// 	message::CausalMessage_Matrix causalMatrix = causalMessage->matrix();
// 	std::cout << "Size is " << causalMatrix.vectors_size() << "\n";
// 	for(int i = 0; i< causalMatrix.vectors_size(); i++){
// 		//std::cout << "Vector " << i << "\n";
// 		for(int j = 0; j < causalMatrix.vectors(i).event_size(); j++){
// 			std::cout << causalMatrix.vectors(i).event(j) << "\t";
// 		}
// 		printf("\n");
// 	}
// }

char* Causal::CreateMessage(char* message, int destID, size_t* sizePtr){

	message::CausalMessage* causalMessage = new message::CausalMessage();
	printf("Reached\n");
	causalMessage->set_messagebdy(message, sizeof(message));
	printf("Message set\n");
	message::CausalMessage_Matrix* causalMatrix = new message::CausalMessage_Matrix();
	print_bytes_causal(&this->timeMatrix, this->timeMatrix.size()*this->timeMatrix[0].size());

	for(int i = 0; i < this->timeMatrix.size(); i++){
		std::cout << "Vector " << i << "\n";
		// message::CausalMessage_Matrix_Vector* causalMessageVector = new message::CausalMessage_Matrix_Vector();
		message::CausalMessage_Matrix_Vector* causalMessageVector = causalMatrix->add_vectors();
		for(int j = 0; j < this->timeMatrix[i].size(); j++){
			causalMessageVector->add_event(this->timeMatrix[i][j]);
		}

		for(auto& e: causalMessageVector->event()){
			std::cout << e << "\t";
		}
		printf("\n");

		for(auto& v: causalMatrix->vectors()){
			for(auto& e: v.event()){
				std::cout << e << "\t";
			}
		}
		printf("\n");
	}
	causalMessage->set_allocated_matrix(causalMatrix);
	print_bytes_causal(causalMatrix, sizeof causalMatrix);
	printf("Matrix set\n");
	causalMessage->set_srcid(this->myID);
	causalMessage->set_destid(destID);

	printf("Allocated\n");
	size_t size = causalMessage->ByteSizeLong(); 
	char *buffer = (char*)malloc(size);
	causalMessage->SerializeToArray(buffer, size);
	printf("Serialized\n");
	print_bytes_causal(causalMessage, size);
	print_bytes_causal(buffer, size);
	*sizePtr = size;
	printf("Inside CreateMessage\n");
	ReadProtoBuf(buffer, size);
	return buffer;
}

void Causal::SendCausal(char* message, int destID){
	size_t size;
	char* causalMessage = this->CreateMessage(message, destID, &size);
	print_bytes_causal(causalMessage, sizeof causalMessage);
	printf("Completed CreateMessage with %s and strlen %lu and sizeof %lu\n", causalMessage, strlen(causalMessage), sizeof(causalMessage));
	this->comm->SendData(causalMessage, destID);
	printf("Completed send\n");
	for(int i = 0; i < this->timeMatrix.size(); i++){
		for(int j = 0; j < this->timeMatrix[i].size(); j++)
			printf("%d\t", this->timeMatrix[i][j]);
		printf("\n");
	}
	this->timeMatrix[this->myID][destID]++;
	for(int i = 0; i < this->timeMatrix.size(); i++){
		for(int j = 0; j < this->timeMatrix[i].size(); j++)
			printf("%d\t", this->timeMatrix[i][j]);
		printf("\n");
	}
	printf("incremented\n");
}

void TestCast(char* causalMessage){
	printf("Begin recast\n");
	Message_t *newMessage = reinterpret_cast<Message_t*>(causalMessage);

	printf("Message Body is %s\n", newMessage->messageBody);
	std::vector<std::vector<int>>* messageMatrix = reinterpret_cast<std::vector<std::vector<int>>*>(newMessage->timeMatrix);

	printf("Vector recast complete and size is %lu\n", (*messageMatrix).size());
	for(int i = 0; i < (*messageMatrix).size(); i++){
		for(int j = 0; j < (*messageMatrix)[i].size(); j++)
			printf("%d\t", (*messageMatrix)[i][j]);
		printf("\n");
	}
	printf("Source ID is %d\n", newMessage->srcID);
	printf("Dest ID is %d\n", newMessage->recvID);

	printf("End recast\n");

}

void Causal::SendCausal(char* message, int destID, long unsigned int delay){
	size_t size;
	char* causalMessage = this->CreateMessage(message, destID, &size);
	printf("Completed CreateMessage with %s and strlen %lu and sizeof %lu\n", causalMessage, strlen(causalMessage), sizeof(causalMessage));
	print_bytes_causal(causalMessage, sizeof causalMessage);
	//TestCast(causalMessage);
	this->comm->SendData(causalMessage, size, destID, delay);
	printf("Completed send\n");
	for(int i = 0; i < this->timeMatrix.size(); i++){
		for(int j = 0; j < this->timeMatrix[i].size(); j++)
			printf("%d\t", this->timeMatrix[i][j]);
		printf("\n");
	}
	this->timeMatrix[this->myID][destID]++;
	for(int i = 0; i < this->timeMatrix.size(); i++){
		for(int j = 0; j < this->timeMatrix[i].size(); j++)
			printf("%d\t", this->timeMatrix[i][j]);
		printf("\n");
	}
	printf("incremented\n");
}

bool Causal::isDelivered(Message_t* newMessage){

	std::vector<std::vector<int>>* messageMatrix = reinterpret_cast<std::vector<std::vector<int>>*>(newMessage->timeMatrix);
	bool isDelivered = true;

	for(int k = 0; k < messageMatrix->size(); k++){
		if((*messageMatrix)[k][this->myID] > this->timeMatrix[k][this->myID]){
			isDelivered = false;
			break;
		}
	}

	return isDelivered;
}

void Causal::PerformDelivery(Message_t* newMessage){
	
	std::vector<std::vector<int>>* messageMatrix = reinterpret_cast<std::vector<std::vector<int>>*>(newMessage->timeMatrix);

	for(int sender=0; sender < this->timeMatrix.size(); sender++){
		for(int receiver = 0; receiver < this->timeMatrix.size(); receiver++){
			int maxTime = std::max(this->timeMatrix[sender][receiver], (*messageMatrix)[sender][receiver]);
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