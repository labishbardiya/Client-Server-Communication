#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>

#pragma comment(lib, "ws2_32.lib") // Link with Winsock library

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_connection(void *socket_ptr)
{
    int new_socket = *(int *)socket_ptr;
    char buffer[BUFFER_SIZE];

    // Receive and send data
    while (1)
    {
        int n = recv(new_socket, buffer, BUFFER_SIZE - 1, 0);
        if (n <= 0)
        {
            break;
        }
        buffer[n] = '\0';
        printf("Received: %s\n", buffer);

        // Send "Pong!" back to the client
        const char *response = "Pong!";
        send(new_socket, response, strlen(response), 0);
    }

    // Close the connection
    closesocket(new_socket);
    _endthread();
}

int main()
{
    WSADATA wsaData;
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addr_len = sizeof(address);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Create TCP socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Server address setup
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR)
    {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_fd, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("Listen failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    while (1)
    {
        // Accept a connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addr_len)) == INVALID_SOCKET)
        {
            printf("Accept failed. Error Code: %d\n", WSAGetLastError());
            continue;
        }

        // Handle the connection in a new thread
        _beginthread(handle_connection, 0, (void *)&new_socket);
    }

    // Close the server socket
    closesocket(server_fd);
    WSACleanup();

    return 0;
}
