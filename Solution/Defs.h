#pragma once

// Found via reverse engineering GeometryDash.exe.
// For future updates, patch diff'ing will make updating this hack extremely fast.
// I would use signatures, but uh... I don't feel like it.
#define ICONS_OFFSET 0x2771E5
#define COLORS_OFFSET 0x89E1D

#define ICONS_ORIGINAL_BYTES 0x75C084FFF05316E8
#define COLORS_ORIGINAL_BYTES 0x74C084000F2ABEE8

#define ICONS_PATCHED_BYTES 0x75C08490909001B0
#define COLORS_PATCHED_BYTES 0x74C08490909001B0

#define GLOW_OFFSET 0x6C2ED8
#define GLOW_POINTER_OFFSET 0x388

void WritePatch(HANDLE hProcess, PVOID pTargetAddress, ULONG_PTR ExpectedBytes, ULONG_PTR PatchedBytes, CHAR* pPatchCode, SIZE_T PatchLength);
void WriteGlowPatch(HANDLE hProcess, PVOID pBaseAddress, CHAR Enabled);