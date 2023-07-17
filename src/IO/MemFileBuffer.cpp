#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MemFileBuffer.h"
#include "IO/FileStream.h"
#if defined(WIN32) || defined(__CYGWIN__)
#include <windows.h>
#endif

IO::MemFileBuffer::MemFileBuffer(Text::CString fileName)
{
	IO::FileStream *file;

	NEW_CLASS(file, IO::FileStream(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	this->fileSize = file->GetLength();
	if (this->fileSize > 0)
	{
		this->filePtr = MemAlloc(UInt8, (UOSInt)this->fileSize);

#if defined(WIN32) || defined(__CYGWIN__)
		HANDLE hProc;
		UOSInt minSize;
		UOSInt maxSize;
		BOOL res;
		UInt32 err;
		hProc = GetCurrentProcess();
		if (VirtualLock(filePtr, (SIZE_T)fileSize) == 0)
		{
			err = GetLastError();
			if (err == ERROR_WORKING_SET_QUOTA)
			{
				res = GetProcessWorkingSetSize(hProc, (PSIZE_T)&minSize, (PSIZE_T)&maxSize);
				if (res == 0)
				{
					err = GetLastError();
				}
				else
				{
					UInt32 addSize = 0;
					if (fileSize & 1048575)
					{
						addSize = (UInt32)(1048576 - (fileSize & 1048575));
					}
					res = SetProcessWorkingSetSize(hProc, (UInt32)(minSize + fileSize + addSize), (UInt32)(maxSize + fileSize + addSize));
					if (res == 0)
					{
						err = GetLastError();
					}
					res = VirtualLock(filePtr, (SIZE_T)fileSize);
					if (res == 0)
					{
						err = GetLastError();
					}
				}
			}
		}
#endif
		file->SeekFromBeginning(0);
		file->Read(Data::ByteArray(this->filePtr, (UOSInt)fileSize));
	}
	else
	{
		this->filePtr = 0;
	}
	DEL_CLASS(file);
}

IO::MemFileBuffer::~MemFileBuffer()
{
	if (filePtr)
	{
#if defined(WIN32) || defined(__CYGWIN__)
		VirtualUnlock(filePtr, (SIZE_T)fileSize);
#endif
		MemFree(filePtr);
	}
}

UInt8 *IO::MemFileBuffer::GetPointer()
{
	return filePtr;
}

UInt64 IO::MemFileBuffer::GetLength()
{
	return fileSize;
}
