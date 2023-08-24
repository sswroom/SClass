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
			NotNullPtr<Text::String> fileName;
			UInt8 *fileBuff;
			UOSInt fileSize;
			Int64 fileTimeTicks;
			Data::Compress::Inflate::CompressionLevel compLevel;
		};
	private:
		ZIPBuilder zip;
		UOSInt threadCnt;
		Sync::Thread **threads;
		Bool toStop;
		Sync::Event mainEvt;
		Data::SyncLinkedList taskList;

		static void __stdcall ThreadProc(NotNullPtr<Sync::Thread> thread);
		static void FreeTask(FileTask *task);
		void AddTask(FileTask *task);
	public:
		ZIPMTBuilder(NotNullPtr<IO::SeekableStream> stm);
		~ZIPMTBuilder();

		Bool AddFile(Text::CString fileName, IO::SeekableStream *stm, Int64 fileTimeTicks, Data::Compress::Inflate::CompressionLevel compLevel);
		Bool AddFile(Text::CString fileName, NotNullPtr<IO::StreamData> fd, Int64 fileTimeTicks, Data::Compress::Inflate::CompressionLevel compLevel);
	};
}
#endif
