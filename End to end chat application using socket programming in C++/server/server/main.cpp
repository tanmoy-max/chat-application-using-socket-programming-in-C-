#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<tchar.h>
#include<thread>
#include<vector>
using namespace std;
#pragma comment (lib,"ws2_32.lib") 

/*
steps to perform the task
1. initialize winsock library

2. create the socket

3. get the ip and port

4. bind the ip and port in socket

5. listen on socket

6. accept

7. recieve and send msg

8. close the socket

9. clean up

*/
bool Initialize() {
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}
void InteractWithClient(SOCKET clientSocket, vector<SOCKET> &clients) {			// msg send and recieve
	char buffer[4096];
	while (1) {
		int byterecvd = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (byterecvd <= 0) {
			cout << " client disconnected" << endl;
			break;
		}
		string message(buffer, byterecvd);
		cout << "message recieved from client" << byterecvd << endl;

		for (auto client : clients) {
			if (client != clientSocket) {
				send(client, message.c_str(), message.length(), 0);
			}
		}
	}
	auto it = find(clients.begin(), clients.end(), clientSocket);
	if (it != clients.end()) {
		clients.erase(it);
	}
	
	closesocket(clientSocket);
}
int main() {
	if (!Initialize()) {
		cout << "winsock initialization failed" << endl;
		return 1;
	}
	cout << "hello" << endl;

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET) {
		cout << " socket creation failed"<<endl;
		return 1;
	}
	// create an address structure
	int port = 12345;
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);

	// convert the (0.0.0.0) ip adress to binary format and put it in sin_family
	if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1) {
		cout << "setting address structure failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//bind
	if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		cout << "bind failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	//Listen 
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		cout << "listen failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;

	}
	cout << "socket is listening at port:" << port << endl;

	vector <SOCKET> clients;
	while (1) {
		// accept
		SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
		if (clientSocket == INVALID_SOCKET) {
			cout << "client socket is invalid" << endl;
		}
		clients.push_back(clientSocket);
		thread t1(InteractWithClient, clientSocket, std::ref(clients));
		t1.detach();
	}
	
	
	closesocket(listenSocket);
	WSACleanup();
	return 0;
}
