#include <Windows.h>
#include <string>
#include <cstring>
#include <TlHelp32.h>
#include <tchar.h>
#include <iostream>
#include "../dllmain.h"
#include "Patching.h"
#include "../Console.h"


namespace PachingUtils {
    // Адрес игрового API лежит в глобальной std::string, заполненной ещё до загрузки
    // модлоудера, поэтому правим саму строку. На 127.0.0.1 порт закрыт: отказ сразу.
    const bool NewsBreaker(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress) {
        const char OldPrefix[] = "https://skgm-prod-game-api";
        const char NewUrl[] = "https://127.0.0.1";
        const DWORD StringAdress = BaseAdress + 0x0085EEF0;

        DWORD Fields[6]{}; // буфер или указатель, длина (+0x10), ёмкость (+0x14)
        if (!ReadProcessMemory(ProcessHandle, (LPCVOID)StringAdress, Fields, sizeof(Fields), NULL)) {
            return false;
        }
        const DWORD Capacity = Fields[5];
        if (Capacity == 0) {
            return false; // строка ещё не создана
        }
        const DWORD Buffer = Capacity < 16 ? StringAdress : Fields[0];

        char Current[sizeof(OldPrefix)]{};
        ReadProcessMemory(ProcessHandle, (LPCVOID)Buffer, Current, sizeof(Current) - 1, NULL);
        if (strncmp(Current, NewUrl, sizeof(NewUrl) - 1) == 0) {
            return true; // уже заменено при прошлом проходе
        }
        // Другая версия игры: не держим остальные патчи в цикле повторов.
        if (strncmp(Current, OldPrefix, sizeof(OldPrefix) - 1) != 0 || Capacity < sizeof(NewUrl) - 1) {
            Console::DLL_WriteOutput("Game API address not found, skipped");
            return true;
        }
        const DWORD NewSize = sizeof(NewUrl) - 1;
        if (!WriteProcessMemory(ProcessHandle, (LPVOID)Buffer, NewUrl, sizeof(NewUrl), NULL) ||
            !WriteProcessMemory(ProcessHandle, (LPVOID)(StringAdress + 0x10), &NewSize, sizeof(NewSize), NULL)) {
            return false;
        }
        Console::DLL_WriteOutput("Breaking News!");
        return true;
    }
    const bool GFSValidathionBreaker(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress) {

        const unsigned char JMP = { 0xEB };
        unsigned char JMPValue{ NULL };

        ReadProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x0011CD67), &JMPValue, 1, 0);
        if (JMPValue == NULL) {
            return false;
        }
        WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x0011CD67), &JMP, 1, NULL);
        Console::DLL_WriteOutput("Breaking .gfs Validathion!");
        return true;
    }
    const bool SalValidathionBreaker(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress) {

        const unsigned char JMP = { 0xEB };
        unsigned char JMPValue{ NULL };


        ReadProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x5C994), &JMPValue, 1, 0);
        if (JMPValue == NULL) {
            return false;
        }
        WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x5C994), &JMP, 1, NULL);
        Console::DLL_WriteOutput("Breaking .sal Validathion!");
        return true;
    }

    const bool ChangeDataDirectoryFirstTime(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress) {
        const unsigned char NewString[] = "/data02/";
        unsigned char OldString[sizeof NewString]{ NULL };

        if (ProcessHandle == NULL || ProcessHandle == INVALID_HANDLE_VALUE) {
            std::cerr << "Invalid process handle\n";
            return false;
        }

        ReadProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x3f3878), &OldString, sizeof(OldString), 0);
        if (OldString == 0) {
            return false;
        }

        DWORD old;
        VirtualProtectEx
        (
            ProcessHandle,
            LPVOID(BaseAdress + 0x3f3000),
            0x4D000,
            PAGE_EXECUTE_READWRITE,
            &old
        );

        WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x3f3878), &NewString, sizeof NewString, 0);

        Console::DLL_WriteOutput("Change Data Directory");
        return true;
    }
    const bool ChangeDataDirectorySecondTime(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress) {
        const unsigned char NewString[] = "data02";
        unsigned char OldString[sizeof NewString]{ NULL };


        ReadProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x3fac38), &OldString, sizeof(OldString), 0);
        if (OldString == 0) {
            return false;
        }

        DWORD old;
        VirtualProtectEx
        (
            ProcessHandle,
            LPVOID(BaseAdress + 0x3fa000),
            0x46000,
            PAGE_EXECUTE_READWRITE,
            &old
        );

        //WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x3fac38), &NewString, sizeof NewString, 0);

        VirtualProtectEx
        (
            ProcessHandle,
            LPVOID(BaseAdress + 0x3fa000),
            0x46000,
            PAGE_READONLY,
            &old
        );
        std::cout << "Change Data Directory Second Time Time\n";
        std::cout << GetLastError() << std::endl;
        return true;
    }
    const bool ChangeSal(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress) {
        char OldString[sizeof(config::SAL_FILE_NAME)]{ NULL };


        ReadProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x3e60cc), &OldString, sizeof(OldString), 0);
        if (OldString == 0) {
            return false;
        }

        DWORD old;
        VirtualProtectEx
        (
            ProcessHandle,
            LPVOID(BaseAdress + 0x3e6000),
            0x1000,
            PAGE_EXECUTE_READWRITE,
            &old
        );

        WriteProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x3e60cc), &config::SAL_FILE_NAME, sizeof config::SAL_FILE_NAME, 0);

        VirtualProtectEx
        (
            ProcessHandle,
            LPVOID(BaseAdress + 0x3e6000),
            0x1000,
            PAGE_READONLY,
            &old
        );

        Console::DLL_WriteOutput("Change .sal File Name");
        return true;
    }
}