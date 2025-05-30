#include <string>
#include "../include/RedisServer.h""
#include "RedisServer.cpp"

int main(int argc, char* argv[]) {
    int port = 6379; //default
    if (argc>=2) port = std::stoi(argv[1]);

    RedisServer server(port);
    return 0;
}