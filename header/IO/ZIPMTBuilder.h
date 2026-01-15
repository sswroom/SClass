#ifndef _SM_IO_ZIPMTBUILDER
#define _SM_IO_ZIPMTBUILDER
#include "Data/SyncLinkedList.h"
#include "IO/StreamData.h"
#include "IO/ZIPBuilder.h"
#include "Sync/Event.h"
#include "Sync/Thread.h"

namespace IO
{
	class ZIPMTBuilder
	{
	private:
		enum class ThreadState
		{
			NotRunning,
			Idle,
			Processing,
			Stopped
		};

		struct ThreadStat
		{
			ZIPMTBuilder *me;
			Sync::Event *evt;
			ThreadState status;
		};

		struct FileTask
		{
			NN<Text::String> fileName;
			UInt8 *fileBuff;
			UIntOS fileSize;
			Data::Timestamp lastModTime;
			Data::Timestamp lastAccessTime;
			Data::Timestamp createTime;
			Data::Compress::Inflate::CompressionLevel compLevel;
			UInt32 unixAttr;
		};
	private:
		ZIPBuilder zip;
		UIntOS threadCnt;
		Sync::Thread **threads;
		Bool toStop;
		Sync::Event mainEvt;
		Data::SyncLinkedList taskList;

		static void __stdcall ThreadProc(NN<Sync::Thread> thread);
		static void FreeTask(NN<FileTask> task);
		void AddTask(NN<FileTask> task);
	public:
		ZIPMTBuilder(NN<IO::SeekableStream> stm, IO::ZIPOS os);
		~ZIPMTBuilder();

		Bool AddFile(Text::CStringNN fileName, NN<IO::SeekableStream> stm, Data::Timestamp lastModTime, Data::Timestamp lastAccessTime, Data::Timestamp createTime, Data::Compress::Inflate::CompressionLevel compLevel, UInt32 unixAttr);
		Bool AddFile(Text::CStringNN fileName, NN<IO::StreamData> fd, Data::Timestamp lastModTime, Data::Timestamp lastAccessTime, Data::Timestamp createTime, Data::Compress::Inflate::CompressionLevel compLevel, UInt32 unixAttr);
		Bool AddDir(Text::CStringNN dirName, Data::Timestamp lastModTime, Data::Timestamp lastAccessTime, Data::Timestamp createTime, UInt32 unixAttr);
		Bool AddDeflate(Text::CStringNN fileName, Data::ByteArrayR buff, UInt64 decSize, UInt32 crcVal, Data::Timestamp lastModTime, Data::Timestamp lastAccessTime, Data::Timestamp createTime, UInt32 unixAttr);
	};
}
#endif
