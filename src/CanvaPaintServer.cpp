//
// Created by mate on 10.02.2021.
//

#include "CanvaPaintServer.h"

bool CanvaPaintServer::Init() {
    // Load saves etc.
    return true;
}

bool CanvaPaintServer::StartThreaded(unsigned int threadCount, unsigned short port) {
    cerr << "Starting Canva Paint Server on " << port << " port, using " << threadCount << " threads." << endl;

    // Spawn threads
    for (unsigned int i = 0; i < threadCount; i++) {
        this->threads.push_back(new thread([this, port] { this->ServerThread(port); }));
    }

    return true;
}

void CanvaPaintServer::ServerThread(unsigned short port) {

    this->printGuard.lock();
    cerr << "Spawned thread with id " << this_thread::get_id() << endl;
    this->printGuard.unlock();

    // Do work
    // Very simple WebSocket echo server

    uWS::App().ws<PerSocketData>("/*", {
        // Settings
        .compression = uWS::DEDICATED_COMPRESSOR_3KB,
        .maxPayloadLength = 16 * 1024,
        .idleTimeout = 16,
        .maxBackpressure = 1 * 1024 * 1024,
        .resetIdleTimeoutOnSend = true,
        .sendPingsAutomatically = true,
        // Handlers
        .upgrade = nullptr,
        .open = [this](auto *ws) {
            // Push new connection, we use this to gracefully close server
            this->socketsVector.lock();
            this->connectedSockets.push_back(ws);
            this->socketsVector.unlock();

            // Every client must listen to data broadcast
            ws->subscribe("data");

            this->printGuard.lock();
            cerr << "[" << this_thread::get_id() << "] new connection from " << ws->getRemoteAddressAsText() << endl;
            this->printGuard.unlock();
        },
        .message = [this](auto *ws, string_view message, uWS::OpCode opCode) {

            // Do not process
            if (this->shutdownRequested) {
                ws->close();
                return;
            }

            this->printGuard.lock();
            cerr << "[" << this_thread::get_id() << "] message from " << ws->getRemoteAddressAsText() << ":" << endl << message << endl;
            this->printGuard.unlock();


            // Broadcast to others
            this->socketsVector.lock();
            for (auto socket : this->connectedSockets) {
                if (socket == ws) continue;
                socket->send(message, opCode, true);
            }
            this->socketsVector.unlock();
        },
        .drain = [](auto *ws) { },
        .ping = [](auto *ws) {
            cerr << "Ping" << endl;
            },
        .pong = [](auto *ws) {
            cerr << "Pong" << endl;
            },
        .close = [this](auto *ws, int code, string_view message) {
            this->socketsVector.lock();

            this->connectedSockets.erase(
            std::remove_if(this->connectedSockets.begin(), this->connectedSockets.end(),
           [ws](auto *soc){ return soc == ws; }),
            this->connectedSockets.end());

            this->socketsVector.unlock();


            this->printGuard.lock();
            cerr << "[" << this_thread::get_id() << "] connection closed from " << ws->getRemoteAddressAsText() << endl;
            this->printGuard.unlock();
        }
    }).listen(port, [this, port](us_listen_socket_t *listen_socket) {

        // Save socket in order to control it
        this->threadsSockets[this_thread::get_id()] = listen_socket;

        this->printGuard.lock();
        cerr << "[" << this_thread::get_id() << "] ";
        cerr << (listen_socket ? "listening" : "failed to listen");
        cerr << " on port " << port << endl;
        this->printGuard.unlock();
    }).run();

    this->printGuard.lock();
    cerr << "[" << this_thread::get_id() << "] Received shutdown request" << endl;
    this->printGuard.unlock();
}

void CanvaPaintServer::Shutdown() {
    this->shutdownRequested = true;

    // Stop listening to new connections
    for (auto pair : this->threadsSockets) {
        us_listen_socket_close(0, pair.second);
    }

    // Close existing connections
    for (auto socket : this->connectedSockets) {
        socket->close();
    }

    // Wait for others
    for (auto &t : this->threads) {
        t->join();
    }
}

void CanvaPaintServer::Restart() {
    this->printGuard.lock();
    cerr << "Shutting down and restarting" << endl;
    this->printGuard.unlock();
}
