#include "listenthread.h"
#include<QDebug>


#define DEFAULT_BUFLEN 500
#define DEFAULT_PORT "25"

ListenThread::ListenThread(QObject *parent) : QThread(parent)
{
    stopped = false;
}

void ListenThread::stop()
{
    stopped = true;
}

void ListenThread::run()
{
    while( !stopped ) {
        WSADATA wsaData;
        int iResult;

        SOCKET ListenSocket = INVALID_SOCKET;
        SOCKET ClientSocket = INVALID_SOCKET;

        struct addrinfo *result = NULL;
        struct addrinfo hints;

        int iSendResult;
        char recvbuf[DEFAULT_BUFLEN + 1];
        int recvbuflen = DEFAULT_BUFLEN;

        // Initialize Winsock
        iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (iResult != 0) {
            //printf("WSAStartup failed with error: %d\n", iResult);
            //return 1;
            qDebug()<<"WSAStartup failed with error: "<<iResult<<"\n";
        }

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        // Resolve the server address and port
        iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
        if ( iResult != 0 ) {
            //printf("getaddrinfo failed with error: %d\n", iResult);
            WSACleanup();
            //return 1;
            qDebug()<<"getaddrinfo failed with error: "<<iResult<<"\n";
        }

        // Create a SOCKET for connecting to server
        ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (ListenSocket == INVALID_SOCKET) {
            //printf("socket failed with error: %ld\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            //return 1;
            qDebug()<<"socket failed with error:"<<WSAGetLastError()<<"\n";
        }

        // Setup the TCP listening socket
        iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
           // printf("bind failed with error: %d\n", WSAGetLastError());
            freeaddrinfo(result);
            closesocket(ListenSocket);
            WSACleanup();
          //  return 1;
            qDebug()<<"bind failed with error: "<<WSAGetLastError()<<"\n";
        }

        freeaddrinfo(result);

        iResult = listen(ListenSocket, SOMAXCONN);
        if (iResult == SOCKET_ERROR) {
           // printf("listen failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
           // return 1;
            qDebug()<<"listen failed with error: "<<WSAGetLastError()<<"\n";
        }

        // Accept a client socket
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
           // printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            //return 1;
            qDebug()<<"accept failed with error: "<< WSAGetLastError()<<"\n";
        }

        // No longer need server socket
        closesocket(ListenSocket);

        // Receive until the peer shuts down the connection

            iSendResult = send( ClientSocket,reply_code[4], strlen(reply_code[4]), 0 );
            if (iSendResult == SOCKET_ERROR) {
                   // printf("send failed with error: %d\n", WSAGetLastError());
                    closesocket(ClientSocket);
                    WSACleanup();
                   // return 1;
                    qDebug()<<"send failed with error: "<<WSAGetLastError()<<"\n";
                }

//            std::cout<<"S:"<<reply_code[4];
            QString str = "S: ";
            str.sprintf(reply_code[4]);
            emit stringChanged(str);
//            SMTPServer s;

            recvbuf[0] = '\0';

            while (strncmp(recvbuf, "QUIT", 4) != 0){
                /*getline(std::cin,a);*/
                iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
                if (iResult <= 0) { continue;}
                recvbuf[iResult] = '\0';
                const char* re = s.respond(recvbuf);
//                const char* ok = reply_code[6];
                if (strncmp(re, "Nsend", 5) == 0){continue;}
                iSendResult = send( ClientSocket,re, strlen(re), 0 );
//                std::cout<<"C:"<<recvbuf<<std::endl;
//                std::cout<<"S:"<<re;
                QString str = "";
//                if (recvbuf[iResult - 1] == '.')
                str.sprintf("C: %s\nS: %s",recvbuf,re);
                emit stringChanged(str);
            }


        // shutdown the connection since we're done
        iResult = shutdown(ClientSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            //printf("shutdown failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            //return 1;
            qDebug()<<"shutdown failed with error: "<<WSAGetLastError()<<"\n";
        }

        // cleanup
        closesocket(ClientSocket);
        WSACleanup();
  }
    stopped = true;
}
