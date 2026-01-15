#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Event.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

#if defined(WIN32_LEAN_AND_MEAN)
#undef WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <stdio.h>

#include "Win32/COMStream.h"
#define DebugMsg(msg)
//#define DebugMsg(msg) printf("%s\n", msg);

Win32::COMStream::COMStream(NN<IO::SeekableStream> stm)
{
	this->stm = stm;
	this->cnt = 0;
	NEW_CLASS(this->dt, Data::DateTime());
}

Win32::COMStream::~COMStream()
{
	DEL_CLASS(this->dt);
}

HRESULT __stdcall Win32::COMStream::QueryInterface(const IID &riid, void **ppvObject)
{
	DebugMsg("COMQueryInterface");
	*ppvObject = 0;
	return E_NOINTERFACE;
}

ULONG __stdcall Win32::COMStream::AddRef(void)
{
	DebugMsg("AddRef");
	return ++cnt;
}

ULONG __stdcall Win32::COMStream::Release(void)
{
	DebugMsg("Release");
	return --cnt;
}

HRESULT __stdcall Win32::COMStream::Read(void *pv, ULONG cb, ULONG *pcbRead)
{
	DebugMsg("Read");
	if (pcbRead)
		*pcbRead = (ULONG)stm->Read(Data::ByteArray((UInt8*)pv, cb));
	else
		stm->Read(Data::ByteArray((UInt8*)pv, cb));
	return 0;
}

HRESULT __stdcall Win32::COMStream::Write(const void *pv, ULONG cb, ULONG *pcbWritten)
{
	DebugMsg("Write");
	if (pcbWritten)
		*pcbWritten = (ULONG)stm->Write(Data::ByteArrayR((UInt8*)pv, cb));
	else
		stm->Write(Data::ByteArrayR((UInt8*)pv, cb));
	return 0;
}

HRESULT __stdcall Win32::COMStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
{
	DebugMsg("Seek");
	if (dwOrigin == STREAM_SEEK_SET)
	{
		if (plibNewPosition)
			plibNewPosition->QuadPart = this->stm->SeekFromBeginning((UInt64)dlibMove.QuadPart);
		else
			this->stm->SeekFromBeginning((UInt64)dlibMove.QuadPart);
	}
	else if (dwOrigin == STREAM_SEEK_END)
	{
		if (plibNewPosition)
			plibNewPosition->QuadPart = this->stm->SeekFromEnd(dlibMove.QuadPart);
		else
			this->stm->SeekFromEnd(dlibMove.QuadPart);
	}
	else
	{
		if (plibNewPosition)
			plibNewPosition->QuadPart = this->stm->SeekFromCurrent(dlibMove.QuadPart);
		else
			this->stm->SeekFromCurrent(dlibMove.QuadPart);
	}
	return 0;
}

HRESULT __stdcall Win32::COMStream::SetSize(ULARGE_INTEGER libNewSize)
{
	DebugMsg("SetSize");
	return 0;
}

HRESULT __stdcall Win32::COMStream::CopyTo(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten)
{
	DebugMsg("CopyTo");
	void *buff = MAlloc((UIntOS)cb.QuadPart);
	UIntOS readSize;
	if (pcbRead)
		readSize = (UIntOS)(pcbRead->QuadPart = stm->Read(Data::ByteArray((UInt8*)buff, (UIntOS)cb.QuadPart)));
	else
		readSize = stm->Read(Data::ByteArray((UInt8*)buff, (UIntOS)cb.QuadPart));
	ULONG ul;
	pstm->Write(buff, (ULONG)readSize, &ul);
	if (pcbWritten)
		pcbWritten->QuadPart = ul;
	MemFree(buff);
	return 0;
}

HRESULT __stdcall Win32::COMStream::Commit(DWORD grfCommitFlags)
{
	DebugMsg("Commit");
	stm->Flush();
	return 0;
}

HRESULT __stdcall Win32::COMStream::Revert( void)
{
	DebugMsg("Revert");
	return 0;
}

HRESULT __stdcall Win32::COMStream::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	DebugMsg("LockRegion");
	return 0;
}

HRESULT __stdcall Win32::COMStream::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	DebugMsg("UnlockRegion");
	return 0;
}

HRESULT __stdcall Win32::COMStream::Stat(STATSTG *pstatstg, DWORD grfStatFlag)
{
	DebugMsg("Stat");
	if (grfStatFlag != STATFLAG_NONAME)
	{
		pstatstg->pwcsName = (LPOLESTR)CoTaskMemAlloc(8);
		Text::StrConcat(pstatstg->pwcsName, L"Mem");
	}
	else
	{
		pstatstg->pwcsName = 0;
	}
	pstatstg->type = STGTY_STREAM;
	pstatstg->cbSize.QuadPart = this->stm->GetLength();
	SYSTEMTIME sysTime;
	sysTime.wYear = dt->GetYear();
	sysTime.wMonth = dt->GetMonth();
	sysTime.wDay = dt->GetDay();
	sysTime.wHour = dt->GetHour();
	sysTime.wMinute = dt->GetMinute();
	sysTime.wSecond = dt->GetSecond();
	sysTime.wMilliseconds = dt->GetMS();
	SystemTimeToFileTime(&sysTime, &pstatstg->mtime);
	SystemTimeToFileTime(&sysTime, &pstatstg->ctime);
	SystemTimeToFileTime(&sysTime, &pstatstg->atime);
	pstatstg->grfMode = STGM_READWRITE;
	pstatstg->grfLocksSupported = 0;
	pstatstg->clsid = CLSID_NULL;
	pstatstg->grfStateBits = 0;
	pstatstg->reserved = 0;
	return S_OK;
}

HRESULT __stdcall Win32::COMStream::Clone(IStream **ppstm)
{
	DebugMsg("Clone");
	return 0;
}

