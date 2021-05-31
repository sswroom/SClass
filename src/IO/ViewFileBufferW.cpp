#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ViewFileBuffer.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>

IO::ViewFileBuffer::ViewFileBuffer(const UTF8Char *fileName)
{
	SECURITY_ATTRIBUTES secAttr;
	secAttr.nLength = sizeof(secAttr);
	secAttr.lpSecurityDescriptor = 0;
	secAttr.bInheritHandle = TRUE;

	this->filePtr = 0;
	WChar sbuff[512];
	Text::StrUTF8_WChar(sbuff, fileName, 0);

	fileHandle = CreateFileW(sbuff, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, &secAttr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, 0);
	if (fileHandle == 0)
	{
		return;
	}
	mapHandle = CreateFileMapping((HANDLE)fileHandle, 0, PAGE_READONLY, 0, 0, 0);
	if (mapHandle == 0)
	{
		return;
	}
}

IO::ViewFileBuffer::~ViewFileBuffer()
{
	if (fileHandle)
	{
		if (mapHandle)
		{
			FreePointer();
			CloseHandle((HANDLE)mapHandle);
		}
		CloseHandle((HANDLE)fileHandle);
	}
}

UInt8 *IO::ViewFileBuffer::GetPointer()
{
	if (fileHandle == 0 || mapHandle == 0)
	{
		return 0;
	}
	if (this->filePtr == 0)
	{
		this->filePtr = (UInt8*)MapViewOfFile((HANDLE)mapHandle, FILE_MAP_READ, 0, 0, 0);
	}
	return this->filePtr;
}

UInt64 IO::ViewFileBuffer::GetLength()
{
	LARGE_INTEGER li;
	if (GetFileSizeEx(this->fileHandle, &li))
	{
		return li.QuadPart;
	}
	else
	{
		return 0;
	}
}

void IO::ViewFileBuffer::FreePointer()
{
	if (this->filePtr)
	{
		UnmapViewOfFile(this->filePtr);
		this->filePtr = 0;
	}
}

