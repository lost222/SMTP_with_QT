#include<QThread>
#include <QObject>
#include"my_base64.h"
#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <QObject>

// Need to link with Ws2_32.lib

#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#ifndef LISTENTHREAD_H
#define LISTENTHREAD_H


class ListenThread : public QThread
{
    Q_OBJECT
public:
    explicit ListenThread(QObject *parent = 0);
    SMTPServer s;
    void stop();
    void run();

private:
    volatile bool stopped;
    volatile int listen_dev;

signals:
void stringChanged(const QString &);


public slots:
};

#endif // LISTENTHREAD_H
