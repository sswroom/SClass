#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Event.h"
#include "IO/FileStream.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>
#undef CreateNamedPipe

void IO::FileStream::InitStream(UnsafeArrayOpt<const WChar> fileName, FileMode mode, FileShare share, BufferType buffType)
{
	handle = (void*)-1;
	UnsafeArray<const WChar> nnfileName;
	if (!fileName.SetTo(nnfileName))
	{
		this->currPos = 0;
		handle = INVALID_HANDLE_VALUE;
		return;
	}
	else if (nnfileName[0] == 0)
	{
		this->currPos = 0;
		handle = INVALID_HANDLE_VALUE;
		return;
	}

	SECURITY_ATTRIBUTES secAttr;
	UInt32 shflag;
	secAttr.nLength = sizeof(secAttr);
	secAttr.lpSecurityDescriptor = 0;
	secAttr.bInheritHandle = FALSE; ////////////////////////////////////
	if (share == IO::FileShare::DenyNone)
	{
		shflag = FILE_SHARE_READ | FILE_SHARE_WRITE;
	}
	else if (share == IO::FileShare::DenyRead)
	{
		shflag = FILE_SHARE_WRITE;
	}
	else if (share == IO::FileShare::DenyWrite)
	{
		shflag = FILE_SHARE_READ;
	}
	else
	{
		shflag = 0;
	}

	UInt32 fileFlag;
	switch (buffType)
	{
	case BufferType::RandomAccess:
		fileFlag = FILE_FLAG_RANDOM_ACCESS;
		break;
	case BufferType::Normal:
		fileFlag = 0;
		break;
	case BufferType::Sequential:
		fileFlag = FILE_FLAG_SEQUENTIAL_SCAN;
		break;
	case BufferType::NoBuffer:
		fileFlag = FILE_FLAG_NO_BUFFERING;
		break;
	case BufferType::NoWriteBuffer:
		fileFlag = FILE_FLAG_WRITE_THROUGH;
		break;
	default:
		fileFlag = 0;
		break;
	}

	if (mode == IO::FileMode::Create)
	{
		handle = CreateFileW(fileName.Ptr(), GENERIC_READ | GENERIC_WRITE, shflag, &secAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | fileFlag, 0);
		currPos = 0;
	}
	else if (mode == IO::FileMode::CreateWrite)
	{
		handle = CreateFileW(fileName.Ptr(), GENERIC_WRITE, shflag, &secAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | fileFlag, 0);
		currPos = 0;
	}
	else if (mode == IO::FileMode::Append)
	{
		handle = CreateFileW(fileName.Ptr(), GENERIC_READ | GENERIC_WRITE, shflag, &secAttr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | fileFlag, 0);
		if (handle == INVALID_HANDLE_VALUE)
		{
			this->currPos = 0;
		}
		else
		{
			Int32 fleng = 0;
			this->currPos = SetFilePointer(handle, 0, (PLONG)&fleng, FILE_END);
			((Int32*)&this->currPos)[1] = fleng;
		}
	}
	else if (mode == IO::FileMode::ReadOnly)
	{
		handle = CreateFileW(fileName.Ptr(), GENERIC_READ, shflag, &secAttr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | fileFlag, 0);
		currPos = 0;
	}
	else if (mode == IO::FileMode::ReadWriteExisting)
	{
		handle = CreateFileW(fileName.Ptr(), GENERIC_READ | GENERIC_WRITE, shflag, &secAttr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | fileFlag, 0);
		currPos = 0;
	}
	else if (mode == IO::FileMode::Device)
	{
		handle = CreateFileW(fileName.Ptr(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
		currPos = 0;
	}
}

IO::FileStream::FileStream() : IO::SeekableStream(CSTR("FileStream"))
{
	this->handle = INVALID_HANDLE_VALUE;
	this->currPos = 0;
}

IO::FileStream::FileStream(NN<Text::String> fileName, FileMode mode, FileShare share, BufferType buffType) : IO::SeekableStream(fileName)
{
	handle = (void*)-1;
	if (fileName->leng == 0)
	{
		this->currPos = 0;
		handle = INVALID_HANDLE_VALUE;
		return;
	}

	SECURITY_ATTRIBUTES secAttr;
	UInt32 shflag;
	secAttr.nLength = sizeof(secAttr);
	secAttr.lpSecurityDescriptor = 0;
	secAttr.bInheritHandle = FALSE;////////////////////////////
	if (share == IO::FileShare::DenyNone)
	{
		shflag = FILE_SHARE_READ | FILE_SHARE_WRITE;
	}
	else if (share == IO::FileShare::DenyRead)
	{
		shflag = FILE_SHARE_WRITE;
	}
	else if (share == IO::FileShare::DenyWrite)
	{
		shflag = FILE_SHARE_READ;
	}
	else
	{
		shflag = 0;
	}

	UInt32 fileFlag;
	switch (buffType)
	{
	case BufferType::RandomAccess:
		fileFlag = FILE_FLAG_RANDOM_ACCESS;
		break;
	case BufferType::Normal:
		fileFlag = 0;
		break;
	case BufferType::Sequential:
		fileFlag = FILE_FLAG_SEQUENTIAL_SCAN;
		break;
	case BufferType::NoBuffer:
		fileFlag = FILE_FLAG_NO_BUFFERING;
		break;
	case BufferType::NoWriteBuffer:
		fileFlag = FILE_FLAG_WRITE_THROUGH;
		break;
	default:
		fileFlag = 0;
		break;
	}


	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(fileName->v);
	if (mode == IO::FileMode::Create)
	{
		handle = CreateFileW(wptr.Ptr(), GENERIC_READ | GENERIC_WRITE, shflag, &secAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | fileFlag, 0);
		currPos = 0;
	}
	else if (mode == IO::FileMode::CreateWrite)
	{
		handle = CreateFileW(wptr.Ptr(), GENERIC_WRITE, shflag, &secAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | fileFlag, 0);
		currPos = 0;
	}
	else if (mode == IO::FileMode::Append)
	{
		handle = CreateFileW(wptr.Ptr(), GENERIC_READ | GENERIC_WRITE, shflag, &secAttr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | fileFlag, 0);
		if (handle == INVALID_HANDLE_VALUE)
		{
			this->currPos = 0;
		}
		else
		{
			Int32 fleng = 0;
			this->currPos = SetFilePointer(handle, 0, (PLONG)&fleng, FILE_END);
			((Int32*)&this->currPos)[1] = fleng;
		}
	}
	else if (mode == IO::FileMode::ReadOnly)
	{
		handle = CreateFileW(wptr.Ptr(), GENERIC_READ, shflag, &secAttr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | fileFlag, 0);
		currPos = 0;
	}
	Text::StrDelNew(wptr);
}

IO::FileStream::FileStream(Text::CStringNN fileName, IO::FileMode mode, FileShare share, BufferType buffType) : IO::SeekableStream(fileName)
{
	handle = (void*)-1;
	if (fileName.leng == 0)
	{
		this->currPos = 0;
		handle = INVALID_HANDLE_VALUE;
		return;
	}

	SECURITY_ATTRIBUTES secAttr;
	UInt32 shflag;
	secAttr.nLength = sizeof(secAttr);
	secAttr.lpSecurityDescriptor = 0;
	secAttr.bInheritHandle = FALSE;////////////////////////////
	if (share == IO::FileShare::DenyNone)
	{
		shflag = FILE_SHARE_READ | FILE_SHARE_WRITE;
	}
	else if (share == IO::FileShare::DenyRead)
	{
		shflag = FILE_SHARE_WRITE;
	}
	else if (share == IO::FileShare::DenyWrite)
	{
		shflag = FILE_SHARE_READ;
	}
	else
	{
		shflag = 0;
	}

	UInt32 fileFlag;
	switch (buffType)
	{
	case BufferType::RandomAccess:
		fileFlag = FILE_FLAG_RANDOM_ACCESS;
		break;
	case BufferType::Normal:
		fileFlag = 0;
		break;
	case BufferType::Sequential:
		fileFlag = FILE_FLAG_SEQUENTIAL_SCAN;
		break;
	case BufferType::NoBuffer:
		fileFlag = FILE_FLAG_NO_BUFFERING;
		break;
	case BufferType::NoWriteBuffer:
		fileFlag = FILE_FLAG_WRITE_THROUGH;
		break;
	default:
		fileFlag = 0;
		break;
	}


	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(fileName.v);
	if (mode == IO::FileMode::Create)
	{
		handle = CreateFileW(wptr.Ptr(), GENERIC_READ | GENERIC_WRITE, shflag, &secAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | fileFlag, 0);
		currPos = 0;
	}
	else if (mode == IO::FileMode::CreateWrite)
	{
		handle = CreateFileW(wptr.Ptr(), GENERIC_WRITE, shflag, &secAttr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | fileFlag, 0);
		currPos = 0;
	}
	else if (mode == IO::FileMode::Append)
	{
		handle = CreateFileW(wptr.Ptr(), GENERIC_READ | GENERIC_WRITE, shflag, &secAttr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | fileFlag, 0);
		if (handle == INVALID_HANDLE_VALUE)
		{
			this->currPos = 0;
		}
		else
		{
			Int32 fleng = 0;
			this->currPos = SetFilePointer(handle, 0, (PLONG)&fleng, FILE_END);
			((Int32*)&this->currPos)[1] = fleng;
		}
	}
	else if (mode == IO::FileMode::ReadOnly)
	{
		handle = CreateFileW(wptr.Ptr(), GENERIC_READ, shflag, &secAttr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | fileFlag, 0);
		currPos = 0;
	}
	Text::StrDelNew(wptr);
}

IO::FileStream::~FileStream()
{
	Close();
}

Bool IO::FileStream::IsDown() const
{
	return this->handle == INVALID_HANDLE_VALUE;
}

Bool IO::FileStream::IsError() const
{
	return this->handle == INVALID_HANDLE_VALUE;
}

UIntOS IO::FileStream::Read(const Data::ByteArray &buff)
{
	if (handle == INVALID_HANDLE_VALUE)
		return 0;
	UInt32 readSize;
	if (ReadFile(handle, buff.Arr().Ptr(), (UInt32)buff.GetSize(), (DWORD*)&readSize, 0))
	{
		this->currPos += readSize;
		return readSize;
	}
	else
	{
//		UInt32 err = GetLastError();
		return 0;
	}
}

UIntOS IO::FileStream::Write(Data::ByteArrayR buff)
{
	if (handle == INVALID_HANDLE_VALUE)
		return 0;
	UInt32 readSize;
	if (WriteFile(handle, buff.Ptr(), (UInt32)buff.GetSize(), (DWORD*)&readSize, 0))
	{
		this->currPos += readSize;
		return readSize;
	}
	else
	{
		return 0;
	}
}

Int32 IO::FileStream::Flush()
{
	if (handle == INVALID_HANDLE_VALUE)
		return 0;
	return FlushFileBuffers(handle);
}

void IO::FileStream::Close()
{
	if (handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(handle);
		handle = INVALID_HANDLE_VALUE;
	}
}

Bool IO::FileStream::Recover()
{
	/////////////////////////////////////
	return false;
}

IO::StreamType IO::FileStream::GetStreamType() const
{
	return IO::StreamType::File;
}

UInt64 IO::FileStream::SeekFromBeginning(UInt64 position)
{
	if (handle == INVALID_HANDLE_VALUE)
		return 0;
	if (this->currPos == (UInt64)position)
		return this->currPos;
	Int32 hipos;
	Int32 lopos;
	lopos = (Int32)position;
	hipos = ((Int32*)&position)[1];
	this->currPos = SetFilePointer(handle, lopos, (PLONG)&hipos, FILE_BEGIN);
	((Int32*)&this->currPos)[1] = hipos;
	return this->currPos;
}

UInt64 IO::FileStream::SeekFromCurrent(Int64 position)
{
	if (handle == INVALID_HANDLE_VALUE)
		return 0;
	if (position == 0)
		return this->currPos;
	Int32 hipos;
	Int32 lopos;
	lopos = (Int32)position;
	hipos = ((Int32*)&position)[1];
	this->currPos = SetFilePointer(handle, lopos, (PLONG)&hipos, FILE_CURRENT);
	((Int32*)&this->currPos)[1] = hipos;
	return this->currPos;
}

UInt64 IO::FileStream::SeekFromEnd(Int64 position)
{
	if (handle == INVALID_HANDLE_VALUE)
		return 0;
	Int32 hipos;
	Int32 lopos;
	lopos = (Int32)position;
	hipos = ((Int32*)&position)[1];
	this->currPos = SetFilePointer(handle, lopos, (PLONG)&hipos, FILE_END);
	((Int32*)&this->currPos)[1] = hipos;
	return this->currPos;
}

UInt64 IO::FileStream::GetPosition()
{
	return currPos;
}

UInt64 IO::FileStream::GetLength()
{
	DWORD dwError;
	DWORD fsHigh;
	DWORD fsLow = GetFileSize(handle, &fsHigh);
	if (fsLow == INVALID_FILE_SIZE && (dwError = GetLastError()) != NO_ERROR)
	{
		return 0;
	}
	else
	{
		return fsLow | (((UInt64)fsHigh) << 32);
	}
}

void IO::FileStream::SetLength(UInt64 newLength)
{
	Int32 hipos;
	Int32 lopos;
	lopos = (Int32)newLength;
	hipos = ((Int32*)&newLength)[1];
	SetFilePointer(handle, lopos, (PLONG)&hipos, FILE_BEGIN);
	SetEndOfFile(handle);
	lopos = (Int32)this->currPos;
	hipos = ((Int32*)&this->currPos)[1];
	SetFilePointer(handle, lopos, (PLONG)&hipos, FILE_BEGIN);
}

Int32 IO::FileStream::GetErrCode()
{
	return (Int32)GetLastError();
}

void IO::FileStream::GetFileTimes(Optional<Data::DateTime> creationTime, Optional<Data::DateTime> lastAccessTime, Optional<Data::DateTime> lastWriteTime)
{
	FILETIME createTime;
	FILETIME lastAccTime;
	FILETIME lastWrTime;
	SYSTEMTIME sysTime;
	GetFileTime(this->handle, &createTime, &lastAccTime, &lastWrTime);
	NN<Data::DateTime> t;
	if (creationTime.SetTo(t))
	{
		FileTimeToSystemTime(&createTime, &sysTime);
		t->SetValueSYSTEMTIME(&sysTime);
	}
	if (lastAccessTime.SetTo(t))
	{
		FileTimeToSystemTime(&lastAccTime, &sysTime);
		t->SetValueSYSTEMTIME(&sysTime);
	}
	if (lastWriteTime.SetTo(t))
	{
		FileTimeToSystemTime(&lastWrTime, &sysTime);
		t->SetValueSYSTEMTIME(&sysTime);
	}
}

void IO::FileStream::GetFileTimes(OptOut<Data::Timestamp> creationTime, OptOut<Data::Timestamp> lastAccessTime, OptOut<Data::Timestamp> lastWriteTime)
{
	FILETIME createTime;
	FILETIME lastAccTime;
	FILETIME lastWrTime;
	GetFileTime(this->handle, &createTime, &lastAccTime, &lastWrTime);
	if (creationTime.IsNotNull())
	{
		creationTime.SetNoCheck(Data::Timestamp(Data::TimeInstant::FromFILETIME(&createTime), 0));
	}
	if (lastAccessTime.IsNotNull())
	{
		lastAccessTime.SetNoCheck(Data::Timestamp(Data::TimeInstant::FromFILETIME(&lastAccTime), 0));
	}
	if (lastWriteTime.IsNotNull())
	{
		lastWriteTime.SetNoCheck(Data::Timestamp(Data::TimeInstant::FromFILETIME(&lastWrTime), 0));
	}
}

Data::Timestamp IO::FileStream::GetCreateTime()
{
	FILETIME createTime;
	FILETIME lastAccTime;
	FILETIME lastWrTime;
	GetFileTime(this->handle, &createTime, &lastAccTime, &lastWrTime);
	return Data::Timestamp::FromFILETIME(&createTime, Data::DateTimeUtil::GetLocalTzQhr());
}

Data::Timestamp IO::FileStream::GetModifyTime()
{
	FILETIME createTime;
	FILETIME lastAccTime;
	FILETIME lastWrTime;
	GetFileTime(this->handle, &createTime, &lastAccTime, &lastWrTime);
	return Data::Timestamp::FromFILETIME(&lastWrTime, Data::DateTimeUtil::GetLocalTzQhr());
}

void IO::FileStream::SetFileTimes(Optional<Data::DateTime> creationTime, Optional<Data::DateTime> lastAccessTime, Optional<Data::DateTime> lastWriteTime)
{
	FILETIME createTime;
	FILETIME lastAccTime;
	FILETIME lastWrTime;
	FILETIME *cTime = 0;
	FILETIME *laTime = 0;
	FILETIME *lwTime = 0;
	SYSTEMTIME sysTime;
	NN<Data::DateTime> t;

	if (creationTime.SetTo(t))
	{
		t->ToUTCTime();
		t->ToSYSTEMTIME(&sysTime);
		SystemTimeToFileTime(&sysTime, cTime = &createTime);
	}
	if (lastAccessTime.SetTo(t))
	{
		t->ToUTCTime();
		t->ToSYSTEMTIME(&sysTime);
		SystemTimeToFileTime(&sysTime, laTime = &lastAccTime);
	}
	if (lastWriteTime.SetTo(t))
	{
		t->ToUTCTime();
		t->ToSYSTEMTIME(&sysTime);
		SystemTimeToFileTime(&sysTime, lwTime = &lastWrTime);
	}
	SetFileTime(this->handle, cTime, laTime, lwTime);
}

void IO::FileStream::SetFileTimes(const Data::Timestamp &creationTime, const Data::Timestamp &lastAccessTime, const Data::Timestamp &lastWriteTime)
{
	FILETIME createTime;
	FILETIME lastAccTime;
	FILETIME lastWrTime;
	FILETIME *cTime = 0;
	FILETIME *laTime = 0;
	FILETIME *lwTime = 0;

	if (!creationTime.IsNull())
	{
		creationTime.ToFILETIME(cTime = &createTime);
	}
	if (!lastAccessTime.IsNull())
	{
		lastAccessTime.ToFILETIME(laTime = &lastAccTime);
	}
	if (!lastWriteTime.IsNull())
	{
		lastWriteTime.ToFILETIME(lwTime = &lastWrTime);
	}
	SetFileTime(this->handle, cTime, laTime, lwTime);
}

Optional<IO::FileStream> IO::FileStream::CreateNamedPipe(UnsafeArray<const UTF8Char> pipeName, UInt32 buffSize)
{
#ifdef _WIN32_WCE
	return 0;
#else
	WChar wbuff[256];
	UnsafeArray<WChar> wptr = Text::StrConcat(wbuff, L"\\\\.\\pipe\\");
	HANDLE hand;
	wptr = Text::StrUTF8_WChar(wptr, pipeName, 0);
	hand = ::CreateNamedPipeW(wbuff, PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE, PIPE_UNLIMITED_INSTANCES, buffSize, buffSize, 0, 0);
	if (hand == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream());
	fs->handle = hand;
	fs->currPos = 0;
	return fs;
#endif
}

Optional<IO::FileStream> IO::FileStream::OpenNamedPipe(UnsafeArrayOpt<const UTF8Char> server, UnsafeArray<const UTF8Char> pipeName)
{
#ifdef _WIN32_WCE
	return 0;
#else
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<const UTF8Char> nnserver;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("\\\\"));
	if (!server.SetTo(nnserver))
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("."));
	}
	else
	{
		sptr = Text::StrConcat(sptr, nnserver);
	}
	sptr = Text::StrConcatC(sptr, UTF8STRC("\\pipe\\"));
	sptr = Text::StrConcat(sptr, pipeName);
	IO::FileStream *outStm;
	NEW_CLASS(outStm, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Device, IO::FileShare::DenyAll, IO::FileStream::BufferType::Normal));
	if (outStm->IsError())
	{
		DEL_CLASS(outStm);
		return 0;
	}
	return outStm;
#endif
}

UIntOS IO::FileStream::LoadFile(Text::CStringNN fileName, UnsafeArray<UInt8> buff, UIntOS maxBuffSize)
{
	IO::FileStream fs(fileName, FileMode::ReadOnly, FileShare::DenyNone, BufferType::Normal);
	if (fs.IsError())
	{
		return 0;
	}
	UInt64 fileLen = fs.GetLength();
	if (fileLen > maxBuffSize || fileLen == 0)
	{
		return 0;
	}
	UIntOS readSize = fs.Read(Data::ByteArray(buff, maxBuffSize));
	if (readSize == fileLen)
	{
		return readSize;
	}
	return 0;
}
