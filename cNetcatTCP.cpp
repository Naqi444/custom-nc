#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "commandHandler.h"
#include "netcatBase.cpp"
#include <cstring>
#include <thread>
#include <cstdlib>
#include <iostream>
#include <pthread.h>


class cNetcatTCP : public cNetcatBase {

private:
    int sockfd;

public:
    cNetcatTCP() {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror ("Cannot create socket");
            exit (EXIT_FAILURE);
        }
    }

    ~cNetcatTCP() {
        close (sockfd);
    }

    void listen(int port) override {
        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(port);

        if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("Bind Failed");
            exit(EXIT_FAILURE);
        }

        if (::listen(sockfd, 5) < 0) {
            perror("Listen failed");
            exit(EXIT_FAILURE);
        }

        std::cout << "Listening on TCP port " << port << std::endl;
        acceptConnections();
    }

    void connect(const std::string& host, int port) override {
        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port  = htons(port);

        if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0) {
            perror("Invalid address");
            exit(EXIT_FAILURE);
        }

        if (::connect(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0) {
            perror("Connection Failed");
            exit(EXIT_FAILURE);
        }

        std::cout << "Connected to " << host << " on port " << port << std::endl;
        handlecConnection();
    }

    void listenWithCommand(int port, const std::string& command) {
        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(port);

        if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("Bind failed");
            exit(EXIT_FAILURE);
        }

        if (::listen(sockfd, 5) < 0) {
            perror("Listen failed");
            exit(EXIT_FAILURE);
        }

        std::cout << "Listening on TCP port " << port << " with command execution" << std::endl;

        while (true) {
            sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_sock = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
            if (client_sock < 0) {
                perror("Accept failed");
                continue;
            }

            // Redirecting stdin, stdout, and stderr to client socket
            ConnectionHandler::executeCommand(client_sock, command);
            close(client_sock);
        }
    }

    void* handleConnection(void *client_sock_ptr) {
        int client_sock = *((int*)client_sock_ptr);
        char buffer[1024];
        ssize_t n;
        std::cout << "Thread started." << std::endl;
        // Read data from client and print
        while ((n = read(client_sock, buffer, sizeof(buffer))) >= 0) {
            if (n > 0) {
                buffer[n] = '\0'; // Null-terminate only if data was received
                std::cout << "Received from client " << client_sock << ":" << buffer << "\n";
            }
        }

        if (n < 0) {
            perror("Error reading from socket");
        }

        // Close client socket
        close(client_sock);

        // Free the memory allocated for client_sock
        delete (int*)client_sock_ptr;

        return nullptr;
    }

    static void* handleConnectionStatic(void* arg) {
        return reinterpret_cast<cNetcatTCP*>(arg)->handleConnection(arg);
    }

    void acceptConnections() {
        while(true) {
            sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int* client_sock_ptr = new int; // Allocate memory for client socket
            *client_sock_ptr = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
            if (*client_sock_ptr < 0) {
                perror("Accept failed");
                delete client_sock_ptr; // Clean up memory in case of failure
                continue;
            }
            std::cout << "Accepted client connection: " << std::endl;
            pthread_t thread;
            if (pthread_create(&thread, nullptr, &cNetcatTCP::handleConnectionStatic, (void*)client_sock_ptr) != 0) {
                perror("Error creating thread");
                delete client_sock_ptr; // Clean up memory in case of failure
                continue;
            }
            pthread_detach(thread);
        }
    }

    void handlecConnection() {
        // Handle an outgoing connection
        char buffer[1024];
        while (true) {
            std::cout << "Enter message: ";
            std::cin.getline(buffer, sizeof(buffer));
            if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
                perror("Send failed");
                break;
            }
        }
    }
};