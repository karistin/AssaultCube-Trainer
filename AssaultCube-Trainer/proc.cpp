#include "stdafx.h"
#include "proc.h"



/// <summary>
/// �ش� ���μ��� �̸��� ������ ���μ��� ��ȯ(DWOD)
/// </summary>
/// <param name="procName">���μ��� �̸�</param>
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
				// ��ҹ��� ���о��� �� 
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
/// ��� �ּ� ��ȯ
/// </summary>
/// <param name="procId">���μ��� �̸�</param>
/// <param name="modName">ac_client�� �����ͷ� ������ base �ּ�</param>
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
/// �ش� �����°� �ּҸ� ���Ͽ� ��ȯ 
/// </summary>
/// <param name="hProc">�ش� ���μ���</param>
/// <param name="ptr">���� �÷��̾� �ּ�</param>
/// <param name="offsets"> �߰��� ������</param>
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
