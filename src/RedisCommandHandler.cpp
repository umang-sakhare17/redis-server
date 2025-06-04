#include <vector>
#include <sstream>
#include <algorithm>
#include <../include/RedisCommandHandler.h>
/*
RESP Parser:
 *2\r\n$4\r\n\PING\r\n$4\r\nTEST\r\n
 *2 -> Array of 2 elements
 \r\n -> Delimiter, sort of like a space between the args. Officially stands for carriage return and new line
 $4 -> next arg has 4 characters
 PING
 similarly followed by TEST
*/

std::vector<std::string> parseRespCommand(const std::string &input) {
    // First we check if it is empty
    std::vector<std::string> tokens;
    if(input.empty()) return tokens;

    // If it does not start with the expected '*', we fall back to splitting by whitespace.
    if (input[0] != '*') {
        // We convert input string to a input stream object 
        // so its easier to go over it word by word instead of char by char
        std::istringstream iss(input);
        std::string token;

        // >> reads word one at a time and stores them in token
        while (iss >> token) {
            // token is appended to tokens vector
            tokens.push_back(token);
        }
        return tokens;
    }

    size_t pos = 0;
    // Expect '*' followed by number of elements
    if (input[pos] != '*') return tokens; // ensure that the string begins with * as per RESP
    pos++; // move past '*'

    // Find first position for crlf
    size_t crlf = input.find("\r\n", pos);
    if(crlf == std::string::npos) return tokens; // if crlf not found, return token

    int numElements = std::stoi(input.substr(pos, crlf - pos));
    pos = crlf + 2; // skip ahead two characters \r and \n

    for (int i = 0; i < numElements; i++) {
        if (pos >= input.size() || input[pos] != '$') break;
        pos++; // skip '$'

        crlf = input.find("\r\n", pos);
        if (crlf == std::string::npos) break;
        int len = std::stoi(input.substr(pos, crlf - pos));
        pos = crlf + 2;

        if (pos + len > input.size()) break;
        std::string token = input.substr(pos, len);
        tokens.push_back(token);
        pos += len + 2; // skip token and CRLF
    }
    return tokens;
}

// Initiate command handler
RedisCommandHandler::RedisCommandHandler(){}

// Process the incoming command
std::string RedisCommandHandler::processCommand(const std::string& commandLine) {
     // User the RESP parser
     auto tokens = parseRespCommand(commandLine);
     if(tokens.empty()) return "Error: Empty Command\r\n";

     std::string cmd = tokens[0];
     std:transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
     std::ostringstream response;

     // Connect to database

     // Run commands

     return response.str();
}