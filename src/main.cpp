#include <string>
#include "../include/RedisServer.h"
#include <thread>
#include <chrono>

int main(int argc, char* argv[]) {
    int port = 6379; //default
    if (argc>=2) port = std::stoi(argv[1]);

    RedisServer server(port);

    // Create background persistance. Save contents to database every 300 seconds.
    std::thread persistanceThread([](){
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(300));
            // Dump the database
        }
    });
    persistanceThread.detach();

    server.run();

    return 0;
}