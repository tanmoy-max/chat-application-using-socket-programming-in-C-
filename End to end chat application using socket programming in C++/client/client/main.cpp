#include<iostream>
#include<WinSock2.h>
#include<ws2tcpip.h>
#include<thread>
using namespace std;
#pragma comment(lib,"ws2_32.lib")

/*
	1. initialize winsock

	2. create socket

	3. connect to the server

	4. send/recv

	5. clode the server

*/
bool Initialize() {
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}
void sendMessage(SOCKET s) {
	cout << "enter your chat name : " << endl;
	string name;
	getline(cin, name);
	string message;

	while (1) {
		getline(cin, message);
		string msg = name + " : " + message;
		int bytesent = send(s, msg.c_str(), msg.length(), 0);
		if (bytesent == SOCKET_ERROR) {
			cout << "send msg failed" << endl;
			break;
		}
		if (message == "quit") {
			cout << "stop the application" << endl;
			break;
		}
		closesocket(s);
		WSACleanup();
	}
}
void recieveMessage(SOCKET s) {
	char buffer[4096];
	int recieveLength;
	string msg="";
	while (1) {
		recieveLength = recv(s, buffer, sizeof(buffer), 0);
		if (recieveLength <= 0) {
			cout << "disconnected from server" << endl;
			break;
		}
		else {
			msg = string(buffer, recieveLength);
			cout << msg << endl;
		}
	}
	closesocket(s);
	WSACleanup();
}
int main() {
	cout << "hello" << endl;
	if (!Initialize()) {
		cout << "initialize winsock failed" << endl;
		return 1;
	}

	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET) {
		cout << "invalid socket created" << endl;
		return 1;
	}
	int port = 12345;
	string serveraddress = "127.0.1.0";
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	inet_pton(AF_INET, serveraddress.c_str(), &(serveraddr.sin_addr));

	if (connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		cout << "it did not get connected" << endl;
		closesocket(s);
		WSACleanup();
		return 1;
	}
	cout << "successfully connect to server" << endl;

	thread senderThread(sendMessage, s);
	thread recieverThread(recieveMessage, s);
	senderThread.join();
	recieverThread.join();

	
	return 0;
}