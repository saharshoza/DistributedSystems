#ifndef __LEADERELECTION_H_INCLUDED__ 
#define __LEADERELECTION_H_INCLUDED__
#include "LeaderElection.h"
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

    LeaderElection leader(numNodes, myVal, &comm);

    int leaderID = leader.RunElection();

    printf("Process %d::LeaderElectionTester Complete::Leader is %d\n", (int) getpid(), leaderID );
    
    //while(1){}
	return 0;
}