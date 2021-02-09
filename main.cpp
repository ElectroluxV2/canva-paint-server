#include "App.h"
#include <thread>
#include <algorithm>

using namespace std;

int main() {
    /* ws->getUserData returns one of these */
    struct PerSocketData {

    };

    /* Simple echo websocket server, using multiple threads */
    vector<thread *> threads(thread::hardware_concurrency());

    transform(threads.begin(), threads.end(), threads.begin(), [](thread */*t*/) {
        return new thread([]() {

            /* Very simple WebSocket echo server */
            uWS::App().ws<PerSocketData>("/*", {
                    /* Settings */
                    .compression = uWS::SHARED_COMPRESSOR,
                    .maxPayloadLength = 16 * 1024,
                    .idleTimeout = 10,
                    .maxBackpressure = 1 * 1024 * 1024,
                    /* Handlers */
                    .upgrade = nullptr,
                    .open = [](auto */*ws*/) {

                    },
                    .message = [](auto *ws, string_view message, uWS::OpCode opCode) {
                        ws->send(message, opCode);
                    },
                    .drain = [](auto */*ws*/) {
                        /* Check getBufferedAmount here */
                    },
                    .ping = [](auto */*ws*/) {

                    },
                    .pong = [](auto */*ws*/) {

                    },
                    .close = [](auto */*ws*/, int /*code*/, string_view /*message*/) {

                    }
            }).listen(9001, [](auto *listen_socket) {
                if (listen_socket) {
                    cout << "Thread " << this_thread::get_id() << " listening on port " << 9001 << endl;
                } else {
                    cout << "Thread " << this_thread::get_id() << " failed to listen on port 9001" << endl;
                }
            }).run();

        });
    });

    for (thread *t : threads) {
        t->join();
    }
}