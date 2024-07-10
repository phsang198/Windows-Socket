#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

void handleClient(SOCKET clientSocket) {
    char recvBuf[512];
    while (true) {
        int bytesReceived = recv(clientSocket, recvBuf, 512, 0);
        if (bytesReceived > 0) {
            recvBuf[bytesReceived] = '\0';
            std::cout << "Received from client: " << recvBuf << std::endl;

            // Gửi lại thông điệp cho client
            send(clientSocket, recvBuf, bytesReceived, 0);
        }
        else if (bytesReceived == 0) {
            std::cout << "Connection closed by client." << std::endl;
            break;
        }
        else {
            std::cerr << "recv() failed: " << WSAGetLastError() << std::endl;
            break;
        }
    }

    // Đóng kết nối client
    closesocket(clientSocket);
}

int main() {
    // Khởi tạo Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }

    // Tạo socket
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Ràng buộc socket với địa chỉ và cổng
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(54000);
    if (InetPton(AF_INET, L"127.0.0.1", &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "bind() failed." << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Lắng nghe kết nối
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Error at listen(): " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server is listening on port 54000..." << std::endl;

    while (true) {
        // Chấp nhận kết nối từ client
        SOCKET clientSocket;
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "accept() failed." << std::endl;
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        std::cout << "Client connected!" << std::endl;

        // Tạo một thread mới để xử lý client
        std::thread clientThread(handleClient, clientSocket);
        clientThread.detach();
    }

    // Đóng socket lắng nghe và làm sạch Winsock
    closesocket(listenSocket);
    WSACleanup();

    return 0;
}
