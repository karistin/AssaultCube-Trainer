
#include "stdafx.h"
#include "proc.h"
#include "mem.h"
#include <iostream>

//https://learn.microsoft.com/ko-kr/windows/win32/inputdev/virtual-key-codes
// VK key set 
int main()
{
	const wchar_t gameName[] = L"ac_client.exe";
	
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	HANDLE hProcess = 0;

	uintptr_t moduleBase = 0, localPlayerPtr = 0, healthAddr = 0, ammoAddr = 0, recoil1 = 0, recoil2 = 0;
	bool bHealth = false, bAmmo = false, bRecoil = false; 

	const int newValue = 9999;
	// hp value

	const int fly_on = 5;
	// 5 : fly , 0 : default
	const int fly_off = 0;

	const int no_recoil_on = 0;
	const int recoil_1 = 25;
	const int recoil_2 = 50;


	DWORD procId = GetProcId(gameName);

	if (procId)
	{
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

		moduleBase = GetModuleBaseAddress(procId, gameName);

		localPlayerPtr = moduleBase + 0x17E0A8;

		healthAddr = FindMAAddy(hProcess, localPlayerPtr, {0xEC});

		//ammoAddr = FindMAAddy(hProcess, localPlayerPtr, {0x367 + 0x14});

		recoil1 = FindMAAddy(hProcess, localPlayerPtr, { 0x364, 0xC, 0x5E });
		recoil2 = FindMAAddy(hProcess, localPlayerPtr, { 0x364, 0xC, 0x60 });


		SetConsoleTextAttribute(hConsole, 2);
		std::cout << "ac.client.exe process FOUND! \r\nStaring Menu ..... \r\n ----------------"<<std::endl;

	}
	else
	{
		SetConsoleTextAttribute(hConsole, 4);
		std::cout << "Process not found, press enter to exit " << std::endl;
		getchar();
		return 0;
	}

	DWORD dwExit = 0;

	std::string health_str = "[Z] HEALTH HACK \n",
		ammo_str = "[C] INFINITE AMMO \n",
		recoil_str = "[X] NO RECOIL \n",
		fly_str = "[NUMPAD 1] FLY HACK \n--------------------------\n";

	std::cout << health_str << ammo_str << recoil_str << fly_str;

	while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE)
	{
		
		(bHealth) ? health_str += "[ENABLED] | " : health_str;
		(bAmmo) ? ammo_str += "[ENABLED] | " : ammo_str;
		(bRecoil) ? recoil_str += "[ENABLED] | " : recoil_str;

		// press Z
		if (GetAsyncKeyState(0x5A) & 1)
		{
			bHealth = !bHealth;
			if (bHealth)
			{
				mem::PatchEx((BYTE*)healthAddr, (BYTE*)&newValue, sizeof(newValue), hProcess);
				SetConsoleTextAttribute(hConsole, 2);
				std::cout << "Health Activated ... \r\n";
			}
			else
			{
				SetConsoleTextAttribute(hConsole, 4);
				std::cout << "Health Deactivated .. \r\n";
			}
			
		}
		// press c
		if (GetAsyncKeyState(0x43) & 1)
		{
			bAmmo = !bAmmo;
			if (bAmmo)
			{
				mem::PatchEx((BYTE*)(moduleBase + 0xc73EF), (BYTE*)"\xFF\x00", 2, hProcess); // FF 06 - inc [esi]
				SetConsoleTextAttribute(hConsole, 2);

				std::cout << "Infinite Ammo Activated...\n";

			}
			else
			{
				mem::PatchEx((BYTE*)(moduleBase + 0xc73EF), (BYTE*)"\xFF\x08", 2, hProcess); // FF 0E - dec [esi]
				SetConsoleTextAttribute(hConsole, 4);

				std::cout << "Infinite Ammo Deactivated...\n";

			}

		}

		// press x
		if (GetAsyncKeyState(0x58) & 1) // Key X - No Recoil
		{
			bRecoil = !bRecoil;

			if (bRecoil)
			{
				mem::PatchEx((BYTE*)(recoil1), (BYTE*) &no_recoil_on, sizeof(no_recoil_on), hProcess);
				mem::PatchEx((BYTE*)(recoil2), (BYTE*) &no_recoil_on, sizeof(no_recoil_on), hProcess);
				SetConsoleTextAttribute(hConsole, 2);

				std::cout << "No Recoil Activated...\n";

			}
			else
			{

				mem::PatchEx((BYTE*)(recoil1), (BYTE*)&recoil_1, sizeof(recoil_1), hProcess);
				mem::PatchEx((BYTE*)(recoil2), (BYTE*)&recoil_2, sizeof(recoil_2), hProcess);
				SetConsoleTextAttribute(hConsole, 4);
				std::cout << "No Recoil Deactivated...\n";

			}
		}

		if (GetAsyncKeyState(VK_INSERT) & 1)
		{
			return 0;
		}

		Sleep(25);
		
	}
	CloseHandle(hProcess);

	return 0;
}



