//
// Created by mate on 10.02.2021.
//

#include "CanvaPaintServer.h"

bool CanvaPaintServer::Init() {
    // Load saves etc.
    return true;
}

bool CanvaPaintServer::StartThreaded(unsigned int threadCount, unsigned short port) {
    cout << "Starting Canva Paint Server on " << port << " port, using " << threadCount << " threads." << endl;

    // Spawn threads
    for (unsigned int i = 0; i < threadCount; i++) {
        this->threads.push_back(new thread([this, port] { this->ServerThread(port); }));
    }

    return true;
}

void CanvaPaintServer::ServerThread(unsigned short port) {

    this->printGuard.lock();
    cout << "Spawned thread with id " << this_thread::get_id() << endl;
    this->printGuard.unlock();

    // Do work
    // Very simple WebSocket echo server

    uWS::App().ws<PerSocketData>("/*", {
        // Settings
        .compression = uWS::SHARED_COMPRESSOR,
        .maxPayloadLength = 16 * 1024,
        .idleTimeout = 16,
        .maxBackpressure = 1 * 1024 * 1024,
        .resetIdleTimeoutOnSend = true,
        .sendPingsAutomatically = true,
        // Handlers
        .upgrade = nullptr,
        .open = [this](auto *ws) {
            this->printGuard.lock();
            cout << "[" << this_thread::get_id() << "] new connection from " << ws->getRemoteAddressAsText() << endl;
            this->printGuard.unlock();
        },
        .message = [this](auto *ws, string_view message, uWS::OpCode opCode) {

            this->printGuard.lock();
            cout << "[" << this_thread::get_id() << "] message from " << ws->getRemoteAddressAsText() << ":" << endl << message << endl;
            this->printGuard.unlock();

            // Do not process
            if (this->shutdownRequested) {
                ws->close();
                return;
            }

            // Resend
            ws->send(message, opCode);
        },
        .drain = [](auto *ws) { },
        .ping = [](auto *ws) { },
        .pong = [](auto *ws) { },
        .close = [this](auto *ws, int code, string_view message) {
            this->printGuard.lock();
            cout << "[" << this_thread::get_id() << "] connection closed from " << ws->getRemoteAddressAsText() << endl;
            this->printGuard.unlock();
        }
    }).listen(port, [this, port](us_listen_socket_t *listen_socket) {

        // Save socket in order to control it
        this->threadsSockets[this_thread::get_id()] = listen_socket;

        this->printGuard.lock();
        cout << "[" << this_thread::get_id() << "] ";
        cout << (listen_socket ? "listening" : "failed to listen");
        cout << " on port " << port << endl;
        this->printGuard.unlock();
    }).run();

    this->printGuard.lock();
    cout << "[" << this_thread::get_id() << "] Received shutdown request" << endl;
    this->printGuard.unlock();
}

void CanvaPaintServer::Shutdown() {
    this->shutdownRequested = true;

    // Kill child sockets threads
    for (auto pair : this->threadsSockets) {
        us_listen_socket_close(0, pair.second);
    }

    // Wait for others
    for (auto &t : this->threads) {
        t->join();
    }
}

void CanvaPaintServer::Restart() {
    this->printGuard.lock();
    cout << "Shutting down and restarting" << endl;
    this->printGuard.unlock();
}
