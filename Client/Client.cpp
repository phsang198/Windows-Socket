#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    // Khởi tạo Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }

    // Tạo socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Thiết lập địa chỉ server
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(54000);
    if (InetPton(AF_INET, L"127.0.0.1", &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Kết nối tới server
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to the server!" << std::endl;

    // Vòng lặp giao tiếp với server
    char sendBuf[512];
    char recvBuf[512];
    while (true) {
        std::cout << "Enter message to send: ";
        std::cin.getline(sendBuf, 512);

        // Gửi thông điệp tới server
        int sendResult = send(clientSocket, sendBuf, strlen(sendBuf), 0);
        if (sendResult == SOCKET_ERROR) {
            std::cerr << "send() failed: " << WSAGetLastError() << std::endl;
            break;
        }

        // Nhận thông điệp từ server
        int bytesReceived = recv(clientSocket, recvBuf, 512, 0);
        if (bytesReceived > 0) {
            recvBuf[bytesReceived] = '\0';
            std::cout << "Received from server: " << recvBuf << std::endl;
        }
        else if (bytesReceived == 0) {
            std::cout << "Connection closed by server." << std::endl;
            break;
        }
        else {
            std::cerr << "recv() failed: " << WSAGetLastError() << std::endl;
            break;
        }
    }

    // Đóng kết nối và làm sạch Winsock
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
