#include <iostream>
#include "Coroutine.h"
#include "stdio.h"
#include <sys/socket.h>
#include "fcntl.h"
#include "unistd.h"
#include <arpa/inet.h>
#include <string.h>
#include "HookSyscall.hpp"
#include <vector>
#include <memory>

#define MAX_MSG_LEN 1000

void client(std::string ip, int port)
{
    int clientSocket;
    sockaddr_in serverAddr;
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 == clientSocket)
    {
        std::cout << "failed to create socket" << std::endl;
        return;
    }
    memset(&serverAddr, 0, sizeof(sockaddr_in));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    serverAddr.sin_port = htons(port);

    while (true)
    {
        while (Utility::coConnect(clientSocket, (sockaddr*)(&serverAddr), sizeof(serverAddr)) != 0)
        {
            close(clientSocket);
            std::cout << "failed to connect server , reconnect server" << std::endl;
            Utility::coSleep(3000);
            memset(&serverAddr, 0, sizeof(sockaddr_in));
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
            serverAddr.sin_port = htons(port);
            clientSocket = socket(PF_INET, SOCK_STREAM, 0);
            if (-1 == clientSocket)
            {
                std::cout << "failed to create socket" << std::endl;
                return;
            }
        }
        
        while (true)
        {
            Utility::coSleep(3000);
            std::string msg("client send msg to server, fd = ");
            msg.append(std::to_string(clientSocket));
            int writeLen = Utility::coWrite(clientSocket, msg.c_str(), msg.length());
            if (writeLen <= 0)
            {
                std::cout << "writeLen = " << writeLen << std::endl;
                close(clientSocket);
                break;
            }
            char recvMsg[MAX_MSG_LEN];
            int readLen = Utility::coRead(clientSocket, recvMsg, MAX_MSG_LEN);
            if (readLen <= 0)
            {
                std::cout << "readLen = " << readLen << std::endl;
                close(clientSocket);
                break;                
            }
        }        
    }
}

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        std::cout << "please input legal params" << std::endl;
        return 0;
    }
    int clientNum = atoi(argv[1]);
    std::string ip = argv[2];
    int port = atoi(argv[3]);
    auto scheduler = Utility::Scheduler::GetCurrentScheduler();
    std::vector<std::shared_ptr<Utility::Coroutine>> coroutines;
    for (int i = 0; i < clientNum; i++)
    {
        std::shared_ptr<Utility::Coroutine> clientCo = std::make_shared<Utility::Coroutine>(client, ip, port);
        coroutines.push_back(clientCo);
        clientCo->Start();
    }
    scheduler->Eventloop();
    return 0;
}