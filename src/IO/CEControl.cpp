#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/CEControl.h"
#include <windows.h>
#include <rapi.h>

#undef FindNextFile
#undef DeleteFile
#undef CreateDirectory

typedef struct
{
	HANDLE handle;
	Bool lastFound;
	CE_FIND_DATA findData;
} FindFileSession;

IO::CEControl::CEControl()
{
//	CeRapiInit();
	RAPIINIT riCopy;
	this->inited = false;
	ZeroMemory(&riCopy, sizeof(RAPIINIT));
	riCopy.cbSize = sizeof(riCopy);

	if (CeRapiInitEx(&riCopy) == S_OK)
	{
        DWORD dwRapiInit = 0;

        dwRapiInit = WaitForSingleObject(riCopy.heRapiInit, 5000);
        if (WAIT_OBJECT_0 == dwRapiInit)
        {
            inited = true;
        }
        else if (WAIT_TIMEOUT == dwRapiInit)
        {
            CeRapiUninit();
        }
        else
        {
			CeRapiUninit();
        }
	}
}

IO::CEControl::~CEControl()
{
	if (inited)
	{
        CeRapiUninit();
		inited = false;
	}
}

Bool IO::CEControl::IsError()
{
	return !inited;
}

void *IO::CEControl::FindFile(const WChar *path)
{
	FindFileSession *sess;
	sess = MemAlloc(FindFileSession, 1);
	sess->lastFound = true;
	sess->handle = CeFindFirstFile(path, &sess->findData);
	if (sess->handle != INVALID_HANDLE_VALUE)
	{
		return sess;
	}
	UInt32 lastErr = CeGetLastError();
	if (lastErr == ERROR_NO_MORE_FILES)
	{
		sess->lastFound = false;
		return sess;
	}
	else
	{
		MemFree(sess);
		return 0;
	}
}

WChar *IO::CEControl::FindNextFile(WChar *buff, void *session, Data::DateTime *modTime, IO::Path::PathType *pt)
{
	IO::Path::PathType tmp;
	if (pt == 0)
		pt = &tmp;

	FindFileSession *sess = (FindFileSession*)session;
	WChar *outPtr;
	if (sess->lastFound)
	{
		outPtr = Text::StrConcat(buff, sess->findData.cFileName);
		if (modTime)
		{
			SYSTEMTIME st;
			FileTimeToSystemTime(&sess->findData.ftLastWriteTime, &st);
			modTime->SetValue(st.wYear, (UInt8)st.wMonth, (UInt8)st.wDay, (UInt8)st.wHour, (UInt8)st.wMinute, (UInt8)st.wSecond, st.wMilliseconds);
		}
		if (sess->findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			*pt = IO::Path::PathType::Directory;
		}
		else
		{
			*pt = IO::Path::PathType::File;
		}
		sess->lastFound = (CeFindNextFile(sess->handle, &sess->findData) != 0);
		return outPtr;
	}
	else
	{
		return 0;
	}
}

void IO::CEControl::FindFileClose(void *session)
{
	FindFileSession *sess = (FindFileSession*)session;
	if (sess->handle != INVALID_HANDLE_VALUE)
		CeFindClose(sess->handle);
	MemFree(sess);
}

IO::Path::PathType IO::CEControl::GetPathType(const WChar *path)
{
	UInt32 fatt = CeGetFileAttributes(path);
	if (fatt == INVALID_FILE_ATTRIBUTES)
	{
		return IO::Path::PathType::Unknown;
	}
	else if (fatt & FILE_ATTRIBUTE_DIRECTORY)
	{
		return IO::Path::PathType::Directory;
	}
	else
	{
		return IO::Path::PathType::File;
	}
}

Bool IO::CEControl::DeleteFile(const WChar *ceFile)
{
	return CeDeleteFile(ceFile) != 0;
}

Bool IO::CEControl::DeleteDirectory(const WChar *ceDir)
{
	return CeRemoveDirectory(ceDir) != 0;
}

