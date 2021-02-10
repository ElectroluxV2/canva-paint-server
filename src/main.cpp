#include "CanvaPaintServer.h"

using namespace std;

int main(int argc, char** argv) {

    CanvaPaintServer server;

    // Initialize configs and saved data
    if (!server.Init()) {
        cerr << "Can't initialize server!" << endl;
        return 1;
    }

    // Start server at default port 3000
    if (!server.StartThreaded(thread::hardware_concurrency(),argc > 1 ? stoi(argv[1]) : 3000)) {
        cerr << "Can't start threaded server" << endl;
        return 2;
    }

    // Start console interface
    string command;
    while (command != "shutdown") {
        // Get command
        cin >> noskipws >> command;

        if (cin.fail()) {
            // Clear error state
            cin.clear();
            // Discard 'bad' character(s)
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        cout << "Executing: \"" << command << "\"" << endl;

        if (command == "restart") {
            server.Restart();
        }
    }

    // Shutdown server
    server.Shutdown();

    /*transform(threads.begin(), threads.end(), threads.begin(), [](thread *t) {
        return new thread([]() {

            // Very simple WebSocket echo server
            uWS::App().ws<PerSocketData>("/*", {
                // Settings
                .compression = uWS::SHARED_COMPRESSOR,
                .maxPayloadLength = 16 * 1024,
                .idleTimeout = 10,
                .maxBackpressure = 1 * 1024 * 1024,
                // Handlers
                .upgrade = nullptr,
                .open = [](auto *ws) {
                    cout << "open" << endl;
                },
                .message = [](auto *ws, string_view message, uWS::OpCode opCode) {
                    cout << "msg: " << message << endl;
                    // ws->send(message, opCode);
                },
                .drain = [](auto *ws) {
                    // Check getBufferedAmount here
                },
                .ping = [](auto *ws) {
                    cout << "ping" << endl;

                },
                .pong = [](auto *ws) {

                },
                .close = [](auto *ws, int code, string_view message) {
                    cout << "close" << endl;
                }
            }).listen(3000, [](auto *listen_socket) {
                if (listen_socket) {
                    cout << "Thread " << this_thread::get_id() << " listening on port " << 3000 << endl;
                } else {
                    cout << "Thread " << this_thread::get_id() << " failed to listen on port 3000" << endl;
                }
            }).run();

        });
    });*/
}