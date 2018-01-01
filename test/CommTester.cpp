#ifndef __COMMINTERFACE_H_INCLUDED__ 
#define __COMMINTERFACE_H_INCLUDED__
#include "../../include/CommInterface.h"
#endif

#include <iostream>
#include <sstream>
#include <thread>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{

	int numNodes, myVal;
	char buffer1[5];
	char buffer2[5];
	unsigned int syncTime = 10000000;
	std::map<int, char*> retPoll;

	std::istringstream ss(argv[1]);
    if (!(ss >> numNodes)){
        std::cerr << "Invalid number " << argv[1] << '\n';
    }

    std::istringstream ss1(argv[2]);

    if (!(ss1 >> myVal)){
        std::cerr << "Invalid number " << argv[2] << '\n';
    }
	
	CommInterface comm(numNodes, myVal);

	// Broadcast 1
	sprintf(buffer1, "%d", myVal);
	comm.BroadcastData(buffer1);

	usleep(syncTime);
	
	// Read Broadcast result
	retPoll = comm.PollData();
	for (auto& kv : retPoll) {
	    std::cout << kv.first << " has value " << kv.second << std::endl;
	}

	usleep(syncTime);
	
	// Read Broadcast result
	retPoll = comm.PollData();
	for (auto& kv : retPoll) {
	    std::cout << "Node " << kv.first << " sent value " << kv.second << std::endl;
	}


	usleep(syncTime*3);

	// Read Broadcast result
	retPoll = comm.PollData();
	for (auto& kv : retPoll) {
	    std::cout << "Node " << kv.first << " sent value " << kv.second << std::endl;
	}

	while(1){}
	return 0;
}