Bool IO::CEControl::IsDirectoryExist(const WChar *ceDir)
{
	UInt32 fatt = CeGetFileAttributes(ceDir);
	if (fatt == INVALID_FILE_ATTRIBUTES)
		return false;
	else
		return (fatt & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

Bool IO::CEControl::CreateDirectory(const WChar *ceDir)
{
	WChar sbuff[256];
	if (IsDirectoryExist(ceDir))
		return true;
	Text::StrConcat(sbuff, ceDir);
	UOSInt i = Text::StrLastIndexOf(sbuff, '\\');
	if (i == INVALID_INDEX)
		return CeCreateDirectory(sbuff, 0) != 0;
	if (sbuff[i - 1] != ':')
	{
		sbuff[i] = 0;
		this->CreateDirectory(sbuff);
		sbuff[i] = '\\';
	}
	return CeCreateDirectory(sbuff, 0) != 0;
}

IO::FileStream *IO::CEControl::OpenFile(const WChar *fileName, IO::FileStream::FileMode mode, IO::FileStream::FileShare share)
{
	Int32 shflag;
	Int64 currPos;
	void *handle;

	if (share == IO::FileStream::FileShare::DenyNone)
	{
		shflag = FILE_SHARE_READ | FILE_SHARE_WRITE;
	}
	else if (share == IO::FileStream::FileShare::DenyRead)
	{
		shflag = FILE_SHARE_WRITE;
	}
	else if (share == IO::FileStream::FileShare::DenyWrite)
	{
		shflag = FILE_SHARE_READ;
	}
	else
	{
		shflag = 0;
	}	

	if (mode == FileStream::FILE_MODE_CREATE)
	{
		handle = CeCreateFile(fileName, GENERIC_READ | GENERIC_WRITE, shflag, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		currPos = 0;
	}
	else if (mode == FileStream::FILE_MODE_APPEND)
	{
		handle = CeCreateFile(fileName, GENERIC_READ | GENERIC_WRITE, shflag, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (handle == INVALID_HANDLE_VALUE)
		{
			currPos = -1;
		}
		else
		{
			Int32 fleng = 0;
			currPos = CeSetFilePointer(handle, 0, (PLONG)&fleng, FILE_END);
			((Int32*)&currPos)[1] = fleng;
		}
	}
	else if (mode == FileStream::FILE_MODE_READONLY)
	{
		handle = CeCreateFile(fileName, GENERIC_READ, shflag, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		currPos = 0;
	}
	else
	{
		handle = INVALID_HANDLE_VALUE;
		currPos = -1;
	}
	if (handle == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	IO::FileStream *stm;
	NEW_CLASS(stm, CEFileStream(handle, currPos));
	return stm;
}

IO::CEFileStream::CEFileStream(void *hand, Int64 currPos) : IO::FileStream()
{
	this->handle = hand;
	this->currPos = currPos;
}

IO::CEFileStream::~CEFileStream()
{
	this->Close();
}

Int64 IO::CEFileStream::Seek(IO::SeekableStream::SeekType origin, Int64 position)
{
	if (handle == INVALID_HANDLE_VALUE)
		return -1;
	Int32 hipos;
	Int32 lopos;
	lopos = (Int32)position;
	hipos = ((Int32*)&position)[1];
	if (origin == IO::SeekableStream::ST_BEGIN)
	{
		if (this->currPos == position)
			return this->currPos;
		this->currPos = CeSetFilePointer(handle, lopos, (PLONG)&hipos, FILE_BEGIN);
		((Int32*)&this->currPos)[1] = hipos;
		return this->currPos;
	}
	else if (origin == IO::SeekableStream::ST_CURRENT)
	{
		if (position == 0)
			return this->currPos;
		this->currPos = CeSetFilePointer(handle, lopos, (PLONG)&hipos, FILE_CURRENT);
		((Int32*)&this->currPos)[1] = hipos;
		return this->currPos;
	}
	else if (origin == IO::SeekableStream::ST_END)
	{
		this->currPos = CeSetFilePointer(handle, lopos, (PLONG)&hipos, FILE_END);
		((Int32*)&this->currPos)[1] = hipos;
		return this->currPos;
	}
	return -1;
}

OSInt IO::CEFileStream::Read(UInt8 *buff, OSInt size)
{
	if (handle == INVALID_HANDLE_VALUE)
		return -1;
	UInt32 readSize;
	if (CeReadFile(handle, buff, (UInt32)size, (DWORD*)&readSize, 0))
	{
		this->currPos += readSize;
		return readSize;
	}
	else
	{
		return -1;
	}
}

OSInt IO::CEFileStream::Write(const UInt8 *buff, OSInt size)
{
	if (handle == INVALID_HANDLE_VALUE)
		return -1;
	UInt32 readSize;
	if (CeWriteFile(handle, buff, (UInt32)size, (DWORD*)&readSize, 0))
	{
		this->currPos += readSize;
		return readSize;
	}
	else
	{
		return -1;
	}
}

void *IO::CEFileStream::BeginRead(UInt8 *buff, OSInt size, Sync::Event *evt)
{
	evt->Set();
	if (handle == INVALID_HANDLE_VALUE)
		return (void*)(OSInt)-1;
	UInt32 readSize;
	if (CeReadFile(handle, buff, (UInt32)size, (DWORD*)&readSize, 0))
	{
		this->currPos += readSize;
		return (void*)(OSInt)readSize;
	}
	else
	{
		return (void*)(OSInt)-1;
	}
}

OSInt IO::CEFileStream::EndRead(void *reqData, Bool toWait)
{
	return (OSInt)reqData;
}

void IO::CEFileStream::CancelRead(void *reqData)
{
}

void *IO::CEFileStream::BeginWrite(const UInt8 *buff, OSInt size, Sync::Event *evt)
{
	evt->Set();
	if (handle == INVALID_HANDLE_VALUE)
		return (void*)(OSInt)-1;
	UInt32 readSize;
	if (CeWriteFile(handle, buff, (UInt32)size, (DWORD*)&readSize, 0))
	{
		this->currPos += readSize;
		return (void*)(OSInt)readSize;
	}
	else
	{
		return (void*)(OSInt)-1;
	}
}

OSInt IO::CEFileStream::EndWrite(void *reqData, Bool toWait)
{
	return (OSInt)reqData;
}

void IO::CEFileStream::CancelWrite(void *reqData)
{
}

Int32 IO::CEFileStream::Flush()
{
	return 0;
}

void IO::CEFileStream::Close()
{
	if (handle != INVALID_HANDLE_VALUE)
	{
		CeCloseHandle(handle);
		handle = INVALID_HANDLE_VALUE;
	}
}

Int64 IO::CEFileStream::GetPosition()
{
	return currPos;
}

Int64 IO::CEFileStream::GetLength()
{
	DWORD fsHigh;
	DWORD fsLow = CeGetFileSize(handle, &fsHigh);
	if (fsLow == INVALID_FILE_SIZE)
	{
		return -1;
	}
	else
	{
		return fsLow | (((Int64)fsHigh) << 32);
	}
}

Bool IO::CEFileStream::IsError()
{
	return handle == INVALID_HANDLE_VALUE;
}

Int32 IO::CEFileStream::GetErrCode()
{
	return CeGetLastError();
}

void IO::CEFileStream::GetFileTimes(Data::DateTime *creationTime, Data::DateTime *lastAccessTime, Data::DateTime *lastWriteTime)
{
	FILETIME createTime;
	FILETIME lastAccTime;
	FILETIME lastWrTime;
	SYSTEMTIME sysTime;
	CeGetFileTime(this->handle, &createTime, &lastAccTime, &lastWrTime);
	if (creationTime)
	{
		FileTimeToSystemTime(&createTime, &sysTime);
		creationTime->SetValueSYSTEMTIME(&sysTime);
	}
	if (lastAccessTime)
	{
		FileTimeToSystemTime(&lastAccTime, &sysTime);
		lastAccessTime->SetValueSYSTEMTIME(&sysTime);
	}
	if (lastWriteTime)
	{
		FileTimeToSystemTime(&lastWrTime, &sysTime);
		lastWriteTime->SetValueSYSTEMTIME(&sysTime);
	}
}

void IO::CEFileStream::SetFileTimes(Data::DateTime *creationTime, Data::DateTime *lastAccessTime, Data::DateTime *lastWriteTime)
{
	FILETIME createTime;
	FILETIME lastAccTime;
	FILETIME lastWrTime;
	FILETIME *cTime = 0;
	FILETIME *laTime = 0;
	FILETIME *lwTime = 0;
	SYSTEMTIME sysTime;

	if (creationTime)
	{
		creationTime->ToSYSTEMTIME(&sysTime);
		SystemTimeToFileTime(&sysTime, cTime = &createTime);
	}
	if (lastAccessTime)
	{
		lastAccessTime->ToSYSTEMTIME(&sysTime);
		SystemTimeToFileTime(&sysTime, laTime = &lastAccTime);
	}
	if (lastWriteTime)
	{
		lastWriteTime->ToSYSTEMTIME(&sysTime);
		SystemTimeToFileTime(&sysTime, lwTime = &lastWrTime);
	}
	CeSetFileTime(this->handle, cTime, laTime, lwTime);
}
