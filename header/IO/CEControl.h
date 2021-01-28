//required rapi.lib
#ifndef _SM_IO_CECONTROL
#define _SM_IO_CECONTROL
#include "Data/DateTime.h"
#include "IO/Path.h"
#include "IO/FileStream.h"

namespace IO
{
	class CEControl
	{
	private:
		Bool inited;
	public:
		CEControl();
		~CEControl();

		Bool IsError();
		void *FindFile(const WChar *path);
		WChar *FindNextFile(WChar *buff, void *session, Data::DateTime *modTime, IO::Path::PathType *pt);
		void FindFileClose(void *session);
		IO::Path::PathType GetPathType(const WChar *path);
		Bool DeleteFile(const WChar *ceFile);
		Bool DeleteDirectory(const WChar *ceDir);
		Bool IsDirectoryExist(const WChar *ceDir);
		Bool CreateDirectory(const WChar *ceDir);
		IO::FileStream *OpenFile(const WChar *fileName, IO::FileStream::FileMode mode, IO::FileStream::FileShare share);
	};

	class CEFileStream : public IO::FileStream
	{
	public:
		CEFileStream(void *hand, Int64 currPos);
		virtual ~CEFileStream();
		virtual OSInt Read(UInt8 *buff, OSInt size);
		virtual OSInt Write(const UInt8 *buff, OSInt size);

		virtual void *BeginRead(UInt8 *buff, OSInt size, Sync::Event *evt);
		virtual OSInt EndRead(void *reqData, Bool toWait);
		virtual void CancelRead(void *reqData);
		virtual void *BeginWrite(const UInt8 *buff, OSInt size, Sync::Event *evt);
		virtual OSInt EndWrite(void *reqData, Bool toWait);
		virtual void CancelWrite(void *reqData);

		virtual Int32 Flush();
		virtual void Close();
		virtual Int64 Seek(IO::SeekableStream::SeekType origin, Int64 position);
		virtual Int64 GetPosition();
		virtual Int64 GetLength();

		virtual Bool IsError();
		virtual Int32 GetErrCode();
		virtual void GetFileTimes(Data::DateTime *creationTime, Data::DateTime *lastAccessTime, Data::DateTime *lastWriteTime);
		virtual void SetFileTimes(Data::DateTime *creationTime, Data::DateTime *lastAccessTime, Data::DateTime *lastWriteTime);
	};
};
#endif
