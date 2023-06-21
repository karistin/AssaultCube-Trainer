#include "stdafx.h"
#include "proc.h"



/// <summary>
/// 해당 프로세스 이름을 가지고 프로세스 반환(DWOD)
/// </summary>
/// <param name="procName">프로세스 이름</param>
/// <returns></returns>
// https://learn.microsoft.com/ko-kr/windows/win32/api/tlhelp32/nf-tlhelp32-createtoolhelp32snapshot
DWORD GetProcId(const wchar_t* procName)
{
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				// 대소문자 구분없이 비교 
				if (!_wcsicmp(procEntry.szExeFile, procName))
				{
					procId = procEntry.th32ProcessID;
					//std::cout << procId << std::endl;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));
		}
	}
	CloseHandle(hSnap);

	return procId;
}

/// <summary>
/// 모듈 주소 반환
/// </summary>
/// <param name="procId">프로세스 이름</param>
/// <param name="modName">ac_client로 포인터로 지정할 base 주소</param>
/// <returns></returns>
uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	uintptr_t moduleBaseAddress = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 moduleEntry;
		moduleEntry.dwSize = sizeof(MODULEENTRY32);

		if (Module32First(hSnap, &moduleEntry))
		{
			do
			{
				if (!_wcsicmp(moduleEntry.szModule, modName))
				{
					moduleBaseAddress = (uintptr_t)moduleEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &moduleEntry));
		}
	}
	CloseHandle(hSnap);

	return moduleBaseAddress;
}

/// <summary>
/// 해당 오프셋과 주소를 더하여 반환 
/// </summary>
/// <param name="hProc">해당 프로세스</param>
/// <param name="ptr">로컬 플레이어 주소</param>
/// <param name="offsets"> 추가할 오프셋</param>
/// <returns></returns>
// https://learn.microsoft.com/ko-kr/windows/win32/api/memoryapi/nf-memoryapi-readprocessmemory
uintptr_t FindMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); i++)
	{
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];

	}

	return addr;
}
