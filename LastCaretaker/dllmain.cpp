#include <Windows.h>
#include <cstdint>
#include "pch.h"

// =====================================================
// X / Z POINTER (validated)
// =====================================================

constexpr uintptr_t BASE_XZ = 0x0A7C4118;

constexpr uintptr_t XZ_230 = 0x230;
constexpr uintptr_t XZ_2C0 = 0x2C0;
constexpr uintptr_t XZ_238 = 0x238;
constexpr uintptr_t XZ_00 = 0x00;
constexpr uintptr_t XZ_20 = 0x20;
constexpr uintptr_t XZ_1B8 = 0x1B8;

constexpr uintptr_t OFF_X = 0x21C;
constexpr uintptr_t OFF_Z = 0x224;

// =====================================================
// Y POINTER (validated)
// =====================================================

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

void PrintFloat(const wchar_t* label, float value)
{
    int integer = (int)value;
    int decimal = (int)((value - integer) * 100.0f);
    if (decimal < 0) decimal = -decimal;

    wchar_t buf[64];
    wsprintfW(buf, L"[POS] %s = %d.%02d\n", label, integer, decimal);
    OutputDebugStringW(buf);
}

// =====================================================
// Main thread
// =====================================================

DWORD WINAPI MainThread(LPVOID)
{
    OutputDebugStringW(L"[DLL] Player position reader started\n");
    Sleep(2000);

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

                PrintFloat(L"X", x);
                PrintFloat(L"Y", y);
                PrintFloat(L"Z", z);
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                OutputDebugStringW(L"[POS] Read error\n");
            }
        }
        else
        {
            OutputDebugStringW(L"[POS] Base not found\n");
        }

        Sleep(300); // ~3 updates per second
    }

    return 0;
}

// =====================================================
// DLL entry point
// =====================================================

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
    }
    return TRUE;
}
