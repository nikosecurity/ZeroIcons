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

	printf("[+] Patched address 0x%p.\n", pTargetAddress);
}

void WriteGlowPatch(HANDLE hProcess, PVOID pBaseAddress, CHAR Enabled)
{
	ULONG_PTR Bytes = 0;

	PVOID pGlowPointer = 0;
	PVOID pGlowL2Pointer = 0;

	pGlowPointer = (PCHAR)(pBaseAddress)+GLOW_OFFSET;
	printf("[+] Glow Pointer: 0x%p\n", pGlowPointer);

	if (!ReadProcessMemory(hProcess, pGlowPointer, &Bytes, sizeof(Bytes), 0))
	{
		printf("[-] Failed to read process memory. Error: %d (0x%x)\n", GetLastError(), GetLastError());
		return;
	}

	pGlowL2Pointer = (PCHAR)(Bytes + GLOW_POINTER_OFFSET);
	printf("[+] Glow Level 2 Pointer: 0x%p\n", pGlowL2Pointer);

	if (!WriteProcessMemory(hProcess, pGlowL2Pointer, &Enabled, 1, 0))
	{
		printf("[-] Failed to write process memory. Error: %d (0x%x)\n", GetLastError(), GetLastError());
		return;
	}

	printf("[+] %s glow. Address: 0x%p\n", Enabled ? "Enabled" : "Disabled", pGlowL2Pointer);
}