#include "mem.h"
#include "stdafx.h"
//https://learn.microsoft.com/ko-kr/windows/win32/api/memoryapi/nf-memoryapi-virtualprotectex
//https://learn.microsoft.com/ko-kr/windows/win32/api/memoryapi/nf-memoryapi-writeprocessmemory
// src로 해당 dst의 메모리 값을 변경 
void mem::PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess)
{
	DWORD oldprotect;
	VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	WriteProcessMemory(hProcess, dst, src, size, nullptr);
	VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	
}
// nop로 해당 dst의 메모리 값을 변경 
void mem::NopEx(BYTE* dst, unsigned int size, HANDLE hProcess)
{
	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size);

	PatchEx(dst, nopArray, size, hProcess);
	delete[] nopArray;
}
