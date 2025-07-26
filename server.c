#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib") // Link with Winsock library

#define PORT 8080

// Get current time in milliseconds on Windows
long long current_time_in_ms()
{
    FILETIME fileTime;
    GetSystemTimeAsFileTime(&fileTime);

    ULARGE_INTEGER time;
    time.LowPart = fileTime.dwLowDateTime;
    time.HighPart = fileTime.dwHighDateTime;

    // Convert from 100-nanosecond intervals since January 1, 1601 to milliseconds
    return (long long)(time.QuadPart / 10000);
}

int main()
{
    WSADATA wsaData;
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[1024] = "Hello Het Bhaiya!!";

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    // Create TCP socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("Socket creation failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Server address setup
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Connection failed with error: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // Record start time
    long long start_time = current_time_in_ms();

    // Send message to server
    send(sockfd, buffer, strlen(buffer), 0);
    printf("Message sent to server\n");

    // Receive server response
    int n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (n > 0)
    {
        buffer[n] = '\0';
        // Print the received message
        printf("Received from server: %s\n", buffer);
    }

    // Record end time
    long long end_time = current_time_in_ms();

    // Calculate RTT
    long long rtt = end_time - start_time;
    printf("RTT: %lld ms\n", rtt);

    // Cleanup
    closesocket(sockfd);
    WSACleanup();

    return 0;
}
