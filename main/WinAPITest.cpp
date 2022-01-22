#include "Stdafx.h"
#include "Core/Core.h"
#include "Text/MyString.h"
#include <Windows.h>

void GetFileInfo(const WChar* fileName)
{
	FILE_BASIC_INFO info;
	HANDLE handle = CreateFileW(fileName, 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, 0);
	GetFileInformationByHandleEx(handle, FileBasicInfo, &info, sizeof(info));
	CloseHandle(handle);
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	WIN32_FILE_ATTRIBUTE_DATA attr;
	GetFileAttributesA("C:\\Progs\\SClass\\build\\Windows_x64\\smake.exe");
	GetFileAttributesW(L"C:\\Progs\\SClass\\build\\Windows_x64\\smake.exe");
	GetFileAttributesExA("C:\\Progs\\SClass\\build\\Windows_x64\\smake.exe", GetFileExInfoStandard, &attr);
	GetFileAttributesExW(L"C:\\Progs\\SClass\\build\\Windows_x64\\smake.exe", GetFileExInfoStandard, &attr);
	GetFileInfo(L"C:\\Progs\\SClass\\build\\Windows_x64\\smake.exe");
	return 0;
}
