#include "../include/RedisServer.h"
#include "../include/RedisCommandHandler.h"
#include "../include/RedisDatabase.h"

#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <thread>
#include <cstring>
#include <signal.h>

static RedisServer* globalServer = nullptr;

void signalHandler(int signum) {
    if (globalServer) {
        std::cout << "Caught signal " << signum << ", shutting down...\n";
        globalServer->shutdown();
    }
    exit(signum);
}

void RedisServer::setupSignalHandler() {
    signal(SIGINT, signalHandler);
}

RedisServer::RedisServer(int port) : port(port), server_socket(-1), running(true) {
    globalServer = this;
    setupSignalHandler();
}

void RedisServer::shutdown() {
    running = false;
    if (server_socket != -1) {
        close(server_socket);
    }
    std::cout << "#### Server sucessfully shutdown ####\n";
}

void RedisServer::run() {
server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        std::cerr << "Error Creating Server Socket\n";
    }
    
    // Set socket options
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind to an address
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Error Binding Server Socket\n";
    }

    // Listen
    if(listen(server_socket, 10) < 0) {
        std::cerr << "Error Listening On Server Socket\n";
    }

    std::cout << "Redis Server Listening on Port " << port << "\n";

    std::vector<std::thread> threads;
    RedisCommandHandler cmdHandler;

    while (running)
    {
        // Accept incoming connection from client and create a new fd/socket for it
        int client_socket = accept(server_socket, nullptr, nullptr); // server_socket is where all the connection requests get queued
        if (client_socket < 0) { // Unsuccessful connection
            if (running) {
                std::cerr << "Error Accepting Client Connection\n";
                break;
            }    
        }
        
        threads.emplace_back([client_socket, &cmdHandler](){
            char buffer[1024];
            while(true) { // Keep listening to requests coming from client
                memset(buffer, 0, sizeof(buffer));
                int bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0); // Receive byte data
                if (bytes <= 0) break; // If nothing comes through break out of the loop
                std::string request(buffer, bytes);
                std::string response = cmdHandler.processCommand(request); // Process request
                send(client_socket, response.c_str(), response.size(), 0); // Send response back to client
            }
            close(client_socket);

        });
    }
    for (auto& t: threads) {
        if (t.joinable()) t.join();
    }

    // Before shutdown, persist the database
    if (RedisDatabase::getInstance().dump("dump.my_rdb")) {
        std::cout << "Database dumped to dump.my_rdb\n";
    }
    else {
        std::cerr << "Error Dumping Database\n";
    }
}