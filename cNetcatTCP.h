#ifndef NETCAT_TCP_H
#define NETCAT_TCP_H

#include <string>

class NetcatTCP {
public:
    NetcatTCP(int port, bool listenMode, bool zeroOnly, const std::string& executeCmd);
    ~NetcatTCP();

    void run();

private:
    int sockfd;
    int port;
    bool listenMode;
    bool zeroOnly;
    std::string executeCmd;

    void setupServer();
    void setupClient();
    void handleClient(int clientSock);
    void executeCommand(const std::string& command);
};

#endif // NETCAT_TCP_H
