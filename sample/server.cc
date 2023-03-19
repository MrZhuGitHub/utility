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

void acceptClient(int clientSocket)
{
    std::cout << "6" << std::endl;
    char msg[MAX_MSG_LEN];
    memset(msg, 0, MAX_MSG_LEN);
    while (true)
    {
        int readLen = Utility::coRead(clientSocket, msg, MAX_MSG_LEN);
        if (readLen <= 0)
        {
            std::cout << "client close, fd = " << clientSocket << std::endl;
            close(clientSocket);
            return;
        }
        std::cout << "recv from client = " << clientSocket << " : " << msg << std::endl;
        int begin = 0;
        while (readLen > 0)
        {
            int writeLen = Utility::coWrite(clientSocket, (msg + begin), readLen);
            if (writeLen <= 0)
            {
                std::cout << "client close, fd = " << clientSocket << std::endl;
                close(clientSocket);
                return;                
            }
            readLen = readLen - writeLen;
            begin = begin + writeLen;
        }
        memset(msg, 0, MAX_MSG_LEN);
    }
}

void listenPort(uint32_t port)
{
    sockaddr_in serverAddr;
    int serverSocket;
    serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 == serverSocket)
    {
        std::cout << "failed to create socket, ret = " << serverSocket << std::endl;
        return;
    }

    memset(&serverAddr, 0, sizeof(sockaddr_in)) ;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);
    if (bind(serverSocket, (sockaddr*)(&serverAddr), sizeof(serverAddr)) == -1)
    {
        std::cout << "failed to bind socket" << std::endl;
        close(serverSocket);
        return;
    }

    if (listen(serverSocket, 5) == -1)
    {
        std::cout << "failed to bind socket" << std::endl;
        close(serverSocket);
        return;
    }

    std::vector<std::shared_ptr<Utility::Coroutine>> coroutines;

    while (1)
    {
        socklen_t clientAddrSize;
        sockaddr_in clientAddr;
        int clientSocket;
        clientAddrSize = sizeof(clientAddr);
        clientSocket = Utility::coAccept(serverSocket, (sockaddr*)(&clientAddr), &clientAddrSize);
        if (-1 == clientSocket)
        {
            std::cout << "accpet fd failed" << std::endl;
        } else {
            std::cout << "established new tcp connection, fd = " << clientSocket << std::endl;
            std::shared_ptr<Utility::Coroutine> ptrAcceptClient = std::make_shared<Utility::Coroutine>(acceptClient, clientSocket);
            coroutines.push_back(ptrAcceptClient);
            ptrAcceptClient->Start();
        }
    }
}

int main()
{
    auto scheduler = Utility::Scheduler::GetCurrentScheduler();
    auto server = std::make_shared<Utility::Coroutine>(listenPort, 8000);
    server->Start();
    scheduler->Eventloop();
    return 0;
}