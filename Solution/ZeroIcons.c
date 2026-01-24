#pragma warning (disable : 6031) // I don't need the return value of getchar, I just want to use it to pause the program instead of immediately exiting.

#include <Windows.h>
#include <Psapi.h>
#include <stdio.h>

#include "Defs.h"

// TODO: Add support for patching the binary on the disk directly.
// This is extremely dangerous as it can prevent GD from starting entirely.
// So... if I add this, I have to be very careful and very precise.

CHAR g_pPatch[] =
{
	0xB0, 0x01,	// mov al, 1
	0x90,		// nop
	0x90,		// nop
	0x90		// nop
};

// Credits to...
// Absolute - The original 2.2 Unlock All/Icon Hack (which I reverse engineered; I could've done it my own way, but the code would've looked worse than it does now)
int main(int argc, char** argv)
{
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	HWND hWindow = 0;

	DWORD ProcessID = 0;
	HANDLE hProcess = 0;

	HMODULE* phModules = 0;
	DWORD BytesReturned = 0;

	CHAR pBaseName[MAX_PATH] = { 0 };

	HMODULE hModule = 0;

	PVOID pIconsAddress = 0;
	PVOID pColorsAddress = 0;
	PVOID pGlowAddress = 0;

	SetConsoleTitleA("ZeroIcons Icon Hack");
	printf
	(
		"[*] ZeroIcons 2.2 Icon Hack\n"
		"[*] Written by The Exploit\n\n"
		"[*] Finding the process window...\n"
	);

	while (!hWindow)
	{
		hWindow = FindWindowA(0, "Geometry Dash");
		Sleep(100);
	}
	printf("[+] Window Handle: 0x%p\n", hWindow);

	if (!GetWindowThreadProcessId(hWindow, &ProcessID))
	{
		printf("[-] Failed to get the process ID. Error: %d (0x%x)\n", GetLastError(), GetLastError());
		getchar();
		return 1;
	}
	printf("[+] Process ID: %d (0x%x)\n", ProcessID, ProcessID);

	hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION, 0, ProcessID);
	if (!hProcess)
	{
		printf("[-] Failed to create a process handle. Error: %d (0x%x)\n", GetLastError(), GetLastError());
		getchar();
		return 1;
	}
	printf("[+] Process Handle: 0x%p\n", hProcess);

	K32EnumProcessModules(hProcess, 0, 0, &BytesReturned);

	phModules = malloc(BytesReturned);
	if (!phModules)
	{
		printf("[-] Failed to allocate heap memory.\n");
		getchar();
		return 1;
	}

	if (!K32EnumProcessModules(hProcess, phModules, BytesReturned, &BytesReturned))
	{
		printf("[-] Failed to enumerate process modules. Error: %d (0x%x)\n", GetLastError(), GetLastError());
		getchar();
		return 1;
	}

	for (DWORD i = 0; i < BytesReturned / sizeof(HMODULE); i++)
	{
		memset(pBaseName, 0, sizeof(pBaseName));

		if (!K32GetModuleBaseNameA(hProcess, phModules[i], pBaseName, sizeof(pBaseName)))
		{
			printf("[-] Failed to enumerate module base name. Error: %d (0x%x). Continuing...\n", GetLastError(), GetLastError());
			continue;
		}

		if (!strcmp(pBaseName, "GeometryDash.exe"))
		{
			hModule = phModules[i];
			break;
		}
	}
	if (!hModule)
	{
		printf("[-] Failed to find the process' base address.\n");
		getchar();
		return 1;
	}
	printf("[+] Base Address: 0x%p\n", hModule);

	pIconsAddress = (PCHAR)(hModule)+ICONS_OFFSET;
	pColorsAddress = (PCHAR)(hModule)+COLORS_OFFSET;
	pGlowAddress = (PCHAR)(hModule)+GLOW_OFFSET;
	printf("[+] Icon Unlock Address: 0x%p\n[+] Color Unlock Address: 0x%p\n[+] Glow Unlock Address: 0x%p\n", pIconsAddress, pColorsAddress, pGlowAddress);

	WritePatch(hProcess, pIconsAddress, ICONS_ORIGINAL_BYTES, ICONS_PATCHED_BYTES, g_pPatch, sizeof(g_pPatch));
	WritePatch(hProcess, pColorsAddress, COLORS_ORIGINAL_BYTES, COLORS_PATCHED_BYTES, g_pPatch, sizeof(g_pPatch));
	WritePatch(hProcess, pGlowAddress, GLOW_ORIGINAL_BYTES, GLOW_PATCHED_BYTES, g_pPatch, sizeof(g_pPatch));

	printf("[+] Done! You may now close this window.");
	getchar();
	return 0;
}