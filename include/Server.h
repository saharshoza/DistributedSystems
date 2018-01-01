#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <queue> 
#include <map> 

class Server
{

private:
	//int serverFd;
	struct sockaddr_in address;
    //int myVal, numNodes;

public:
	int serverFd;
	int myVal, numNodes;

    std::map<int, std::queue<char*>> sendChannel;
    std::map<int, int> socketMap;

	Server(int numNodes, int myVal);
	void CreateSocket();
	int SetConnections();
	void SendData(int nodeNum);
	void ListSockets();
	~Server(){};
	
};