#include "pch.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdint>

#pragma comment(lib, "Ws2_32.lib")

// =====================================================
// NETWORK CONFIG
// =====================================================

#define SERVER_IP   "127.0.0.1"
#define SERVER_PORT 27015

constexpr uintptr_t XZ_230 = 0x230;
constexpr uintptr_t XZ_2C0 = 0x2C0;
constexpr uintptr_t XZ_238 = 0x238;
constexpr uintptr_t XZ_00 = 0x00;
constexpr uintptr_t XZ_20 = 0x20;
constexpr uintptr_t XZ_1B8 = 0x1B8;
constexpr uintptr_t OFF_X = 0x21C;
constexpr uintptr_t OFF_Z = 0x224;

// Y
constexpr uintptr_t BASE_Y = 0x0A38D5A0;
constexpr uintptr_t Y_3C0 = 0x3C0;
constexpr uintptr_t Y_8B8 = 0x8B8;
constexpr uintptr_t Y_340 = 0x340;
constexpr uintptr_t Y_A8 = 0xA8;
constexpr uintptr_t Y_158 = 0x158;
constexpr uintptr_t Y_698 = 0x698;
constexpr uintptr_t OFF_Y = 0xCF4;

// =====================================================

uintptr_t GetModuleBase()
{
    return (uintptr_t)GetModuleHandleW(L"VoyageSteam-Win64-Shipping.exe");
}

// =====================================================
// Resolve X/Z base
// =====================================================

uintptr_t ResolveXZ()
{
    uintptr_t base = GetModuleBase();
    if (!base) return 0;

    uintptr_t addr = *(uintptr_t*)(base + BASE_XZ);

    __try
    {
        addr = *(uintptr_t*)(addr + XZ_230);
        addr = *(uintptr_t*)(addr + XZ_2C0);
        addr = *(uintptr_t*)(addr + XZ_238);
        addr = *(uintptr_t*)(addr + XZ_00);
        addr = *(uintptr_t*)(addr + XZ_20);
        addr = *(uintptr_t*)(addr + XZ_1B8);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return 0;
    }

    return addr;
}

// =====================================================
// Resolve Y base
// =====================================================

uintptr_t ResolveY()
{
    uintptr_t base = GetModuleBase();
    if (!base) return 0;

    uintptr_t addr = *(uintptr_t*)(base + BASE_Y);

    __try
    {
        addr = *(uintptr_t*)(addr + Y_3C0);
        addr = *(uintptr_t*)(addr + Y_8B8);
        addr = *(uintptr_t*)(addr + Y_340);
        addr = *(uintptr_t*)(addr + Y_A8);
        addr = *(uintptr_t*)(addr + Y_158);
        addr = *(uintptr_t*)(addr + Y_698);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return 0;
    }

    return addr;
}

// =====================================================
// Safe float printing (2 decimals, no CRT)
// =====================================================

DWORD WINAPI NetThread(LPVOID)
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 0;

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET)
        return 0;

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr);

    // CONNECT
    PlayerPacket pkt{};
    pkt.type = 1;
    sendto(sock, (char*)&pkt, sizeof(pkt), 0,
        (sockaddr*)&server, sizeof(server));

    while (true)
    {
        uintptr_t xzBase = ResolveXZ();
        uintptr_t yBase = ResolveY();

        if (xzBase && yBase)
        {
            __try
            {
                float x = *(float*)(xzBase + OFF_X);
                float z = *(float*)(xzBase + OFF_Z);
                float y = *(float*)(yBase + OFF_Y);

                PlayerPacket pos{};
                pos.type = 2;
                pos.x = x;
                pos.y = y;
                pos.z = z;

                sendto(sock, (char*)&pos, sizeof(pos), 0,
                    (sockaddr*)&server, sizeof(server));
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                // ignore read errors, keep alive
            }
        }

        Sleep(300); // ~3 updates per second
    }

    return 0;
}

// =====================================================
// DLL ENTRY
// =====================================================

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, NetThread, nullptr, 0, nullptr);
    }
    return TRUE;
}
