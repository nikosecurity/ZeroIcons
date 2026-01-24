#include <Windows.h>
#include <stdio.h>

#include "Defs.h"

void WritePatch(HANDLE hProcess, PVOID pTargetAddress, ULONG_PTR ExpectedBytes, ULONG_PTR PatchedBytes, CHAR* pPatchCode, SIZE_T PatchLength)
{
	ULONG_PTR Bytes = 0;

	if (!ReadProcessMemory(hProcess, pTargetAddress, &Bytes, sizeof(Bytes), 0))
	{
		printf("[-] Failed to read process memory. Error: %d (0x%x)\n", GetLastError(), GetLastError());
		return;
	}

	if (Bytes == PatchedBytes)
	{
		printf("[+] Looks like this offset is already patched! Skipping... (Found Bytes: 0x%p)\n", (PVOID)Bytes);
		return;
	}
	if (Bytes != ExpectedBytes)
	{
		printf("[-] Failed to find the expected bytes. Did Geometry Dash update? Found Bytes: 0x%p\n", (PVOID)Bytes);
		return;
	}

	if (!WriteProcessMemory(hProcess, pTargetAddress, pPatchCode, PatchLength, 0))
	{
		printf("[-] Failed to write process memory. Error: %d (0x%x)\n", GetLastError(), GetLastError());
		return;
	}

	printf("[+] Patched 0x%p!\n", pTargetAddress);
}