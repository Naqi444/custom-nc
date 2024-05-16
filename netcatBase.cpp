#include <iostream>
#include <string>

class cNetcatBase {
public:
    virtual void listen(int port)  = 0;
    virtual void connect(const std::string& host, int port) = 0;
};