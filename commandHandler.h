#ifndef CONNECTION_HANDLER_H
#define CONNECTION_HANDLER_H

#include <string>

class ConnectionHandler {
public:
    static void executeCommand(int socket, const std::string& command);
};

#endif // CONNECTION_HANDLER_H
