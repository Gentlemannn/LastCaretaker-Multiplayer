#include <Windows.h>
#include <cstdint>
#include "pch.h"

// ============================
// Utils
// ============================
uintptr_t GetModuleBase(const wchar_t* moduleName)
{
    return (uintptr_t)GetModuleHandleW(moduleName);
}

// ============================
// Résolution du pointeur position
// ============================
uintptr_t ResolvePositionBase()
{
    uintptr_t base = GetModuleBase(L"VoyageSteam-Win64-Shipping.exe");
    if (!base) return 0;

    uintptr_t p = *(uintptr_t*)(base + 0x0A7C4118);
    if (!p) return 0;

    p = *(uintptr_t*)(p + 0x230);
    if (!p) return 0;

    p = *(uintptr_t*)(p + 0x2C0);
    if (!p) return 0;

    p = *(uintptr_t*)(p + 0x238);
    if (!p) return 0;

    p = *(uintptr_t*)(p + 0x00);
    if (!p) return 0;

    p = *(uintptr_t*)(p + 0x20);
    if (!p) return 0;

    p = *(uintptr_t*)(p + 0x1B8);
    if (!p) return 0;

    return p; // base FVector
}

// ============================
// Thread principal
// ============================
DWORD WINAPI MainThread(LPVOID)
{
    Sleep(5000); // laisser le jeu charger

    char buffer[128];

    while (true)
    {
        __try
        {
            uintptr_t posBase = ResolvePositionBase();

            if (posBase)
            {
                float fx = *(float*)(posBase + 0x21C);
                float fz = *(float*)(posBase + 0x224);

                int xi = (int)(fx * 100.0f);
                int zi = (int)(fz * 100.0f);

                int x_ent = xi / 100;
                int x_dec = xi % 100; if (x_dec < 0) x_dec = -x_dec;

                int z_ent = zi / 100;
                int z_dec = zi % 100; if (z_dec < 0) z_dec = -z_dec;

                wsprintfA(
                    buffer,
                    "[MYMULTI][POS] X=%d.%02d Z=%d.%02d",
                    x_ent, x_dec,
                    z_ent, z_dec
                );
            }
            else
            {
                wsprintfA(buffer, "[MYMULTI][POS] indisponible");
            }

            OutputDebugStringA(buffer);
            OutputDebugStringA("\n");
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            // Ici on absorbe le crash pendant le reload
            OutputDebugStringA("[MYMULTI][WARN] Position invalide (reload)\n");
            Sleep(500); // laisser Unreal respirer
        }

        Sleep(100); // 10 Hz
    }

    return 0;
}


// ============================
// DllMain
// ============================
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
    }
    return TRUE;
}
