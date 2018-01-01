#ifndef __LEADERELECTION_H_INCLUDED__ 
#define __LEADERELECTION_H_INCLUDED__
#include "LeaderElection.h"
#endif

#ifndef __COMMINTERFACE_H_INCLUDED__ 
#define __COMMINTERFACE_H_INCLUDED__
#include "CommInterface.h"
#endif

// #include "LeaderElection.h"

#include <iostream>
#include <sstream>
#include <thread>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	int numNodes, myVal;

	std::istringstream ss(argv[1]);
    if (!(ss >> numNodes)){
        std::cerr << "Invalid number " << argv[1] << '\n';
    }

    std::istringstream ss1(argv[2]);

    if (!(ss1 >> myVal)){
        std::cerr << "Invalid number " << argv[2] << '\n';
    }

    CommInterface comm(numNodes, myVal);

    LeaderElection leader(numNodes, myVal, &comm);

    int leaderID = leader.RunElection();

    printf("Process %d::LeaderElectionTester Complete::Leader is %d\n", (int) getpid(), leaderID );
    
    //while(1){}
	return 0;
}