#ifndef __CAUSAL_H_INCLUDED__ 
#define __CAUSAL_H_INCLUDED__
#include "Causal.h"
#endif

#ifndef __COMMINTERFACE_H_INCLUDED__ 
#define __COMMINTERFACE_H_INCLUDED__
#include "CommInterface.h"
#endif

#ifndef __GRAPHLOADER_H_INCLUDED__ 
#define __GRAPHLOADER_H_INCLUDED__
#include "../../include/GraphLoader.h"
#endif

#include <iostream>
#include <sstream>
#include <thread>
#include <stdio.h>
#include <unistd.h>

#define PATHSIZE 128

#define SENDER 1
#define RECEIVER 5

int main(int argc, char const *argv[])
{
	int numNodes, myVal;

    setbuf(stdout, NULL);

	std::istringstream ss(argv[1]);
    if (!(ss >> numNodes)){
        std::cerr << "Invalid number " << argv[1] << '\n';
    }

    std::istringstream ss1(argv[2]);

    if (!(ss1 >> myVal)){
        std::cerr << "Invalid number " << argv[2] << '\n';
    }

    GraphLoader g;
    
    const char *dirPath = getenv("DistributedSystems");
    printf("dirPath is %s\n", dirPath);
    char inputPath[PATHSIZE];
    memset(inputPath, '\0', sizeof(inputPath));
    strcat(inputPath, dirPath);
    strcat(inputPath, "demo/leader/input.txt");

    std::vector<int> neighbors = g.GetNeighbors(myVal, inputPath);
    printf("neighbors of node %d are\n", myVal);
    for(auto &v: neighbors){
        printf("%d\t", v);
    }
    printf("\n");

    CommInterface comm(numNodes, myVal, neighbors);

    comm.CreateLogger("leader");

    printf("Logging path is %s\n", comm.logName);

    Causal causal(numNodes, myVal, &comm);

    // Send 3 messages. One to each server
    if(myVal == SENDER){
        causal.SendCausal("M1", 1);
        causal.SendCausal("M2", 2);
        causal.SendCausal("M3", 3);
    }

    // First message is delayed and next to reach instantly
    if(myVal != SENDER && myVal != RECEIVER){
        std::vector<char*> messageBodies = causal.GetDelivered();
        if(myVal == 2)
            usleep(500000);
        for(auto &v: messageBodies){
            causal.SendCausal(v, RECEIVER);
        }
    }

    // Wait for messages to be delivered at the receiver
    if(myVal == RECEIVER){
        while(1){
            std::vector<char*> messageBodies = causal.GetDelivered();
            if(messageBodies.size() == 3){
                printf("Process %d::CausalTester Complete::Messages are\n", (int) getpid());
                for(auto &v: messageBodies){
                    printf("%s\t", v);
                }
                printf("\n");
                break;
            }
        }
    }
    
    //while(1){}
	return 0;
}