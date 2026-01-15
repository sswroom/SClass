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
		IO::FileStream *OpenFile(const WChar *fileName, IO::FileMode mode, IO::FileShare share);
	};

	class CEFileStream : public IO::FileStream
	{
	public:
		CEFileStream(void *hand, Int64 currPos);
		virtual ~CEFileStream();
		virtual IntOS Read(UInt8 *buff, IntOS size);
		virtual IntOS Write(const UInt8 *buff, IntOS size);

		virtual Optional<StreamReadReq> BeginRead(const Data::ByteArray &buff, NN<Sync::Event> evt);
		virtual UIntOS EndRead(NN<StreamReadReq> reqData, Bool toWait, OutParam<Bool> incomplete);
		virtual void CancelRead(NN<StreamReadReq> reqData);
		virtual Optional<StreamWriteReq> BeginWrite(Data::ByteArrayR buff, NN<Sync::Event> evt);
		virtual UIntOS EndWrite(NN<StreamWriteReq> reqData, Bool toWait);
		virtual void CancelWrite(NN<StreamWriteReq> reqData);

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
}
#endif
