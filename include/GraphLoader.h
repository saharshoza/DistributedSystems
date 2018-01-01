#include <fstream>
#include <string>
#include <stdio.h>
#include <vector>
#include <string.h>

class GraphLoader
{
private:
	std::string filePath;
	std::ifstream fileHandler;
	int myVal;
public:
	GraphLoader(){};
	std::vector<int> GetNeighbors(int myVal, std::string filePath);
	~GraphLoader(){};
};