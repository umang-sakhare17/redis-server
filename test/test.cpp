#include <vector>
#include <string>
#include <iostream>

// Say we have a string *2\r\n$4\r\nPING\r\n$4\r\nTEST\r\n
// we want to take this and convert it to a vector with commands in it

// std::vector<std::string> tokens;
// if(input.empty()) return tokens;

// size_t pos = 0;
// int i = 0;

// if(input[0] != '*') return tokens;
// pos++;

// // find how many elements in this RESP input (think about whether it will always be on 1st position)
// int numElements = std::stoi(input[pos]);

// // run the loop as many times as the command is expected
// while(i < numElements) {
//     pos = pos + 2; // move past /r/n
//     if(input[pos] != '$') return tokens; // now we are expecting $ so check if this is true, not sure what to return here
//     int len = std::stoi(input[++pos]); // get the value after $ and convert it to int
//     pos = pos + 2; // move past /r/n

//     for(int j = 0; j < len; j++) {
//         std::string token = input.substr(pos, len); // get 'len' number of characters after pos to get the command
//         tokens.push_back(token);
//         pos += len + 2;
//     }
// };

// return tokens;

std::vector<std::string> parseRespCommand(const std::string& input) {
    std::vector<std::string> tokens;

    std::cout << "[DEBUG] Input received: " << input << std::endl;

    if (input.empty()) {
        std::cout << "[DEBUG] Input is empty.\n";
        return tokens;
    }

    size_t pos = 0;
    int i = 0;

    if (input[pos] != '*') {
        std::cout << "[DEBUG] Input does not start with '*'. Exiting.\n";
        return tokens;
    }
    pos++;  // move past '*'

    // Find first position for crlf
    size_t crlf = input.find("\r\n", pos);
    if (crlf == std::string::npos) {
        std::cout << "[DEBUG] Couldn't find CRLF after '*'.\n";
        return tokens;
    }

    // Find number of RESP commands
    int numElements = std::stoi(input.substr(pos, crlf - pos));
    std::cout << "[DEBUG] numElements: " << numElements << "\n";

    pos = crlf + 2;

    while (i < numElements) {
        if(pos > input.size() || input[pos] != '$') {
            std::cout << "[DEBUG] Expected '$' at position " << pos << ". Found: " << input[pos] << "\n";
            return tokens;
        }
        pos++; // move past '$'

        crlf = input.find("\r\n", pos);
        if (crlf == std::string::npos) {
            std::cout << "[DEBUG] Couldn't find CRLF after '$'.\n";
            return tokens;
        }

        int len = std::stoi(input.substr(pos, crlf - pos));
        std::cout << "[DEBUG] Element length: " << len << "\n";

        pos = crlf + 2;

        // Check if there's enough chars left in string to prevent buffer overflow
        if (pos + len > input.size()) {
            std::cout << "[DEBUG] Not enough characters for the token. Needed: " << len << "\n";
            return tokens;
        }

        std::string token = input.substr(pos, len);
        std::cout << "[DEBUG] Token parsed: " << token << "\n";
        tokens.push_back(token);

        pos = pos + len + 2;
        i++;
    }
    return tokens;
}

int main() {
    // std::string input = "*2\r\n$4\r\nPING\r\n$4\r\nTEST\r\n";
    std::string input = "*2$4\r\nPING\r\n$4\r\nTEST\r\n";
    std::vector<std::string> result = parseRespCommand(input);

    std::cout << "\n=== Final Parsed Tokens ===\n";
    for (const auto& token : result) {
        std::cout << "[" << token << "]\n";
    }
    return 0;
}