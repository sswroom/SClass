#ifndef _SM_WIN32_COMSTREAM
#define _SM_WIN32_COMSTREAM
#include "IO/SeekableStream.h"
#include "Data/DateTime.h"

namespace Win32
{
	class COMStream : public IStream
	{
	private:
		IO::SeekableStream *stm;
		Int32 cnt;
		Data::DateTime *dt;

	public:
		COMStream(IO::SeekableStream *stm);
		virtual ~COMStream();
		HRESULT __stdcall QueryInterface(const IID &riid, void **ppvObject);
		ULONG __stdcall AddRef(void);
		ULONG __stdcall Release(void);
		HRESULT __stdcall Read(void *pv, ULONG cb, ULONG *pcbRead);
		HRESULT __stdcall Write(const void *pv, ULONG cb, ULONG *pcbWritten);
		HRESULT __stdcall Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition);
		HRESULT __stdcall SetSize(ULARGE_INTEGER libNewSize);
		HRESULT __stdcall CopyTo(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten);
		HRESULT __stdcall Commit(DWORD grfCommitFlags);
		HRESULT __stdcall Revert( void);
		HRESULT __stdcall LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
		HRESULT __stdcall UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
		HRESULT __stdcall Stat(STATSTG *pstatstg, DWORD grfStatFlag);
		HRESULT __stdcall Clone(IStream **ppstm);
	};
};
#endif
