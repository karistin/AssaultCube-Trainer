
#include "stdafx.h"
#include "proc.h"
#include <iostream>


int main()
{
    // get proID of target process
    DWORD procId = GetProcId(L"ac_client.exe");

    //get module base address
    uintptr_t moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");
    
    //get handle to process 
    HANDLE hProcess = 0;
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
    //https://learn.microsoft.com/ko-kr/windows/win32/api/processthreadsapi/nf-processthreadsapi-openprocess
    
    
    //Resolve base address of the pointer chain 
    uintptr_t dynamicPtrBaseAddr = moduleBase + 0x18AC00;

    std::cout << "DynamicPtrBaseAddr = " << "0x" << std::hex << dynamicPtrBaseAddr << std::endl;
    
    //Resolve our ammo pointer chain 
    //라이플 권총 수류탄 
    std::vector<unsigned int> ammoOffsets = {0x0140, 0x012C, 0x0144};
    uintptr_t ammoAddr = FindDMAAddy(hProcess, dynamicPtrBaseAddr, {0xF0});
    
    
    std::cout << "ammoAddr = " << "0x" << std::hex << ammoAddr << std::endl;

    //Read Ammo value
    int ammoValue = 0;
    ReadProcessMemory(hProcess, (BYTE*)ammoAddr, &ammoValue, sizeof(ammoValue), nullptr);
    std::cout << "Current ammo = " << std::dec << ammoAddr << std::endl;

    //Write to it
    //int newAmmo = 1337;
    //WriteProcessMemory(hProcess, (BYTE*)ammoAddr, &newAmmo, sizeof(newAmmo), nullptr);

    return 0;
}


