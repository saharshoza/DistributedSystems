#ifndef __MST_H_INCLUDED__ 
#define __MST_H_INCLUDED__
#include "MST.h"
#endif

#ifndef __COMMINTERFACE_H_INCLUDED__ 
#define __COMMINTERFACE_H_INCLUDED__
#include "../../include/CommInterface.h"
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

    GraphLoader g;
    std::vector<int> neighbors = g.GetNeighbors(myVal, "input.txt");
    printf("neighbors of node %d are\n", myVal);
    for(auto &v: neighbors){
        printf("%d\t", v);
    }
    printf("\n");
    
    //int numNodes = 0;

    // std::vector<int> neighbors;
    
    // if(myVal == 0){
    //     neighbors.push_back(1);
    //     neighbors.push_back(2);
    //     //numNodes = 2;
    // }
    // else if(myVal == 1){
    //     neighbors.push_back(0);
    //     neighbors.push_back(2);
    //     neighbors.push_back(3);
    //     //numNodes = 3;
    // }
    // else if(myVal == 2){
    //     neighbors.push_back(0);
    //     neighbors.push_back(1);
    //     neighbors.push_back(3);
    //     //numNodes = 3;    
    // }
    // else if(myVal == 3){
    //     neighbors.push_back(1);
    //     neighbors.push_back(2);
    //     //numNodes = 2;
    // }

    CommInterface comm(numNodes, myVal, neighbors);

    MST mst(neighbors.size(), myVal, &comm);

    mst.RunMST();

    printf("Process %d::MSTTester Complete::Parent is %d\n", (int) getpid(), mst.GetParent() );

    for (auto& v : mst.GetChildren()){
        printf("%d\n", v);
    }
    //while(1){}
	return 0;
}