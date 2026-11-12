#include <Windows.h>
#include <string>
#include <TlHelp32.h>
#include <tchar.h>
#include <iostream>
#include "../dllmain.h"
#include "Patching.h"
#include "../Console.h"


namespace PachingUtils {
    const bool NewsBreaker(const HANDLE& ThreadHande, const HANDLE& ProcessHandle, const DWORD BaseAdress) {
        const unsigned char NewNewsString[] = "SG_is_Still_Alive";
        int NewsAdress{};


        ReadProcessMemory(ProcessHandle, (LPVOID)(BaseAdress + 0x00436E54), &NewsAdress, sizeof(NewsAdress), 0);
        if (NewsAdress == 0) {
            return false;
        }
        WriteProcessMemory(ProcessHandle, (LPVOID)(NewsAdress), (LPVOID)NewNewsString, sizeof NewNewsString, NULL);
        std::cout << "Breaking News!\n" << std::endl;
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