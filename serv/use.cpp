#include<iostream>
#include<string>
#include<cstring>
#include<fstream>
#include <winsock2.h>
#include "ThreadPool.h"
#pragma comment(lib, "ws2_32.lib")
using namespace std;

void say_haha() {
	SOCKET serv = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = PF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(8080);
	bind(serv, (sockaddr*)&addr, sizeof(sockaddr));
	listen(serv, 20);
	while (1) {
		sockaddr caddr;
		int size = sizeof(sockaddr);
		SOCKET client = accept(serv, (sockaddr*)&caddr, &size);
		cout << "connection established" << endl;
		for (int i = 1; i <= 20; i++) {
			send(client, "xzz hahaha  ", 13, 0);
			if (i % 5 == 0) send(client, "\n", 1, 0);
		}
		shutdown(client, SD_SEND);
		char buf[15] = { 0 };
		recv(client, buf, 15, 0);
		cout << buf << endl;
		closesocket(client);
	}
	closesocket(serv);
}
void webs() {
	SOCKET serv = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = PF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(8080);
	string content, tmp;
	ifstream ifs("hello.html");
    if (!ifs) throw runtime_error("error!!!");
	while (getline(ifs, tmp)) content.append(tmp);
	string response = "HTTP/1.1 200 OK\r\n\r\n" + content;
	::bind(serv, (sockaddr*)&addr, sizeof(sockaddr));
	listen(serv, 20);
	ThreadPool pool(5);
    int stop = 0;
	while (1 && !stop) {
		sockaddr caddr;
		int size = sizeof(sockaddr);
		SOCKET client = accept(serv, (sockaddr*)&caddr, &size);
		function<void()> tmp = [&stop, &response, client]() {
			printf("connection established\n");
			char buf[30] = { '\0' };
			recv(client, buf, 20, 0);
			string req(buf);
            if (!req.find("GET /stop HTTP/1.1")) {
                send(
                    client, 
                    "HTTP/1.1 200 OK\r\n\r\n <body><h1>OK!!!!!</h1><p>has stopped</p></body>"
                    , 67
                    , 0
                );
                stop = 1;
            }
			if (!req.find("GET /hello HTTP/1.1") || !req.find("GET / HTTP/1.1"))
				send(client, response.c_str(), response.size(), 0);
			shutdown(client, SD_SEND);
			closesocket(client);
		};
		pool.execute(tmp);
	}
	pool.stop();
	closesocket(serv);
}
int main() {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	webs();
	WSACleanup();
}