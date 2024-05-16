#include <iostream>
#include <cstring>
#include "cNetcatTCP.cpp"

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " -l -p [port] [-e command]" << std::endl;
        return 1;
    }

    bool listenMode = false;
    bool execMode = false;
    std::string command;
    std::string host;
    int port = 0;
    int i = 1;

    while (i < argc) {
        if (strcmp(argv[i], "-l") == 0) {
            listenMode = true;
        } else if (strcmp(argv[i], "-p") == 0) {
            if (i + 1 < argc) {
                port = std::stoi(argv[++i]);
            }
        } else if (strcmp(argv[i], "-e") == 0) {
            execMode = true;
            if (i + 1 < argc) {
                command = argv[++i];
            }
        } else {
            host = argv[i];
        }
        i++;
    }

    if (port == 0) {
        std::cerr << "Port not specified correctly." << std::endl;
        return 1;
    }

    cNetcatTCP nc;

    if (listenMode) {
        if (execMode) {
            nc.listenWithCommand(port, command);
        } else {
            nc.listen(port);
        }
    } else {
        if (execMode) {
            std::cerr << "-e option is only valid in listen mode." << std::endl;
            return 1;
        }
        nc.connect(host, port);
    }

    return 0;
}
