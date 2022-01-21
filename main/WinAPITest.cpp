#include "Stdafx.h"
#include "Core/Core.h"
#include "Text/MyString.h"
#include <Windows.h>

Int32 MyMain(Core::IProgControl *progCtrl)
{
	WIN32_FILE_ATTRIBUTE_DATA attr;
	GetFileAttributesA("C:\\Progs\\SClass\\build\\Windows_x64\\smake.exe");
	GetFileAttributesW(L"C:\\Progs\\SClass\\build\\Windows_x64\\smake.exe");
	GetFileAttributesExA("C:\\Progs\\SClass\\build\\Windows_x64\\smake.exe", GetFileExInfoStandard, &attr);
	GetFileAttributesExW(L"C:\\Progs\\SClass\\build\\Windows_x64\\smake.exe", GetFileExInfoStandard, &attr);
	return 0;
}
