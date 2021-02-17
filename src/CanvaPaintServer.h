//
// Created by mate on 10.02.2021.
//

#ifndef CANVAPAINTSERVER_CANVAPAINTSERVER_H
#define CANVAPAINTSERVER_CANVAPAINTSERVER_H

#include <uWebSockets/App.h>
#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <string>
#include <mutex>

using namespace std;

class CanvaPaintServer {
    // Controls live of other threads
    atomic_bool shutdownRequested = false;

    // ws->getUserData returns one of these
    struct PerSocketData {

    };

    // Contains all threads used by server
    vector<thread*> threads;
    map<thread::id, us_listen_socket_t*> threadsSockets;

    // Multithreading guards
    mutex printGuard;

    // Single thread
    void ServerThread(unsigned short port);

public:
    bool Init();

    bool StartThreaded(unsigned int threadCount, unsigned short port);

    void Shutdown();

    void Restart();
};


#endif //CANVAPAINTSERVER_CANVAPAINTSERVER_H
