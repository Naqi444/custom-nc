#include "commandHandler.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>

void ConnectionHandler::executeCommand(int socket, const std::string& command) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return;
    }

    if (pid == 0) {
        dup2(socket, STDIN_FILENO);
        dup2(socket, STDOUT_FILENO);
        dup2(socket, STDERR_FILENO);

        close(socket);

        const char* cmd = "/bin/sh";
        const char* args[] = {cmd, "-c", command.c_str(), nullptr};
        execvp(cmd, const_cast<char* const*>(args));

        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else {
        close(socket);
    }
}
