#include "GraphLoader.h"

int main(int argc, char const *argv[])
{
	std::vector<int> out;
	GraphLoader g;
	out = g.GetNeighbors(0, "input.txt");
	for(auto &v: out){
		printf("%d\n", v);
	}
	printf("returned\n");
	out = g.GetNeighbors(1, "input.txt");
	printf("returned\n");
	out = g.GetNeighbors(2, "input.txt");
	printf("returned\n");
	out = g.GetNeighbors(3, "input.txt");
	printf("returned\n");
	return 0;
}