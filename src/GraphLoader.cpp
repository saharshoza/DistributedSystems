#include <fstream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <vector>

#ifndef __GRAPHLOADER_H_INCLUDED__ 
#define __GRAPHLOADER_H_INCLUDED__
#include "../include/GraphLoader.h"
#endif

#define NUMBERSIZE 8
#define LINESIZE 64

std::vector<int> GraphLoader::GetNeighbors(int myVal, std::string filePath){
	std::ifstream infile(filePath);
	std::string line;
	std::vector<int> neighborsVector;
	while (std::getline(infile, line))
	{
		char* valLine = (char*) malloc(NUMBERSIZE);
		valLine = strtok((char*) line.c_str(), ":");
		int valInt = *valLine - '0';
		printf("valInt is %d\n", valInt);
		if(valInt == myVal){
			char* neighborStr = valLine;
			neighborStr = strtok(NULL, ",");
			while(neighborStr != NULL){
				//printf("Start while loop\n");
				//printf("neighborStr is %s\n", neighborStr);
				int neighborInt = *neighborStr - '0';
				neighborsVector.push_back(neighborInt);
				printf("neighbor is %d\n", neighborInt);
				neighborStr = strtok(NULL, ",");
			}
			printf("Done while loop\n");
			return neighborsVector;
		}
	}
	return neighborsVector;
}