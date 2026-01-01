#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#pragma pack(push, 1)
struct PlayerPacket
{
    uint8_t valid;   // 1 = live, 0 = loading
    float x;
    float y;
    float z;
};
#pragma pack(pop)

void SetColor(WORD c)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

int main()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // 🔹 NON-BLOCKING
    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(27015);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sock, (sockaddr*)&addr, sizeof(addr));

    SetColor(10);
    std::cout << "[SERVER] Started on port 27015\n";
    SetColor(9);
    std::cout << "[SERVER] Waiting for players...\n";
    SetColor(7);

    bool connected = false;
    DWORD lastPacketTime = 0;

    float lastX = 0, lastY = 0, lastZ = 0;
    bool hasLast = false;

    while (true)
    {
        PlayerPacket pkt{};
        sockaddr_in from{};
        int fromLen = sizeof(from);

        int ret = recvfrom(
            sock,
            (char*)&pkt,
            sizeof(pkt),
            0,
            (sockaddr*)&from,
            &fromLen
        );

        DWORD now = GetTickCount64();

        if (ret == sizeof(pkt))
        {
            lastPacketTime = now;

            if (!connected)
            {
                connected = true;
                SetColor(14);
                std::cout << "[PLAYER] Player 1 connected\n";
                SetColor(7);
            }

            // 🔥 ON AFFICHE LES COORDONNÉES QUOI QU’IL ARRIVE
            if (!hasLast ||
                pkt.x != lastX ||
                pkt.y != lastY ||
                pkt.z != lastZ)
            {
                std::cout
                    << "[PLAYER] X=" << pkt.x
                    << " Y=" << pkt.y
                    << " Z=" << pkt.z;

                if (!pkt.valid)
                    std::cout << " (loading)";

                std::cout << "\n";

                lastX = pkt.x;
                lastY = pkt.y;
                lastZ = pkt.z;
                hasLast = true;
            }
        }

        // 🔹 TIMEOUT DISCONNECT
        if (connected && now - lastPacketTime > 3000)
        {
            connected = false;
            hasLast = false;

            SetColor(14);
            std::cout << "[PLAYER] Player disconnected\n";
            SetColor(9);
            std::cout << "[SERVER] Waiting for players...\n";
            SetColor(7);
        }

        Sleep(1);
    }
}
