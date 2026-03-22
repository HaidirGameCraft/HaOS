#pragma once

// 32 Bits Color for VESA Graphics Mode
#define R8G8B8(r8, g8, b8) (dword)(((r8 & 0xFF) << 16) | ((g8 & 0xFF) << 8) | ((b8) & 0xFF))